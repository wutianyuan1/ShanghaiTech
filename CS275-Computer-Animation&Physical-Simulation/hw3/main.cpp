#include "base.hpp"
#include "mesh.hpp"
#include "solver.hpp"

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


GLfloat lightDiffuse[]  = {0x66/255.0f, 0xcc/255.0f, 0xff/255.0f, 1.0};      /* Red diffuse light. */
GLfloat lightPosition[] = {-1.0, -1.0, -1.0, 0.0};     /* Infinite light location. */
int width = 600, height = 480, meshSize = 53;
enum {DEMO_CIRCLE, DEMO_CUBE};
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

void display(Solver* solver, unsigned char type)
{
    int t = 0;
    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        clothMesh->draw();
        glfwSwapBuffers(window);
        if (type == DEMO_CIRCLE)
            solver->animateCircleForce(t, 30);
        else if (type == DEMO_CUBE)
            solver->animateCubeCollision(t, 30);
        t += 1;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2 || (std::string(argv[1]) != "circle" && std::string(argv[1]) != "cube"))
    {
        std::cerr << "invalid command\n";
        exit(1);
    }
    glfwInit();
    window = glfwCreateWindow(width, height, "Fast-Mass-Spring", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glClearColor(1, 1, 1, 1);
    glfwSetKeyCallback(window, keyboardFunc);
    glewExperimental = true;
    glewInit();
    init();
    clothMesh = new Mesh(Mesh::NewGrid(meshSize, 2.5), meshSize);
    Solver* clothSolver = new Solver(clothMesh->getVertices(), meshSize, 2.5);

    if (std::string(argv[1]) == "circle")
        display(clothSolver, DEMO_CIRCLE);
    else
        display(clothSolver, DEMO_CUBE);
    glfwTerminate();

    clothMesh->cleanUp();
    delete clothMesh;
    delete clothSolver;
    return 0;
}
