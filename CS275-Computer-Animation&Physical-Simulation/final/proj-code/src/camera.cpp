#include "camera.hpp"

void Camera::handleMouse(float x_diff, float y_diff)
{
    rotate(m_sensitivity * x_diff, m_sensitivity * y_diff);
}

void Camera::rotate(float yaw, float pitch)
{
    m_yaw += yaw;
    m_pitch += pitch;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    else if (m_pitch < -89.0f)
        m_pitch = -89.0f;
    if (m_yaw > 180.0f)
        m_yaw -= 360.0f;
    else if (m_yaw < -180.0f)
        m_yaw += -360.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
    front.y = sin(glm::radians(m_pitch));
    front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
    m_facing = glm::normalize(front);
}

void Camera::handleScroll(float diff)
{
    m_dist_from_origin -= diff * m_zoom_speed;
    if (m_dist_from_origin < 0.5f)
        m_dist_from_origin = 0.5f;
}

glm::mat4 Camera::getViewMatrix()
{
    glm::vec3 position = m_facing * m_dist_from_origin;
    return glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

Camera::Camera(float dist_from_origin, float sensitivity, float zoom_speed, float yaw, float pitch)
    : m_dist_from_origin(dist_from_origin),
      m_sensitivity(sensitivity),
      m_zoom_speed(zoom_speed),
      m_yaw(yaw),
      m_pitch(pitch)
{
    m_yaw = 0;
    m_pitch = -10;
    m_dist_from_origin = 18;
    handleMouse(0.0, 0.0); // initialize facing
}