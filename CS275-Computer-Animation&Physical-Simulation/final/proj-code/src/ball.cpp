#include "ball.hpp"
#define RENDER_ACC 50.0f

Ball::Ball(vec3f pos, float a_)
: position(pos), a(a_), vertices(), velocity(vec3f(0, 0, 0)), should_drop(false)
{
    position.z -= 5;
    position.y -= 7;
    renderer = new render::Renderer(std::vector<render::RenderParticle>(), 0.2, true);
}

void Ball::draw()
{
    auto pos = position;
    vertices.clear();
    for (int i = 0; i < int(RENDER_ACC) + 1; i++)
        for (int j = 0; j < int(RENDER_ACC) + 1; j++)
            for (int k = 0 ; k < int(RENDER_ACC) + 1; k++)
            {
                vec3f p(i * a / RENDER_ACC - a / 2, j * a / RENDER_ACC - a / 2, k * a / RENDER_ACC - a / 2);
                float d = p.x * p.x + p.y * p.y + p.z * p.z;
                if (d < a * a / 4.0 && d > a * a / 4.5)
                    vertices.push_back({pos + p, vec3f(1, 0, 0)});
            }
    renderer->setParticles(vertices);
}

Ball::~Ball()
{
    delete renderer;
}

void Ball::Update()
{
    if (should_drop)
    {
        velocity = velocity + 2.0f * config::GRAVITY * config::TIME_STEP;
        position = position + velocity * config::TIME_STEP;
        if (position.y - a/2 < -7.0) // hit bottom od box
        {
            velocity = velocity * config::DAMPING;
            position.y = -7.0 + a/2;
        }        
    }
}

void Ball::Render()
{
    this->draw();
    renderer->draw();
}