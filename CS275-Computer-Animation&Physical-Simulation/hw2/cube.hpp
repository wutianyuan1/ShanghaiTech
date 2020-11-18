#pragma once
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <GLUT/glut.h>
#include <vector>

#include "basicTypes.hpp"

class Cube : public BasicObject
{
public:
    Cube(GLfloat scale=1);
    Cube(void);
    ~Cube();

    void setXYZ(GLfloat x, GLfloat y, GLfloat z);
    void setRotateMat(GLfloat mat[4][4]);

    Vec3f getXYZ() const;
    const GLfloat* getRotateMat() const;
    std::vector<Vec3f> getVertices() const;
    std::vector<Vec3f> getNorms() const;

    bool collide(const Cube* other, Vec3f & point, Vec3f & normal);

    GLfloat getScale() const;
    void show();

private:
    std::vector<std::vector<GLfloat> > n;           /* Normals for the 6 faces of a cube. */
    std::vector<std::vector<GLint> >   faces;       /* Vertex indices for the 6 faces of a cube. */
    GLfloat v[8][3];                                /* Will be filled in with X,Y,Z vertexes. */
    GLfloat scalingFactor;                          /* Size of the cube */
    Vec3<float> position;                           /* Position (x,y,z) of the cube */
    GLfloat rotMat[4][4];
};
