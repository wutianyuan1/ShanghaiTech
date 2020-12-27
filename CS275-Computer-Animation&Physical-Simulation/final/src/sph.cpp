#include "sph.hpp"
#define PI 3.14159265359
#define INVALID_HASH (-1u)
#define POW2(x) ((x) *(x))
#define POW3(x) ((x)*(x)*(x))
#define POW6(x) (POW3(x) * POW3(x))
#define POW9(x) (POW6(x) * POW3(x))
#define DIST2(p1, p2) ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z))

using namespace config;

#define MAX_PARTICLE 25000

ParticleSystem::ParticleSystem(vec3f worldSize, float cubeSize)
: _worldSize(worldSize), _cubeSize(cubeSize), _numParticles(0), _renderChunk(new Chunk())
{
    printf ("construct begin\n");
    _gridSize.x = (int)ceil(_worldSize.x / _cubeSize);
    _gridSize.y = (int)ceil(_worldSize.y / _cubeSize);
    _gridSize.z = (int)ceil(_worldSize.z / _cubeSize);
    _hashTable = new ParticleHash(_gridSize.x, _gridSize.y, _gridSize.z, cubeSize);
    _particles = new Particle[MAX_PARTICLE];
    printf ("world size = (%f %f %f)\n", _worldSize.x, _worldSize.y, _worldSize.z);
    printf ("grid size = (%d %d %d)\n", _gridSize.x, _gridSize.y, _gridSize.z);
    printf ("cube size = (%f)\n", _cubeSize);
    for (float i = _gridSize.x * 1 / 4; i < _gridSize.x * 3 / 4; i += 0.35)
        for (float j = _gridSize.y * 1 / 4; j < _gridSize.y * 3 / 4; j += 0.35)
            for (float k =_gridSize.z * 1 / 4; k < _gridSize.z * 3 / 4; k += 0.35)
                if ((i - _gridSize.x/2) * (i - _gridSize.x / 2) + (j - _gridSize.y /2) * (j - _gridSize.y/2) + \
                    (k - _gridSize.z/2) * (k - _gridSize.z/2) < (_gridSize.x * _gridSize.x / 16) )
                    NewParticle(vec3f(i*_cubeSize, j*_cubeSize, k*_cubeSize), vec3f(0.0f));

    _data = new float** [100];
    for (int x = 0; x < 100; x++)
    {
        _data[x] = new float* [100];
        for (int y = 0; y < 100; y++)
            _data[x][y] = new float[100];        
    }

    _kernelSize = _cubeSize;
    _particleMass = 0.01;
    _POLY6 = 315.0 / (64.0 * PI * POW9(_kernelSize));
    _SPIKY = -45.0 / (PI * POW6(_kernelSize));
    _VISCO = 45.0  / (PI * POW6(_kernelSize));
    _GRADIENT_POLY6 = -945.0 / (32 * PI * POW9(_kernelSize));
    _LAPLACE_POLY6  = -945.0 / (8 * PI * POW9(_kernelSize));
    _DENSITY = 2 * _particleMass * _POLY6 * POW6(_kernelSize);
    _LAPLACE_COLOR = _particleMass * _LAPLACE_POLY6 * _kernelSize * _kernelSize * (-3.0 / 4.0 * _kernelSize * _kernelSize);
}

ParticleSystem::~ParticleSystem()
{
    for (int i = 0; i < 100; ++i)
    {
        for (int j = 0; j < 100; ++j)
            delete[] _data[i][j];
        delete[] _data[i];
    }
    delete[] _data;
    delete[] _particles;
    delete _renderChunk;
    delete _hashTable;
}

void ParticleSystem::NewParticle(vec3f position, vec3f velocity)
{
    Particle* curr = &_particles[_numParticles];
    curr->position = position;
    curr->velocity = velocity;
    curr->density = INIT_DENSITY;
    curr->next = nullptr;
    curr->pressure = 0;
    INIT_ZERO_VEC(curr->acceleration);
    _numParticles++;
}

void ParticleSystem::UpdatePressureAndDensity()
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

void ParticleSystem::UpdateForce()
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
        laplacian = _LAPLACE_COLOR / curr->density;
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

void ParticleSystem::Update()
{
    _hashTable->Build(_particles, _numParticles);
    UpdatePressureAndDensity();
    UpdateForce();
    #pragma omp parallel for
    for (int i = 0; i < _numParticles; ++i)
    {
        Particle* curr = &_particles[i];
        curr->velocity += curr->acceleration * TIME_STEP / (float)curr->density + GRAVITY * TIME_STEP;
        curr->position = curr->position + curr->velocity * TIME_STEP;
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
        if (curr->position.x >= _worldSize.x - 0.001)
        {
            curr->position.x = _worldSize.x - 0.001;
            curr->velocity.x = curr->velocity.z * DAMPING;;
        }
        if (curr->position.y >= _worldSize.y - 0.001)
        {
            curr->position.y = _worldSize.y - 0.001;
            curr->velocity.y = curr->velocity.y * DAMPING;
        }
        if (curr->position.z >= _worldSize.z - 0.001)
        {
            curr->position.z = _worldSize.z - 0.001;
            curr->velocity.z = curr->velocity.z * DAMPING;
        }
    }
}

void ParticleSystem::Render()
{
    Particle* iter;
    int x, y, z, nx, ny, nz;
    #pragma omp parallel for
    for (int i = 0; i < _numParticles; i++)
    {
        x = int((_particles[i].position.x * 100) / _worldSize.x);
        y = int((_particles[i].position.y * 100) / _worldSize.y);
        z = int((_particles[i].position.z * 100) / _worldSize.z);
        for (int xx = -1; xx <= 1; xx++)
            for (int yy = -1; yy <= 1; yy++)
                for (int zz = -1; zz <= 1; zz++)
                {        
                    nx = std::max(0, std::min(99, x + xx));
                    ny = std::max(0, std::min(99, y + yy));
                    nz = std::max(0, std::min(99, z + zz));
                    _data[nx][ny][nz] = 0.8;
                }
                    
    }
    _renderChunk->render(_data);
    for (int i = 0; i < _numParticles; i++)
    {
        x = int((_particles[i].position.x * 100) / _worldSize.x);
        y = int((_particles[i].position.y * 100) / _worldSize.y);
        z = int((_particles[i].position.z * 100) / _worldSize.z);
        for (int xx = -1; xx <= 1; xx++)
            for (int yy = -1; yy <= 1; yy++)
                for (int zz = -1; zz <= 1; zz++)
                {
                    nx = std::max(0, std::min(99, x + xx));
                    ny = std::max(0, std::min(99, y + yy));
                    nz = std::max(0, std::min(99, z + zz));
                    _data[nx][ny][nz] = 0;
                }
    }
}
