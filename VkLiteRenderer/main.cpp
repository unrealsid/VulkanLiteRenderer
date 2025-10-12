#include <iostream>

#include "renderer/Renderer.h"
#include "renderer/RenderPassBuilder.h"

int main()
{
    core::renderer::Renderer renderer;
    renderer.renderer_init();

    auto window = renderer.get_render_context()->window_manager->get_window();

    while (!glfwWindowShouldClose(window))
    {
        // Input handling
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        renderer.pass_builder->draw_frame();

        glfwPollEvents();
    }

    return 0;
}
