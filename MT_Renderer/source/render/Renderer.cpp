#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "Config.inl"
#include "materials/Material.h"
#include "renderer/RenderPassBuilder.h"
#include "structs/vulkan/PushConstantBlock.h"
#include "structs/vulkan/Vertex.h"

namespace core::renderer
{

    void Renderer::renderer_init()
    {
        std::cout << "Renderer Setup" << std::endl;

        init_vulkan();
        init_cleanup();

        material::Material material("default", render_context.get());
        auto new_material = material.create_material("default");

        pass_builder = RenderPassBuilder(render_context.get(), 2);

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
            .record_draw_batches([&](VkCommandBuffer command_buffer, RenderContext* render_context, material::Material* material)
            {
                material->get_shader_object()->set_initial_state(render_context->dispatch_table, render_context->swapchain_manager->get_swapchain().extent, command_buffer,
                                                                 Vertex::get_binding_description(), Vertex::get_attribute_descriptions(), render_context->swapchain_manager->get_swapchain().extent);
                material->get_shader_object()->bind_material_shader(render_context->dispatch_table, command_buffer);

                //Passing Buffer Addresses
                PushConstantBlock references{};
                // Pass a pointer to the global matrix via a buffer device address
                // references.scene_buffer_address = engine_context.renderer->get_gpu_scene_buffer().scene_buffer_address;
                // references.material_params_address = engine_context.material_manager->get_material_params_address();

                render_context->dispatch_table.cmdDraw(command_buffer, 3, 1, 0, 0);
            })
            .end_rendering()
            .end_command_buffer_recording(i);
        }
    }

    void Renderer::renderer_update()
    {
        std::cout << "Renderer Update" << std::endl;
    }
}
