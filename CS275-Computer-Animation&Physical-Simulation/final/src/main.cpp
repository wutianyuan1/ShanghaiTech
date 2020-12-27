#include "base.hpp"
#include "cube.hpp"
#include "sph.hpp"
#include "render/window.h"
#include "render/shader.h"
#include "render/chunk.h"
#include "render/light.h"
#include "render/camera.h"

#include <glm/gtc/matrix_transform.hpp>
double xpos, ypos;
bool pressed = false;
float deltaTime = 0.0f;
Camera camera;

// namespace config
// {
//     float INIT_DENSITY    = 700.0;
//     float DAMPING         = -0.5;
//     float VISCOSITY       = 5.0;
//     float GAS_CONST       = 1.0;
//     float SURFACE_NORM    = 6.0;
//     float SURFACE_COEF    = 0.1;
//     float TIME_STEP       = 0.01; 
//     vec3f GRAVITY         = vec3f(0, -9.8, 0);
// }

namespace config
{
    float INIT_DENSITY    = 900.0;
    float DAMPING         = -0.5;
    float VISCOSITY       = 5.0;
    float GAS_CONST       = 1.0;
    float SURFACE_NORM    = 6.0;
    float SURFACE_COEF    = 0.1;
    float TIME_STEP       = 0.02; 
    vec3f GRAVITY         = vec3f(0, -1.6, 0);
}

void keyBoardFunc(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
		return;
	camera.update_kbd(deltaTime, key);
}

void mouseButtonFunc(GLFWwindow* window, int button, int action, int mods)
{
	pressed = ((action == GLFW_PRESS || action == GLFW_REPEAT) && button == GLFW_MOUSE_BUTTON_LEFT);
        
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	xpos = float(0 - (x - 1080 / 2) / 1080) * 2;
	ypos = float(0 - (y - 800 / 2) / 800) * 2;
    if (pressed)
        camera.update_mouse(xpos, ypos);
    else
        camera.lastMouseX = camera.lastMouseY = -1;
}

int main()
{
    if (Window::initWindow("OpenGL Marching Cubes Demo", 1080, 800) != 0)
    {
        std::cout << "Failed to initialize GLFW Window" << std::endl;
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

    glEnable(GL_DEPTH_TEST);
    Light l(glm::vec3(0.2f, -1.0f, 1.2f), { glm::vec3(0.25f), glm::vec3(1.0f), glm::vec3(1.0f) });
    Light::lights.push_back(&l);

    Light flashLight = Light(glm::vec3(0.0f), glm::vec3(0.0f),
    	{ glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(0.5f) },
    	{ 1.0f, 0.09f, 0.032f }, 12.5f, 20.0f);

    Light::lights.push_back(&flashLight);

    Cube cube(MODE_CUBE);
    ParticleSystem system(vec3f(1), 0.05);

    Shader shader({ "defaultVertexShader.vs", "defaultFragmentShader.fs" }, { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER });
    shader.use();
    shader.setFloat("specular", 0.2f);
    shader.setFloat("shininess", 16.0f);

    glfwSetKeyCallback(Window::window, keyBoardFunc);
    glfwSetMouseButtonCallback(Window::window, mouseButtonFunc);
	glfwSetCursorPosCallback(Window::window, cursor_position_callback);

    float lastTime = 0.0f;
    int i = 0;
    while (!glfwWindowShouldClose(Window::window))
    {
        float now = glfwGetTime();
        deltaTime = now - lastTime;
        printf("FPS = %f\n", 1.0 / deltaTime);
        lastTime = now;

        glClearColor(0.0f, 0.3f, 0.4f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update_tail();
        shader.setVec3("cameraPos", camera.position);

        flashLight.setPosition(camera.position);
        flashLight.setDirection(camera.front);
        Light::update(&shader);

        glm::mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)Window::width / (float)Window::height, 0.1f, 300.0f);
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(28.0f, -60.0f, -60.0f));
        shader.setMat4("model", model);
        shader.setMat4("pvm", projection * camera.viewMatrix * model);
        if (i++ % 300 == 0)
        {
            for (float i = 16 * 1 / 4; i < 16 * 3 / 4; i += 0.35)
                for (float j = 16 * 1 / 4; j < 16 * 3 / 4; j += 0.35)
                    for (float k =16 * 1 / 4; k < 16 * 3 / 4; k += 0.35)
                        if ((i - 16/2) * (i - 16 / 2) + (j - 16 /2) * (j - 16/2) + \
                            (k - 16/2) * (k - 16/2) < (16 * 16 / 16) )
                             system.NewParticle(vec3f(i*0.05, j*0.05, k*0.05), vec3f(0.0f));
        }
        // cube.render();
        system.Update();
        system.Render();
        

        glfwSwapBuffers(Window::window);
        glfwPollEvents();
        
    }

    glfwTerminate();
}
