#ifndef SIMULATION_H
#define SIMULATION_H

#include "base.hpp"

namespace render
{
struct RenderParticle
{
    glm::vec3 position;
    glm::vec3 color;
};

class Renderer
{
private:
    std::vector<RenderParticle> m_particles;
    float m_particle_radius;
    unsigned int m_vao_id;
    unsigned int m_vbo_id;
    bool m_has_vao;

    // for artificial motion until I get an actual simulation together
    float m_time;
    std::vector<RenderParticle> m_particles_offset;

public:
    void prepareRendering();
    void draw();

    void updateOffsetsSinewave(float dt, float strength);
    void setParticles(std::vector<RenderParticle> particles);

    void initialize();
    Renderer(std::vector<RenderParticle> particles, float particle_radius, bool initialize_now = true);

private:
    Renderer();
};

namespace scenarios
{
std::vector<RenderParticle> ball(float side_length, std::size_t particles_per_side, glm::vec3 color1, glm::vec3 color2);
}

} // namespace render

#endif // SIMULATION_H