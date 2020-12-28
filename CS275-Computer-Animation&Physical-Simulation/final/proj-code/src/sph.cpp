#include "sph.hpp"
#define PI 3.14159265359
#define MAX_PARTICLE 25000
#define INVALID_HASH (-1u)
#define POW2(x) ((x) *(x))
#define POW3(x) ((x)*(x)*(x))
#define POW6(x) (POW3(x) * POW3(x))
#define POW9(x) (POW6(x) * POW3(x))
#define DIST2(p1, p2) ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z))

using namespace config;
using namespace render;

inline void print_vec(vec3f v)
{
    printf("(%f %f %f)\n", v.x, v.y, v.z);
}

inline float norm(vec3f v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

SPH::SPH(vec3f worldSize, float cubeSize)
: _boxSize(worldSize), _cubeSize(cubeSize), _numParticles(0) , _renderChunk(new Renderer(std::vector<RenderParticle>(), 1.0f, true))
{
    _gridSize.x = (int)ceil(_boxSize.x / _cubeSize);
    _gridSize.y = (int)ceil(_boxSize.y / _cubeSize);
    _gridSize.z = (int)ceil(_boxSize.z / _cubeSize);
    _hashTable = new ParticleHash(_gridSize.x, _gridSize.y, _gridSize.z, cubeSize);
    _particles = new Particle[MAX_PARTICLE];
    for (float i = _gridSize.x * 1 / 4; i < _gridSize.x * 3 / 4; i += 0.35)
        for (float j = _gridSize.y * 1 / 4; j < _gridSize.y * 3 / 4; j += 0.35)
            for (float k =_gridSize.z * 1 / 4; k < _gridSize.z * 3 / 4; k += 0.35)
                if ((i - _gridSize.x/2) * (i - _gridSize.x / 2) + (j - _gridSize.y /2) * (j - _gridSize.y/2) + \
                    (k - _gridSize.z/2) * (k - _gridSize.z/2) < (_gridSize.x * _gridSize.x / 16) )
                    NewParticle(vec3f(i*_cubeSize, j*_cubeSize, k*_cubeSize), vec3f(0.0f));

    _kernelSize = _cubeSize;
    _particleMass = 0.01;
    _POLY6 = 315.0 / (64.0 * PI * POW9(_kernelSize));
    _SPIKY = -45.0 / (PI * POW6(_kernelSize));
    _VISCO = 45.0  / (PI * POW6(_kernelSize));
    _GRADIENT_POLY6 = -945.0 / (32 * PI * POW9(_kernelSize));
    _LAPLACE_POLY6  = -945.0 / (8 * PI * POW9(_kernelSize));
    _DENSITY = 2 * _particleMass * _POLY6 * POW6(_kernelSize);
    _LAPLACE_KERNEL = _particleMass * _LAPLACE_POLY6 * _kernelSize * _kernelSize * (-3.0 / 4.0 * _kernelSize * _kernelSize);
    _renderParticles.resize(_numParticles);
    printf ("Initialization done, number of particles = %d\n", _numParticles);
}

SPH::~SPH()
{
    delete[] _particles;
    delete _renderChunk;
    delete _hashTable;
}

void SPH::NewParticle(vec3f position, vec3f velocity)
{
    if (_numParticles >= MAX_PARTICLE)
        throw std::runtime_error("Too many particles\n");
    Particle* curr = &_particles[_numParticles];
    curr->position = position;
    curr->velocity = velocity;
    curr->density = INIT_DENSITY;
    curr->next = nullptr;
    curr->pressure = 0;
    INIT_ZERO_VEC(curr->acceleration);
    _numParticles++;
}

void SPH::AddWater()
{
    printf ("Add water to system\n");
    for (float i = _gridSize.x * 2 / 4; i < _gridSize.x * 3 / 4; i += 0.35)
        for (float j = _gridSize.y * 2 / 4; j < _gridSize.y * 3 / 4; j += 0.35)
            for (float k =_gridSize.z * 2 / 4; k < _gridSize.z * 3 / 4; k += 0.35)
                if ((i - 5 * _gridSize.x / 8) * (i - 5 * _gridSize.x / 8) + (j - 5 * _gridSize.y / 8) * (j - 5 * _gridSize.y / 8) + \
                    (k - 5 * _gridSize.z / 8) * (k - 5 * _gridSize.z / 8) < (_gridSize.x * _gridSize.x / 64) )
                        NewParticle(vec3f(i*_cubeSize - 0.1, j*_cubeSize, k*_cubeSize - 0.1), vec3f(0.0f));
    _renderParticles.resize(_numParticles);
}

void SPH::UpdatePressureAndDensity()
{
    #pragma omp parallel for
    for (int i = 0; i < _numParticles; ++i)
    {
        Particle* curr = &_particles[i];
        vec3i idx = Pos2GridIdx(curr->position, _cubeSize);
        curr->pressure = 0;
        curr->density = 0;
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; y++)
                for (int z = -1; z <= 1; z++)
                {
                    vec3i neighborIdx(idx.x + x, idx.y + y, idx.z + z);
                    uint32_t hashVal = _hashTable->HashHashFunc(neighborIdx);
                    if (hashVal != INVALID_HASH)
                    {
                        for (Particle* iter = _hashTable->Query(hashVal); iter != nullptr; iter = iter->next)
                        {
                            float d2 = DIST2(iter->position, curr->position);
                            if (d2 < _kernelSize * _kernelSize && d2 > 1e-12)
                                curr->density = curr->density + _POLY6 * _particleMass * POW3((_kernelSize * _kernelSize) - d2);
                        }
                    }
                }
        }
        curr->density = curr->density + _DENSITY;
        curr->pressure = GAS_CONST * (POW6((curr->density / INIT_DENSITY)) * (curr->density / INIT_DENSITY) - 1);
    }
}

void SPH::UpdateForce()
{
    #pragma omp parallel for
    for (int i = 0; i < _numParticles; ++i)
    {    
        float r, v, rest, F, tmp;
        vec3f gradient(0, 0, 0), rv(0, 0, 0), rp(0, 0, 0);
        float laplacian = 0, d2 = 0;
        Particle* curr = &_particles[i];
        vec3i idx = Pos2GridIdx(curr->position, _cubeSize);
        INIT_ZERO_VEC(curr->acceleration);
        INIT_ZERO_VEC(gradient);
        laplacian = _LAPLACE_KERNEL / curr->density;
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; y++)
                for (int z = -1; z <= 1; z++)
                {
                    vec3i neighborIdx(idx.x + x, idx.y + y, idx.z + z);           
                    uint32_t hashVal = _hashTable->HashHashFunc(neighborIdx);
                    if (hashVal != INVALID_HASH)
                    {
                        for (Particle* iter = _hashTable->Query(hashVal); iter != nullptr; iter = iter->next)
                        {
                            d2 = DIST2(iter->position, curr->position);
                            rp = curr->position - iter->position;
                            rv = iter->velocity - curr->velocity;
                            if (d2 < _kernelSize * _kernelSize && d2 > 1e-10)
                            {
                                r = sqrt(d2), v = (_particleMass / iter->density) * 0.5;
                                rest = _kernelSize - r;
                                F = v * (iter->pressure + curr->pressure) * _SPIKY * rest * rest;
                                curr->acceleration = curr->acceleration - rp * F / r;
                                F = v * VISCOSITY * _VISCO * (_kernelSize - r);
                                curr->acceleration = curr->acceleration + rv * F;
                                tmp = POW2(POW2(_kernelSize) - d2);
                                gradient = gradient + ((-1) * _GRADIENT_POLY6 * v * tmp) * rp;
                            }
                        }
                    }
                }
        curr->surface_tension = sqrt((gradient.x * gradient.x) + (gradient.y * gradient.y) + (gradient.z * gradient.z));
        if (curr->surface_tension > SURFACE_NORM)
            curr->acceleration += (SURFACE_COEF * laplacian / (float)curr->surface_tension) * gradient;
    }
}

void SPH::Update(Ball* ball)
{
    _hashTable->Build(_particles, _numParticles);
    UpdatePressureAndDensity();
    UpdateForce();
    vec3f c, v;
    float a;
    if (ball != nullptr)
    {
        c = vec3f(ball->position.x / 10.0, (ball->position.y + 7.0) / 10.0, (ball->position.z + 5.0f) / 10);
        v = ball->velocity;
        a = ball->a / 10.0;
    }
        
    #pragma omp parallel for
    for (int i = 0; i < _numParticles; ++i)
    {
        Particle* curr = &_particles[i];
        curr->velocity += curr->acceleration * TIME_STEP / (float)curr->density + GRAVITY * TIME_STEP;
        curr->position = curr->position + curr->velocity * TIME_STEP;
        if (ball != nullptr)
        {
            vec3f p = curr->position;
            if ( POW2(p.x - c.x) + POW2(p.y - c.y) + POW2(p.z - c.z) < a * a / 4.0 )
            {
                vec3f rp(p.x - c.x, p.y - c.y, p.z - c.z), rv(curr->velocity.x - v.x, curr->velocity.y - v.y, curr->velocity.z - v.z);
                rp = glm::normalize(rp);
                curr->position = c + rp * a / 2.0f;
                float cosx = (rv.x * rp.x + rv.y * rp.y + rv.z * rp.z) / norm(rv);
                cosx = std::min(cosx, 0.999f); // prevent FP errors
                float sinx = sqrt(1 - cosx * cosx);
                curr->velocity = v + rv * cosx + rv * sinx;
                ball->velocity = v - rv * cosx * 0.02f;
            }    
        }
        if (curr->position.x < 0)
        {
            curr->velocity.x = curr->velocity.x * DAMPING;
            curr->position.x = 0;
        }
        if (curr->position.y < 0)
        {
            curr->velocity.y = curr->velocity.y * DAMPING;
            curr->position.y = 0;
        }
        if (curr->position.z < 0)
        {
            curr->velocity.z = curr->velocity.z * DAMPING;
            curr->position.z = 0.0;        }
        if (curr->position.x >= _boxSize.x - 0.001)
        {
            curr->position.x = _boxSize.x - 0.001;
            curr->velocity.x = curr->velocity.z * DAMPING;;
        }
        if (curr->position.y >= _boxSize.y - 0.001)
        {
            curr->position.y = _boxSize.y - 0.001;
            curr->velocity.y = curr->velocity.y * DAMPING;
        }
        if (curr->position.z >= _boxSize.z - 0.001)
        {
            curr->position.z = _boxSize.z - 0.001;
            curr->velocity.z = curr->velocity.z * DAMPING;
        }
    }


    for(int i = 0; i < _numParticles; i++)
    {
        _renderParticles[i] = {_particles[i].position * 10.0f,  glm::vec3(102.0/255.0, 204.0/255.0, 1.0)};
        _renderParticles[i].position.z -= 5.0f;
        _renderParticles[i].position.y -= 7.0f;
    }      
    _renderChunk->setParticles(_renderParticles);
}

void SPH::Render()
{
    _renderChunk->draw();
}
