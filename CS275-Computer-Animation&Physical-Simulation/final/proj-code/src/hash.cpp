#include "hash.hpp"
#define self (*this)

vec3i Pos2GridIdx(const vec3f& pos, const float cubeSize)
{
    vec3i idx;
    idx.x = int(floor(pos.x / cubeSize));
    idx.y = int(floor(pos.y / cubeSize));
    idx.z = int(floor(pos.z / cubeSize));
    return idx;
}

ParticleHash::ParticleHash(int h, int w, int d, float cubeSize)
: _h(h), _w(w), _d(d), _size(h * w * d), _cubeSize(cubeSize)
{
    self._data = new Particle* [self._size];
}

ParticleHash::~ParticleHash()
{
    delete[] self._data;
}

uint32_t ParticleHash::HashHashFunc(vec3i gridIdx)
{
    if (gridIdx.x < 0 || gridIdx.y < 0 || gridIdx.z < 0 
      || gridIdx.x >= self._h || gridIdx.y >= self._w || gridIdx.z >= self._d)
        return -1u;
    return gridIdx.x + gridIdx.y * self._h + gridIdx.z * self._h * self._w;
}

void ParticleHash::Build(Particle* particles, int numParticles)
{
    memset(self._data, 0, sizeof(Particle*) * self._size);

    for (int i = 0; i < numParticles; ++i)
    {
        vec3i idx = Pos2GridIdx(particles[i].position, self._cubeSize);
        uint32_t hashValue = HashHashFunc(idx);
        Particle* elem = &particles[i];
        if (self._data[hashValue] != nullptr)
        {
            elem->next = self._data[hashValue];
            self._data[hashValue] = elem;
        }
        else
        {
            self._data[hashValue] = elem;
            elem->next = nullptr;
        }
    }
}

Particle* ParticleHash::Query(uint32_t hashValue)
{
    return hashValue == -1u ? nullptr : self._data[hashValue];
}

#undef self
