#include "render/Renderer.h"

#include <iostream>
#include <ostream>

#include "Config.inl"
#include "platform/WindowManager.h"
#include "structs/engine/RenderContext.h"
#include "structs/engine/FrameContext.h"
#include "vulkanapp/VulkanCleanupQueue.h"

namespace core::renderer
{
    Renderer::Renderer() : render_context(nullptr), frame_context(nullptr)
    {
    }

    void Renderer::renderer_init(FrameContext* p_frame_context)
    {
        std::cout << "Renderer Setup" << std::endl;

        frame_context = p_frame_context;

        init_vulkan();
        init_cleanup();

        cleanup();
    }

    void Renderer::renderer_update()
    {
        std::cout << "Renderer Update" << std::endl;

        while (true)
        {
            RenderCommand command;

            while ( bool next_element_available = frame_context->render_command_buffer.try_dequeue(command))
            {

            }
        }
    }
}
