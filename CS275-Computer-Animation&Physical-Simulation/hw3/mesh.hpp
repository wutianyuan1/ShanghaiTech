#pragma once
#include "base.hpp"

struct Triangle 
{
    Triangle() {};
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
    : p1_(p1), p2_(p2), p3_(p3)
    {
        glm::vec3 A = p2 - p1, B = p3 - p1;
        normal.x = A.y * B.z - A.z * B.y;
        normal.y = A.z * B.x - A.x * B.z;
        normal.z = A.x * B.y - A.y * B.x;
        float len = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal = normal / len;
    }
    Triangle& operator=(const Triangle& other) 
    {
        p1_ = other.p1_; p2_ = other.p2_; p3_ = other.p3_; 
        normal = other.normal; 
        return *this;
    }

    glm::vec3 p1_, p2_, p3_;
    glm::vec3 normal;
};


class Mesh {
public:
    /*  Functions  */
    Mesh();
    Mesh(glm::vec3* vertices, size_t n);

    void cleanUp();
    glm::vec3* getVertices();
    static glm::vec3* NewGrid(size_t n, GLfloat edgeLen);
    void draw();
private:
    /*  Mesh Data  */
    glm::vec3*  m_vbuff;
    size_t      m_meshsize;
};

std::ostream& operator<< (std::ostream& os, glm::vec3 & vec)
{
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

std::ostream& operator<< (std::ostream& os, Triangle & tri)
{
    os << "Triangle: [" << tri.p1_ << ", " << tri.p2_ << ", " << tri.p3_ << "]";
    return os;
}

