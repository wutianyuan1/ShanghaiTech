#include <iostream>
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Dense>
#include "rigidBody.hpp"

const GLfloat dt = 0.02;
const GLfloat Cr = 1.0;
#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))

template <typename T>
RigidBody<T>::RigidBody() : m_mass(1) {};

template <typename T>
RigidBody<T>::RigidBody(T* polygon, GLfloat mass) : m_polygon(std::move(polygon)), m_mass(mass) {};

template <typename T>
RigidBody<T>::RigidBody(std::shared_ptr<T> polygon) : m_mass(1), m_polygon(std::move(polygon)) {};

template <typename T>
Vec3f RigidBody<T>::getVelocity() const { return m_velocity; }

template <typename T>
Vec3f RigidBody<T>::getForce() const { return m_force; }

template <typename T>
Vec3f RigidBody<T>::getAngularVelocity() const { return m_angularVelocity; }

template <typename T>
Vec3f RigidBody<T>::getTorque() const { return m_torque; }

template <typename T>
void RigidBody<T>::setVelocity(Vec3f&& velocity)  {
    m_velocity = velocity; 
}

template <typename T>
void RigidBody<T>::setVelocity(Vec3f& velocity)  { this->setVelocity(std::move(velocity)); }

template <typename T>
void RigidBody<T>::setForce(Vec3f&& force) { m_force = force; }

template <typename T>
void RigidBody<T>::setForce(Vec3f& force) { this->setForce(std::move(force)); }

template <typename T>
void RigidBody<T>::setAngularVelocity(Vec3f&& angularVelocity) { m_angularVelocity = angularVelocity; }

template <typename T>
void RigidBody<T>::setAngularVelocity(Vec3f& angularVelocity) { this->setAngularVelocity(std::move(angularVelocity)); }

template <typename T>
void RigidBody<T>::setTorque(Vec3f&& torque) { m_torque = torque; }

template <typename T>
void RigidBody<T>::setTorque(Vec3f& torque) { this->setTorque(std::move(torque)); }

template <typename T>
void RigidBody<T>::show() { m_polygon->show(); }

template <typename T>
bool RigidBody<T>::collide(RigidBody<T> other, Vec3f & pt, Vec3f & norm) 
{ 
    return m_polygon->collide( &(*other.m_polygon), pt, norm); 
}

template <typename T>
void RigidBody<T>::handleCollision(RigidBody<T> & other, Vec3f & pt, Vec3f & norm)
{
    (void) pt; // clang n**l, c***gb
    Vec3f v_rel = m_velocity - other.m_velocity;
    Vec3f v_norm = v_rel * norm;
    float v_norm_val = v_norm.x + v_norm.y + v_norm.z / norm.len();
    v_norm = norm * v_norm_val / other.m_polygon->getScale();
    Vec3f v1 = (v_rel * other.m_mass + v_rel * other.m_mass * Cr) / (m_mass + other.m_mass) * (-1) + m_velocity;
    Vec3f v2 = (v_rel * other.m_mass - v_rel * m_mass * Cr) / (m_mass + other.m_mass) + m_velocity;
    this->setVelocity(std::move(v1));
    other.setVelocity(std::move(v2));
}

template <typename T>
void RigidBody<T>::update()
{
    // update velocity by given force, a = F/m and dv = a*dt
    m_velocity = m_velocity + (m_force / m_mass) * dt;

    // update angularVelocity by given torque, \beta = M/J, w = w + \beta*dt
    const GLfloat* R = m_polygon->getRotateMat();
    Eigen::Matrix3f Rmat;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            Rmat(i, j) = R[i * 4 + j];
    Eigen::Matrix3f I;
    I.setIdentity();
    I(0, 0) = I(1, 1) = I(2, 2) = m_mass * (2*m_polygon->getScale()) * (2*m_polygon->getScale()) / 6;
    Eigen::Matrix3f I_global = Rmat * I * Rmat.transpose();
    Eigen::Vector3f tau(m_torque.x, m_torque.y, m_torque.z);
    tau = I_global.inverse() * tau * dt;
    m_angularVelocity = m_angularVelocity + Vec3f(tau(0), tau(1), tau(2));

    // update position
    auto newXYZ = m_polygon->getXYZ() + m_velocity * dt;

    // update orientation
    // 1. calculate rotate matrix
    GLfloat tmpMat[4][4];
    memset(tmpMat, 0, sizeof(tmpMat));
    auto w = m_angularVelocity;
    tmpMat[0][0] = tmpMat[1][1] = tmpMat[2][2] = tmpMat[3][3] = 1;
    tmpMat[0][0] = -w.z*R[4] + w.y*R[8];
    tmpMat[0][1] = -w.z*R[5] + w.y*R[9];
    tmpMat[0][2] = -w.z*R[6] + w.y*R[10];
    tmpMat[1][0] = w.z*R[0] - w.x*R[8];
    tmpMat[1][1] = w.z*R[1] - w.x*R[9];
    tmpMat[1][2] = w.z*R[2] - w.x*R[10];
    tmpMat[2][0] = w.x*R[4] - w.y*R[0];
    tmpMat[2][1] = w.x*R[5] - w.y*R[1];
    tmpMat[2][2] = w.x*R[6] - w.y*R[2];
    for (int i = 0; i < 16; i++)
        tmpMat[i/4][i%4] = R[i] + tmpMat[i/4][i%4] * dt;

    // 2. normalize the rotation matrix
    for (int i = 0; i < 3; i++)
    {
        GLfloat len = std::sqrt(tmpMat[0][i]*tmpMat[0][i] + tmpMat[1][i]*tmpMat[1][i] + tmpMat[2][i]*tmpMat[2][i]);
        for (int j = 0; j < 3; j++)
            tmpMat[j][i] = tmpMat[j][i] / len;
    }
    for (int i = 0; i < 3; i++)
    {
        GLfloat len = std::sqrt(tmpMat[i][0]*tmpMat[i][0] + tmpMat[i][1]*tmpMat[i][1] + tmpMat[i][2]*tmpMat[i][2]);
        for (int j = 0; j < 3; j++)
            tmpMat[i][j] = tmpMat[i][j] / len;
    }

    tmpMat[0][3] = tmpMat[1][3] = tmpMat[2][3] = tmpMat[3][0] = tmpMat[3][1] = tmpMat[3][2] = 0;
    tmpMat[3][3] = 1;

    // reset position if out of bounds
    if (newXYZ.x > 2.5 || newXYZ.x < -2.5) newXYZ.x = -newXYZ.x;
    if (newXYZ.y > 2.5 || newXYZ.y < -2.5) newXYZ.y = -newXYZ.y;
    if (newXYZ.z > 2.5 || newXYZ.z < -2.5) newXYZ.z = -newXYZ.z;

    // set position & orientation
    m_polygon->setXYZ(newXYZ.x, newXYZ.y, newXYZ.z);
    m_polygon->setRotateMat(tmpMat);
}

