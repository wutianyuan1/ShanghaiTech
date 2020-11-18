#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <eigen3/Eigen/Eigen>
#include "basicTypes.hpp"
#include "cube.hpp"

Cube::Cube(GLfloat scale) :
    n({ {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
          {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} }), 
    faces({ {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
            {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} }),
    scalingFactor(scale), position(0, 0, 0)
{
    // Initial center position: (0, 0, 0)
    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -scalingFactor;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = scalingFactor;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -scalingFactor;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = scalingFactor;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = scalingFactor;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = -scalingFactor;
    memset(rotMat, 0, sizeof(rotMat));
    rotMat[0][0] = rotMat[1][1] = rotMat[2][2] = rotMat[3][3] = 1.0f;
}

Cube::~Cube() {};

void Cube::setRotateMat(GLfloat mat[4][4])
{
    memcpy(rotMat, mat, sizeof(GLfloat)*16);
}

const GLfloat* Cube::getRotateMat() const
{
    return (const GLfloat*) &rotMat[0][0];
}

void Cube::setXYZ(GLfloat x, GLfloat y, GLfloat z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

Vec3f Cube::getXYZ() const
{
    return Vec3f(position.x, position.y, position.z);
}

GLfloat Cube::getScale() const
{
    return scalingFactor;
}

std::vector<Vec3f > Cube::getVertices() const
{
    // without center position, need plus center position when using.
    Vec3f v1(rotMat[0][0] + rotMat[0][1] + rotMat[0][2], rotMat[1][0] + rotMat[1][1] + rotMat[1][2], rotMat[2][0] + rotMat[2][1] + rotMat[2][2]);
    Vec3f v2(rotMat[0][0] + rotMat[0][1] - rotMat[0][2], rotMat[1][0] + rotMat[1][1] - rotMat[1][2], rotMat[2][0] + rotMat[2][1] - rotMat[2][2]);
    Vec3f v3(rotMat[0][0] - rotMat[0][1] + rotMat[0][2], rotMat[1][0] - rotMat[1][1] + rotMat[1][2], rotMat[2][0] - rotMat[2][1] + rotMat[2][2]);
    Vec3f v4(-rotMat[0][0] + rotMat[0][1] + rotMat[0][2], -rotMat[1][0] + rotMat[1][1] + rotMat[1][2], -rotMat[2][0] + rotMat[2][1] + rotMat[2][2]);
    Vec3f v5(-rotMat[0][0] - rotMat[0][1] + rotMat[0][2], -rotMat[1][0] - rotMat[1][1] + rotMat[1][2], -rotMat[2][0] - rotMat[2][1] + rotMat[2][2]);
    Vec3f v6(rotMat[0][0] - rotMat[0][1] - rotMat[0][2], rotMat[1][0] - rotMat[1][1] - rotMat[1][2], rotMat[2][0] - rotMat[2][1] - rotMat[2][2]);
    Vec3f v7(-rotMat[0][0] + rotMat[0][1] - rotMat[0][2], -rotMat[1][0] + rotMat[1][1] - rotMat[1][2], -rotMat[2][0] + rotMat[2][1] - rotMat[2][2]);
    Vec3f v8(-rotMat[0][0] - rotMat[0][1] - rotMat[0][2], -rotMat[1][0] - rotMat[1][1] - rotMat[1][2], -rotMat[2][0] - rotMat[2][1] - rotMat[2][2]);
    return std::vector<Vec3f>({v1*scalingFactor, v2*scalingFactor, v3*scalingFactor, v4*scalingFactor, v5*scalingFactor, v6*scalingFactor, v7*scalingFactor, v8*scalingFactor});
}

std::vector<Vec3f > Cube::getNorms() const
{
    // without center position, need plus center position when using.
    return  std::vector<Vec3f>({ 
        Vec3f(rotMat[0][0], rotMat[1][0], rotMat[2][0])*scalingFactor, Vec3f(-rotMat[0][0], -rotMat[1][0], -rotMat[2][0])*scalingFactor, \
        Vec3f(rotMat[0][1], rotMat[1][1], rotMat[2][1])*scalingFactor, Vec3f(-rotMat[0][1], -rotMat[1][1], -rotMat[2][1])*scalingFactor, \
        Vec3f(rotMat[0][2], rotMat[1][2], rotMat[2][2])*scalingFactor, Vec3f(-rotMat[0][2], -rotMat[1][2], -rotMat[2][2])*scalingFactor
        });
}

bool Cube::collide(const Cube* other, Vec3f & point, Vec3f & normal)
{
    std::vector<Vec3f> otherVerts = other->getVertices();
    std::vector<Vec3f> selfNorms  = this->getNorms();
    Vec3f otherPos = other->getXYZ();
    for (int i = 0; i < 6; i++)
    {
        bool innerFlag = false;
        std::vector<Vec3f> points;
        for (int j = 0; j < 8; j++)
        {
            Vec3f prod = (otherVerts[j] - selfNorms[i]  + otherPos - position) * selfNorms[i];
            if (prod.x + prod.y + prod.z < 0)
                innerFlag = true;
            if (fabs(prod.x + prod.y + prod.z) <= 0.004)
                points.push_back(otherVerts[j] + otherPos);
        }
        if (points.size() != 0)
        {
            Vec3f mid;
            for (size_t i = 0; i < points.size(); i++)
                mid = mid + points[i];
            mid = mid / points.size();
            point = mid;
            normal = selfNorms[i];
        }
        if (innerFlag == false)
            return false;
    }
    return true;
}

void Cube::show()
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glMultMatrixf(&rotMat[0][0]);
    for (int i = 0; i < 6; i++)
    {
        glBegin(GL_QUADS);
        glNormal3fv(&n[i][0]);
        glVertex3fv(&v[faces[i][0]][0]);
        glVertex3fv(&v[faces[i][1]][0]);
        glVertex3fv(&v[faces[i][2]][0]);
        glVertex3fv(&v[faces[i][3]][0]);
        glEnd();
    }   
    glPopMatrix();
}