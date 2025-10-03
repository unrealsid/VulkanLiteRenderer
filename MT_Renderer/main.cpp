#include <iostream>
#include <GLFW/glfw3.h>
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

void setup_triangle()
{
    std::cout << "Setup Triangle" << std::endl;
}

void application_update()
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