#include "base.hpp"
#include "mesh.hpp"


#include "mesh.hpp"

/**
 * My coodinrates:
            x
            ^
            |
            | 
            . --------> y
          /
       |/_   
      z
 */


GLfloat lightDiffuse[] = {0x66/255.0f, 0xcc/255.0f, 0xff/255.0f, 1.0};      /* Red diffuse light. */
GLfloat lightPosition[] = {1.0, 1.0, 1.0, 0.0};     /* Infinite light location. */
int width = 600, height = 480, meshSize = 30;
GLFWwindow* window;
Mesh* clothMesh;

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
    gluLookAt(5.0, 0.0, 0.0,            /* eye is at (0,0,10) */
            0.0, 0.0, 0.0,              /* center is at (0,0,0) */
            0.0, 1.0, 0.0);             /* up is in positive Y direction */
    glRotatef(60, 0, 1, 0);             /* Rotate along y axis, 60-90 is good */
    glRotatef(-90, 0, 0, 1);            /* Rotate along z axis */
    glTranslatef(-0.75, -0.65, 0);
}

void testGrid(glm::vec3* &vs, GLfloat t)
{
    for (int i = 0; i < meshSize; ++i)
        for (int j = 0; j < meshSize; ++j)
            vs[i * meshSize + j].z = (float)2*t*sqrt((float)i/meshSize);
}


void display()
{
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // show and update
    glfwSwapBuffers(window);      
}


int main(int, char**)
{
    glfwInit();
    window = glfwCreateWindow(width, height, "Fast-Mass-Spring", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glClearColor(1, 1, 1, 1);
    glfwSetKeyCallback(window, keyboardFunc);
    glewExperimental = true;
    glewInit();
    init();
    clothMesh = new Mesh(Mesh::NewGrid(meshSize, 1.5), meshSize);
    auto vts = clothMesh->getVertices();
    float t = 0;
    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        testGrid(vts, sin(t));
        clothMesh->draw();
        glfwSwapBuffers(window);
        t += 0.02;
    }

    glfwTerminate();
    clothMesh->cleanUp();
    return 0;
}
