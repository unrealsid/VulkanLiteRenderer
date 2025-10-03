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

int main()
{
    core::Engine engine;
    engine.init();
    core::Engine::update();
    core::Engine::shutdown();
    return 0;
}