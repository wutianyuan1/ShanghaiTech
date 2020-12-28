#pragma once
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GL/glew.h>
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
