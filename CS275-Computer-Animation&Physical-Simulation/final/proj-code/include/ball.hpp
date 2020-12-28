#pragma once
#include "base.hpp"
#include "render.hpp"
// #include "sph.hpp"

#define MODE_LINE 1
#define MODE_CUBE 2

class Ball
{
public:
	Ball(vec3f pos, float a);
	~Ball();
	void Render();
	void Update();
    vec3f position;
	vec3f velocity;
	float a;
	bool should_drop;
private:
	void draw();
	render::Renderer* renderer;
	std::vector<render::RenderParticle> vertices;
};