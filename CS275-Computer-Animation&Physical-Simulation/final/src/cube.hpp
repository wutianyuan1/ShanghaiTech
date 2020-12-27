#pragma once
#include "base.hpp"

#define MODE_LINE 1
#define MODE_CUBE 2

struct CubeVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
};

class Cube
{
public:
	Cube(int mode);
	~Cube();
	void render();
    vec3f position;
private:
	unsigned int vaoId, vboId;
	std::vector<CubeVertex> vertices;
    int _mode;
};