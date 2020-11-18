#include <GL/glew.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <GLUT/glut.h>
#include <vector>
#include <iostream>
#include <cmath>

#include "cube.hpp"
#include "basicTypes.hpp"
#include "rigidBody.hpp"

GLfloat lightDiffuse[] = {0x66/255.0f, 0xcc/255.0f, 0xff/255.0f, 1.0};      /* Red diffuse light. */
GLfloat lightPosition[] = {1.0, 1.0, 1.0, 0.0};     /* Infinite light location. */
std::vector<RigidBody<Cube> > cubes;
int width = 600, height = 480;
GLFWwindow* window;

static void keyboardFunc(GLFWwindow* window, int key, int scancode, int action,
                         int mods) 
{
    (void)window;
    (void)scancode;
    (void)mods;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) 
    {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void init(void)
{
    /* Enable a single OpenGL light. */
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    /* Use depth buffering for hidden surface elimination. */
    glEnable(GL_DEPTH_TEST);

    /* Setup the view of the cube. */
    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 40.0,
        /* aspect ratio */ 1.0,
        /* Z near */ 1.0, /* Z far */ 10.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 5.0,            /* eye is at (0,0,5) */
            0.0, 0.0, 0.0,              /* center is at (0,0,0) */
            0.0, 1.0, 0.0);             /* up is in positive Y direction */

    /* Adjust cube position to be asthetic angle. */
    glTranslatef(0.0, 0.0, -1.0);
    glRotatef(90, 0, 1, 0);
}


void display(bool stopWhenCollision=false, bool haveAngularVel=false)
{
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Vec3f pt, norm;
    for (auto& cube : cubes)
    {
        cube.show();   
        if (!cubes[1].collide(cubes[0], pt, norm) )   
            cube.update(); 
        else if (!stopWhenCollision)
        {
            cubes[1].handleCollision(cubes[0], pt, norm);
            cubes[0].update(); cubes[1].update();
            if (haveAngularVel)
            {
                cubes[0].setVelocity(cubes[0].getVelocity() + Vec3f(0, 0, -0.2));
                cubes[0].setAngularVelocity(cubes[0].getAngularVelocity() + Vec3f(-0.9, 0, 0));
                cubes[1].setVelocity(cubes[1].getVelocity() + Vec3f(0, 0, 0.2));
                cubes[1].setAngularVelocity(cubes[1].getAngularVelocity() + Vec3f(0.5, 0, 0));
            }
            break;
        }
    }
    glfwSwapBuffers(window);      
}

void test0()
{
    Cube* cb1 = new Cube(0.2);
    cb1->setXYZ(0, 1.5, 0);
    Cube* cb2 = new Cube(0.2);
    cb2->setXYZ(0, -1.5, 0);
    Cube* cb3 = new Cube(0.2);
    cb3->setXYZ(0, 0, 0);
    Cube* cb4 = new Cube(0.1);
    cb4->setXYZ(0, 0.7, 0.5);
    Cube* cb5 = new Cube(0.1);
    cb5->setXYZ(0, -0.7, -0.5);
    cubes.push_back(RigidBody<Cube>(cb1));
    cubes.push_back(RigidBody<Cube>(cb2));
    cubes.push_back(RigidBody<Cube>(cb3));
    cubes.push_back(RigidBody<Cube>(cb4));
    cubes.push_back(RigidBody<Cube>(cb5));
    cubes[0].setVelocity(Vec3f(0, 0, 1));
    cubes[1].setForce(Vec3f(0, 0, 1));
    cubes[2].setAngularVelocity(Vec3f(0, 0, 1));
    cubes[3].setTorque(Vec3f(0, 0, 0.01));
    cubes[4].setTorque(Vec3f(0, 0, 0.005));
    cubes[4].setVelocity(Vec3f(0, 0.5, 0));
    while (glfwWindowShouldClose(window) == GL_FALSE)
        display();  
}

void test1()
{
    Cube* cb1 = new Cube(0.2);
    cb1->setXYZ(0, 1.5, 0);
    Cube* cb2 = new Cube(0.2);
    cb2->setXYZ(0, -1.5, 0);
    cubes.push_back(RigidBody<Cube>(cb1));
    cubes.push_back(RigidBody<Cube>(cb2));
    Vec3f vel(0, 0, 0.1);
    cubes[0].setVelocity(vel);
    cubes[0].setTorque(Vec3f(0.06, 0, 0));
    Vec3f vel1(0, 0.5, 0);
    cubes[1].setVelocity(vel1);
    cubes[1].setForce(Vec3f(0, 0.1, 0));
    while (glfwWindowShouldClose(window) == GL_FALSE)
        display(true);    
}

void _test_collision_response(Vec3f torque, bool haveAngularVel=false)
{
    Cube* cb1 = new Cube(0.3);
    cb1->setXYZ(0, 1.5, 0);
    Cube* cb2 = new Cube(0.2);
    cb2->setXYZ(0, -1.5, 0);
    cubes.push_back(RigidBody<Cube>(cb1));
    cubes.push_back(RigidBody<Cube>(cb2));
    Vec3f vel(0, -0.3, 0);
    cubes[0].setVelocity(vel);
    cubes[0].setTorque(torque);
    Vec3f vel1(0, 0.5, 0);
    cubes[1].setVelocity(vel1);
    while (glfwWindowShouldClose(window) == GL_FALSE)
        display(false, haveAngularVel);
}

void test2()
{
    _test_collision_response(Vec3f(0, 0, 0));
}

void test3()
{
    _test_collision_response(Vec3f(0.01, 0, 0), true);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " [num]" << std::endl;
        return -2;
    }
    if (!glfwInit()) 
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }
    window = glfwCreateWindow(width, height, "Cubes", NULL, NULL);
    if (window == NULL) 
    {
        std::cerr << "Failed to open GLFW window. " << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glClearColor(1, 1, 1, 1);

    glfwSetKeyCallback(window, keyboardFunc);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) 
    {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        return -1;
    }

    init();
    switch(std::atoi(argv[1]))
    {
        case 0: test0(); break;
        case 1: test1(); break;
        case 2: test2(); break;
        case 3: test3(); break;
        default: break;
    }
    glfwTerminate();
    return 0;
}
