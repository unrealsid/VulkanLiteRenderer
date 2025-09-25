#include "Renderer.h"

#include <iostream>
#include <ostream>
#include "platform/OS_Manager.h"
#include "structs/RenderContext.h"

namespace core
{
    void Renderer::renderer_init(RenderContext* p_render_context, platform::OS_Manager* p_os_manager)
    {
        std::cout << "Renderer Setup" << std::endl;

        load_opengl();
        os_manager = p_os_manager;
    }

    void Renderer::load_opengl()
    {
        // Load OpenGL functions using GLAD
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            std::cerr << "Failed to initialize GLAD\n";
            glfwTerminate();
            throw std::runtime_error("Cannot load GLAD");
        }
    }

    void Renderer::renderer_update(RenderContext* p_render_context)
    {
        std::cout << "Renderer Update" << std::endl;
        os_manager->os_update();
    }
}
