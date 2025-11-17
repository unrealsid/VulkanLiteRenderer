#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "Config.inl"
#include "materials/Material.h"
#include "renderer/Subpass.h"
#include "structs/vulkan/PushConstantBlock.h"
#include "structs/vulkan/Vertex.h"

namespace core::renderer
{
    void Renderer::renderer_init()
    {
        std::cout << "Renderer Setup" << std::endl;

        init_vulkan();
        init_cleanup();

        render_pass = std::make_unique<RenderPass>(render_context.get(), engine_context, 2);
        render_pass->render_pass_init();

        std::cout << "Done";
    }

    void Renderer::renderer_update() const
    {
        std::cout << "\nFrame Context can render: " << engine_context->start_rendering;
        std::cout << "Renderer Update" << std::endl;

        //TODO: Change this.
        if (engine_context->start_rendering)
        {
            while (true)
            {
                render_pass->draw_frame();
            }
        }
    }
}
