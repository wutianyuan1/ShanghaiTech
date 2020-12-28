#pragma once
#include "base.hpp"
#include "hash.hpp"
#include "ball.hpp"
#include "render.hpp"

#define INIT_ZERO_VEC(vec) { vec.x =0; vec.y =0; vec.z =0; }

class SPH
{
public:
    SPH(vec3f worldSize, float cubeSize);
    ~SPH();
    void Update(Ball* ball=nullptr);
    void Render();
    void AddWater();
    void NewParticle(vec3f position, vec3f velocity);
private:

    void UpdatePressureAndDensity();
    void UpdateForce();

private:
    Particle* _particles;
    render::Renderer* _renderChunk;
    ParticleHash* _hashTable;
    int _numParticles;
    vec3f _boxSize;
    vec3f _gravity;
    vec3i _gridSize;
    float _cubeSize;
    float _particleMass;
    float _kernelSize;
    float _POLY6;
    float _SPIKY;
    float _VISCO;
    float _GRADIENT_POLY6;
    float _LAPLACE_POLY6;
    float _DENSITY;
    float _LAPLACE_KERNEL;
    std::vector<render::RenderParticle> _renderParticles;
};
