#pragma once
#include "base.hpp"

vec3i Pos2GridIdx(const vec3f& pos, const float cubeSize);

class ParticleHash
{
public:
    ParticleHash(int w, int h, int d, float cubeSize);
    ~ParticleHash();
    void Build(Particle* particles, int numParticles);
    Particle* Query(uint32_t hashValue);
    uint32_t HashHashFunc(vec3i gridIdx);
    Particle** _data;
private:

    int _size;
    int _h, _w, _d;
    float _cubeSize;
};
