#pragma once
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <GLFW/glfw3.h>

typedef glm::vec3 vec3f;
typedef glm::vec<3, int> vec3i;

struct Particle
{
    vec3f position;
    vec3f velocity;
    vec3f acceleration;
    
    double density;
    double pressure;
    double surface_tension;
    Particle* next;
};

