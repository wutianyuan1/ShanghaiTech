#include <iostream>
#include <vector>
#include <GL/gl.h>

#if defined(__APPLE__)
#include <OpenGL/glut.h>
#else
#include <GL/glut.h>
#endif

#include "objects.hpp"

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

static void displayFunc(void);
static void renderPixels(std::vector<Position> points);

std::vector<BaseObject*> objs; // All objects
int t = 0;

int main(int argc, char* argv[])
{
    // Setup the OpenGL context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("CS100 Homework-X");
	glutDisplayFunc(displayFunc);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glShadeModel(GL_FLAT);
	glFlush();

    // Add your objects here
    objs.push_back(new Circle(100, 100, 200));
    objs.push_back(new Circle(100, 400, 200));
    objs.push_back(new Rectangle(300, 150, 175, 200));

    // Start the main render loop
	glutMainLoop();
	return 0;
}


// Render funtions
// ===========================================
void displayFunc(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
    for (auto& obj : objs)
    {
        renderPixels(obj->getPixelPositions());
        fallDown(obj, t);
    }
    t++;
    glFlush();
    glutPostRedisplay();
}

void renderPixels(std::vector<Position> points)
{
    glColor3f(102.0/255.0, 204.0/255.0, 1.0);
    glBegin(GL_POINTS);
    for (auto& pos : points)
        glVertex2f(pos.x, pos.y);
    glEnd();
}