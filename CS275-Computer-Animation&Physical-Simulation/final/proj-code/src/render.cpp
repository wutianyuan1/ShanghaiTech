#include "render.hpp"

namespace render
{
Renderer::Renderer(std::vector<RenderParticle> particles, float particle_radius, bool initialize_now)
    : m_particles(particles),
      m_particle_radius(particle_radius),
      m_time(0.0f),
      m_particles_offset(particles)
{
    if (initialize_now)
        initialize();
}

void Renderer::initialize()
{
    if (m_has_vao)
    {
        glDeleteVertexArrays(1, &m_vao_id);
        glDeleteBuffers(1, &m_vbo_id);
    }

    glGenBuffers(1, &m_vbo_id);

    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, m_particles.size() * sizeof(RenderParticle), &m_particles[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::draw()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_particles_offset.size() * sizeof(RenderParticle), &m_particles_offset[0]);
    glBindVertexArray(m_vao_id);
    glDrawArrays(GL_POINTS, 0, (GLsizei)m_particles_offset.size());
}

void Renderer::updateOffsetsSinewave(float dt, float strength)
{
    m_time += dt;
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        m_particles_offset[i].position.y = m_particles[i].position.y + (strength * glm::sin(m_particles[i].position.x + m_particles[i].position.z + m_time));
    }
}

void Renderer::setParticles(std::vector<RenderParticle> particles)
{
    m_particles = particles;
    m_particles_offset = particles;
    initialize();
}

} // namespace render