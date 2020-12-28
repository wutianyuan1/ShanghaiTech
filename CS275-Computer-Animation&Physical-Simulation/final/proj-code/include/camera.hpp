#pragma once
#include "base.hpp"

// A camera that looks at the origin and rotates by mouse movement.
class Camera
{
private:
    float m_dist_from_origin;
    float m_sensitivity;
    float m_zoom_speed;

    float m_yaw;
    float m_pitch;
    glm::vec3 m_facing;

public:
    void handleMouse(float x_diff, float y_diff);
    void handleScroll(float diff);
    void rotate(float yaw, float pitch);
    glm::mat4 getViewMatrix();

    Camera(float dist_from_origin, float sensitivity = 1.0f, float zoom_speed = 0.1f, float yaw = 0.0f, float pitch = 0.0f);
};
