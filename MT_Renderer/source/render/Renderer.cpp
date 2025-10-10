#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "Config.inl"

namespace core::renderer
{

    void Renderer::renderer_init()
    {
        std::cout << "Renderer Setup" << std::endl;

        init_vulkan();
        init_cleanup();
    }

    void Renderer::renderer_update()
    {
        std::cout << "Renderer Update" << std::endl;
    }
}
