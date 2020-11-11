#include <GL/glew.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Open an OpenGL window
GLFWwindow* window;

/****Step 1: define vertices in (x, y, z) form****/

// Coordinates to draw a cube
const GLdouble coordinates[8][3] = {
    {-0.5, -0.5, -0.5},
    {0.5, -0.5, -0.5},
    {0.5, -0.5, 0.5},
    {-0.5, -0.5, 0.5},
    {-0.5, 0.5, 0.5},
    {-0.5, 0.5, -0.5},
    {0.5, 0.5, -0.5},
    {0.5, 0.5, 0.5}
};
/************************/


int main( void ) {
    if (!glfwInit()){
        fprintf(stderr, "Failed to initialize GLFW.\n");
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(700, 500, "Hello World", NULL, NULL);
    if (window == NULL) {
            fprintf(stderr, "glfw failed to create window.\n");
            //glfwTerminate();
            return -1;
            }
    // Make the window's context current
    glfwMakeContextCurrent(window);

    glewInit();
    if (glewInit() != GLEW_OK){
        fprintf(stderr, "Failed to initialize GLEW: %s.\n", glewGetErrorString(glewInit()));
        return -1;
    }
    // 4x anti aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);

    int cube_size = sizeof(coordinates)/sizeof(coordinates[0]);
    /**Step 2: send this cube vertices to OpenGL through a buffer**/
    GLuint vertexBuffer; // Declare vertex buffer
    glGenBuffers(1, &vertexBuffer); // generating 1 buffer, put resulting identifier in this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, cube_size*12, coordinates, GL_STATIC_DRAW);
    /************************/

    std::cout << sizeof(coordinates)/sizeof(coordinates[0]);

    /**Step 3: Main loop for OpenGL draw the shape**
    /* Main loop */
    do{
        glClearColor(1.0, 0.1, 0.1, .0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_LINE_LOOP, 0, cube_size);
        glDisableVertexAttribArray(0);

        // Swap front and back rendering buffers
        glfwSwapBuffers(window);
        //Poll for and process events
        glfwPollEvents();
    } // check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
    /***********************************************/


    // Close window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    // Exit program
    exit( EXIT_SUCCESS );
}