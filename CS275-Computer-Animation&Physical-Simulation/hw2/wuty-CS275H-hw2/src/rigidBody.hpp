#pragma once
#include <memory>
#include "helpers.hpp"
#include "basicTypes.hpp"

#define UNUSED __attribute__((unused))

template <typename T>
class RigidBody
{
public:
    RigidBody();
    RigidBody(T* body);
    RigidBody(std::shared_ptr<T> body);

    Vec3f getVelocity() const;
    Vec3f getForce() const;
    Vec3f getAngularVelocity() const;
    Vec3f getTorque() const;

    void setVelocity(Vec3f&& velocity);
    void setVelocity(Vec3f& velocity);
    void setForce(Vec3f&& force);
    void setForce(Vec3f& force);
    void setAngularVelocity(Vec3f&& angularVelocity);
    void setAngularVelocity(Vec3f& angularVelocity);
    void setTorque(Vec3f&& torque);
    void setTorque(Vec3f& torque);

    void show();
    void update();
    bool collide(RigidBody<T> other, Vec3f & pt, Vec3f & norm);
    void handleCollision(RigidBody<T> & other, Vec3f & pt UNUSED, Vec3f & norm);
// private:
    std::shared_ptr<T> m_polygon;
    Vec3f m_velocity;
    Vec3f m_force;
    Vec3f m_angularVelocity;
    Vec3f m_torque;
    GLfloat m_mass;
};

#include "rigidBody.ipp"

