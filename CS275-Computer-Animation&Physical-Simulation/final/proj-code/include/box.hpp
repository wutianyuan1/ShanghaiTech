#pragma once
#include "base.hpp"
#include "render.hpp"

class Box
{
public:
	Box(vec3f size);
	~Box();
	void Render();
private:
	void draw();
    vec3f size;
    vec3f position;
	render::Renderer* renderer;
	std::vector<render::RenderParticle> vertices;
};