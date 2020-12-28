#include "box.hpp"
#define RENDER_ACC 200.0f

Box::Box(vec3f size_)
: size(size_), vertices(), position(0.0f, 0.0f, 0.0f)
{
    position.z -= 5;
    position.y -= 7;
    renderer = new render::Renderer(std::vector<render::RenderParticle>(), 0.2, true);
    this->draw();
}

void Box::draw()
{
    auto pos = position;
    for (int i = -4; i < RENDER_ACC + 4; ++i)
    {
        vertices.push_back({vec3f(i * size.x / RENDER_ACC, -0.2, -0.2) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(i * size.x / RENDER_ACC, -0.2, size.z + 0.2) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(i * size.x / RENDER_ACC, size.y + 0.2, -0.2) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(i * size.x / RENDER_ACC, size.y + 0.2, size.z + 0.2) + pos, vec3f(1, 1, 1)});

        vertices.push_back({vec3f(-0.2, i * size.y / RENDER_ACC, -0.2) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(-0.2, i * size.y / RENDER_ACC, size.z + 0.2) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(size.x + 0.2, i * size.y / RENDER_ACC, -0.2) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(size.x + 0.2, i * size.y / RENDER_ACC, size.z + 0.2) + pos, vec3f(1, 1, 1)});

        vertices.push_back({vec3f(-0.2, -0.2, i * size.z / RENDER_ACC) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(-0.2, size.y + 0.2, i * size.z / RENDER_ACC) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(size.x + 0.2, -0.2, i * size.z / RENDER_ACC) + pos, vec3f(1, 1, 1)});
        vertices.push_back({vec3f(size.x + 0.2, size.y + 0.2, i * size.z / RENDER_ACC) + pos, vec3f(1, 1, 1)});
    }
        
    renderer->setParticles(vertices);
}

Box::~Box()
{
    delete renderer;
}

void Box::Render()
{
    renderer->draw();
}