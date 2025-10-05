#include <iostream>
#include <GLFW/glfw3.h>

#include "core/Application.h"
#include "include/core/Engine.h"

// Resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

}

// ESC key to close window
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void setup_triangle(core::Application* application)
{
    std::cout << "Setup Triangle" << std::endl;

    //Setup clear state
    ClearState clear_state{glm::vec4(1.0f, 0.5f, .5f, 1.0f), 1.0f, 0};
    application->clear_state_resource = application->renderer_create_clear_state(clear_state);
}

void application_update(core::Application* application)
{
    std::cout << "Application Update" << std::endl;
}

int main()
{
    core::Engine engine(setup_triangle, application_update);
    engine.init();
    engine.update();
    engine.shutdown();

    return 0;
}