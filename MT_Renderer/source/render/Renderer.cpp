#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "Config.inl"
#include "renderer/RenderPassBuilder.h"

namespace core::renderer
{

    void Renderer::renderer_init()
    {
        std::cout << "Renderer Setup" << std::endl;

        init_vulkan();
        init_cleanup();

        RenderPassBuilder pass_builder(render_context.get(), 2);

        auto swapchain_ref = render_context->swapchain_manager->get_swapchain();

        //Pass 0
        uint32_t pass_id = 0;
        auto pass = pass_builder.init_pass(pass_id, true);
        pass.create_command_pool()
        .create_depth_stencil_image(pass_id);

        //Images
        for (int i = 0; i < pass.get_max_frames_in_flight(); i++)
        {
            pass.allocate_command_buffer(0, i)
            .cache_active_command_buffer(pass_id, i)
            .begin_command_buffer_recording()
            .set_present_image_transition(pass_id, i, PresentationImageType::SwapChain)
            .set_present_image_transition(pass_id, i, PresentationImageType::DepthStencil)
            .setup_color_attachment(i, { {0.1f, 0.1f, 0.1f, 1.0f} })
            .setup_depth_attachment(i, { {1.0f, 0} })
            .begin_rendering()
            .record_draw_batches([&]() -> void
            {
                //Render commands
            })
            .end_command_buffer_recording(i);
        }
    }

    void Renderer::renderer_update()
    {
        std::cout << "Renderer Update" << std::endl;
    }
}
