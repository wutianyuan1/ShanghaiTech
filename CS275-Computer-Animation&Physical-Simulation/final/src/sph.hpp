#pragma once
#include "base.hpp"
#include "hash.hpp"
#include "render/chunk.h"

namespace config
{
    extern float INIT_DENSITY;
    extern float DAMPING;
    extern float VISCOSITY;
    extern float GAS_CONST;
    extern float SURFACE_NORM;
    extern float SURFACE_COEF;
    extern float TIME_STEP; 
    extern vec3f GRAVITY;   
}


#define INIT_ZERO_VEC(vec) { vec.x =0; vec.y =0; vec.z =0; }

class ParticleSystem
{
public:
    ParticleSystem(vec3f worldSize, float cubeSize);
    ~ParticleSystem();
    void Update();
    void Render();
    void NewParticle(vec3f position, vec3f velocity);
    void UpdatePressureAndDensity();
    void UpdateForce();

private:
    Particle* _particles;
    Chunk* _renderChunk;
    ParticleHash* _hashTable;
    int _numParticles;
    vec3f _worldSize;
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
    float _LAPLACE_COLOR;

    float*** _data;
};
