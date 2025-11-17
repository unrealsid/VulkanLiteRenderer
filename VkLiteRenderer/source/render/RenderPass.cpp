
#include "renderer/RenderPass.h"

#include <iostream>
#include <thread>
#include "structs/vulkan/PushConstantBlock.h"
#include "structs/vulkan/Vertex.h"
#include "structs/engine/EngineContext.h"


namespace core::renderer
{
    RenderPass::RenderPass(RenderContext* render_context, std::weak_ptr<EngineContext> engine_context, uint32_t max_frames_in_flight) :
        render_context(render_context),
        max_frames_in_flight(max_frames_in_flight),
        engine_context(std::move(engine_context))
    {
        swapchain_manager = render_context->swapchain_manager.get();
        device_manager = render_context->device_manager.get();
    }

    void RenderPass::initialize_geometry_pass()
    {
        std::unique_ptr<Subpass> geometry_pass = std::make_unique<Subpass>(render_context, max_frames_in_flight);
        subpasses.push_back(std::move(geometry_pass));

        material::Material material("default", render_context);
        auto new_material = material.create_material("default");

        //Pass 0
        Subpass& pass = subpasses[0]->init_pass(true);
        pass.create_command_pool()
        .create_depth_stencil_image();

        //Images
        for (int i = 0; i < max_frames_in_flight; i++)
        {
            pass.allocate_command_buffer(i)
                .cache_active_command_buffer(i)
                .begin_command_buffer_recording()
                .set_present_image_transition(i, PresentationImageType::SwapChain)
                .set_present_image_transition(i, PresentationImageType::DepthStencil)
                .setup_color_attachment(i, { {0.1f, 0.1f, 0.1f, 1.0f} })
                .setup_depth_attachment({ {1.0f, 0} })
                .set_material(new_material)
                .begin_rendering()
                .record_draw_batches([&](VkCommandBuffer command_buffer, RenderContext* render_context, material::Material* material)
                {
                    auto extents = render_context->swapchain_manager->get_extent();
                    material->get_shader_object()->set_initial_state(render_context->dispatch_table, extents, command_buffer,
                                                                     Vertex::get_binding_description(), Vertex::get_attribute_descriptions(), extents, {0, 0});
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

    void RenderPass::render_pass_init()
    {
        create_sync_objects();

        initialize_geometry_pass();
    }

    void RenderPass::reset_subpass_command_buffers()
    {
        for (const auto& subpass : subpasses)
        {
            subpass->reset_command_pool();
        }

        subpasses.clear();
    }

    bool RenderPass::draw_frame()
    {
        auto dispatch_table = render_context->dispatch_table;
        dispatch_table.waitForFences(1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

        uint32_t image_index = 0;
        VkResult result = dispatch_table.acquireNextImageKHR(swapchain_manager->get_swapchain(), UINT64_MAX, available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            dispatch_table.deviceWaitIdle();
            reset_subpass_command_buffers();

            auto window_manager = engine_context.lock()->window_manager;

            auto window_width = window_manager->get_window_width();
            auto window_height = window_manager->get_window_height();
            swapchain_manager->recreate_swapchain(window_width, window_height);

            initialize_geometry_pass();

            return true;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            std::cout << "failed to acquire swapchain image. Error " << result << "\n";
            return false;
        }

        if (image_in_flight[image_index] != VK_NULL_HANDLE)
        {
            dispatch_table.waitForFences(1, &image_in_flight[image_index], VK_TRUE, UINT64_MAX);
        }

        image_in_flight[image_index] = in_flight_fences[current_frame];

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = { available_semaphores[current_frame] };
        VkPipelineStageFlags wait_stages[] =
        {
           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = wait_semaphores;
        submitInfo.pWaitDstStageMask = wait_stages;

        std::vector command_buffers_to_submit = { *subpasses[0]->get_command_buffer(image_index) };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = command_buffers_to_submit.data();

        VkSemaphore signal_semaphores[] = { finished_semaphores[current_frame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signal_semaphores;

        dispatch_table.resetFences(1, &in_flight_fences[current_frame]);
        if (dispatch_table.queueSubmit(device_manager->get_graphics_queue(), 1, &submitInfo, in_flight_fences[current_frame]) != VK_SUCCESS)
        {
            //std::cout << "failed to submit draw command buffer\n";
            return false;
        }

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapChains[] = { swapchain_manager->get_swapchain() };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapChains;

        present_info.pImageIndices = &image_index;

        result = dispatch_table.queuePresentKHR(device_manager->get_present_queue(), &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            dispatch_table.deviceWaitIdle();
            reset_subpass_command_buffers();

            auto window_manager = engine_context.lock()->window_manager;

            auto window_width = window_manager->get_window_width();
            auto window_height = window_manager->get_window_height();
            swapchain_manager->recreate_swapchain(window_width, window_height);

            initialize_geometry_pass();

            return true;
        }

        if (result != VK_SUCCESS)
        {
            std::cout << "failed to present swapchain image\n";
            return false;
        }

        current_frame = (current_frame + 1) % max_frames_in_flight;
        return true;
    }

   bool RenderPass::create_sync_objects()
   {
       available_semaphores.resize(max_frames_in_flight);
       finished_semaphores.resize(max_frames_in_flight);
       in_flight_fences.resize(max_frames_in_flight);
       image_in_flight.resize(swapchain_manager->get_image_count(), VK_NULL_HANDLE);

       VkSemaphoreCreateInfo semaphore_info = {};
       semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

       VkFenceCreateInfo fence_info = {};
       fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
       fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

       auto dispatch_table = render_context->dispatch_table;

       for (size_t i = 0; i < max_frames_in_flight; i++)
       {
           if (dispatch_table.createSemaphore(&semaphore_info, nullptr, &available_semaphores[i]) != VK_SUCCESS ||
               dispatch_table.createSemaphore(&semaphore_info, nullptr, &finished_semaphores[i]) != VK_SUCCESS ||
               dispatch_table.createFence(&fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS)
           {
               std::cout << "failed to create sync objects\n";
               return false;
           }
       }

       return true;
   }
}
