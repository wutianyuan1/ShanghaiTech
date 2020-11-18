#pragma once
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <GLUT/glut.h>


template <typename T>
struct Vec3
{
    Vec3() : x(0), y(0), z(0){};
    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {};
    Vec3(const Vec3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {};
    Vec3<T>& operator=(const Vec3& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }

    Vec3<T> operator+(T rhs) { return Vec3(x + rhs, y + rhs, z + rhs); }
    Vec3<T> operator+(Vec3 rhs) { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }

    Vec3<T> operator-(T rhs) { return Vec3(x - rhs, y - rhs, z - rhs); }
    Vec3<T> operator-(Vec3 rhs) { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }

    Vec3<T> operator*(T rhs) { return Vec3(x * rhs, y * rhs, z * rhs); }
    Vec3<T> operator*(Vec3 rhs) { return Vec3(x * rhs.x, y * rhs.y, z * rhs.z); }

    Vec3<T> operator/(T rhs) { return Vec3(x / rhs, y / rhs, z / rhs); }
    Vec3<T> operator/(Vec3 rhs) { return Vec3(x / rhs.x, y / rhs.y, z / rhs.z); }

    T len() const { return std::sqrt(x*x + y*y + z*z); }

    T x, y, z;
};

typedef Vec3<GLfloat> Vec3f;

class BasicObject
{
public:
    BasicObject() {};
    BasicObject(float) {};
    virtual ~BasicObject() {};

    virtual void setXYZ(GLfloat x, GLfloat y, GLfloat z) = 0;
    virtual void setRotateMat(GLfloat mat[4][4]) = 0;
    virtual Vec3f getXYZ() const = 0;
    virtual const GLfloat* getRotateMat() const = 0;
    virtual GLfloat getScale() const = 0;
    virtual void show() = 0;
};
