#include "renderer/RenderPassBuilder.h"
#include "renderer/Renderer.h"
#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/RenderUtils.h"

namespace core::renderer
{
    RenderPassBuilder::RenderPassBuilder(RenderContext* render_context, uint32_t max_frames_in_flight):
                        render_context(render_context),
                        max_frames_in_flight(max_frames_in_flight),
                        command_pool(nullptr),
                        active_command_buffer(nullptr)
    {
        swapchain_manager = render_context->swapchain_manager.get();
        device_manager = render_context->device_manager.get();
    }

    VkCommandBuffer* RenderPassBuilder::get_command_buffer(uint32_t pass_id, uint32_t image_id)
    {
        //Find the command buffer with a particular id and image_id
        auto itr = std::ranges::find(pass_ids, pass_id);

        if (itr == pass_ids.end())
        {
            return nullptr;
        }

        size_t base_index = std::distance(pass_ids.begin(), itr);
        size_t index = base_index + image_id;

        if (index < command_buffers.size())
            return &command_buffers[index];

        return nullptr;
    }

    RenderPassBuilder& RenderPassBuilder::cache_active_command_buffer(uint32_t pass_id, uint32_t image_id)
    {
        active_command_buffer = get_command_buffer(pass_id, image_id);
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::init_pass(uint32_t pass_id, bool use_max_frames)
    {
        create_sync_objects();

        if (use_max_frames)
        {
            for (int i = 0; i < max_frames_in_flight; i++)
            {
                pass_ids.push_back(pass_id);
                command_buffers.push_back({});
                depth_stencil_images.push_back({});
            }
        }
        else
        {
            pass_ids.push_back(pass_id);
            command_buffers .push_back({});
        }

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::create_command_pool()
    {
        utils::RenderUtils::create_command_pool(*render_context, command_pool);
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::create_depth_stencil_image(uint32_t pass_id)
    {
        if (pass_id < depth_stencil_images.size())
        {
            utils::RenderUtils::get_supported_depth_stencil_format(render_context->device_manager->get_physical_device(), &depth_stencil_images[pass_id].format);
            utils::RenderUtils::create_depth_stencil_image(*render_context, render_context->swapchain_manager->get_swapchain().extent, render_context->device_manager->get_allocator(), depth_stencil_images[pass_id]);
        }
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::allocate_command_buffer(uint32_t pass_id, uint32_t image)
    {
        if (VkCommandBuffer* command_buffer = get_command_buffer(pass_id, image))
        {
            utils::RenderUtils::allocate_command_buffer(*render_context, command_pool, *command_buffer);
        }
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::begin_command_buffer_recording()
    {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        if (active_command_buffer &&
            (render_context->dispatch_table.beginCommandBuffer(*active_command_buffer, &begin_info) != VK_SUCCESS))
        {
            std::cout << "failed to begin recording command buffer\n";
            assert(false);
        }

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::set_present_image_transition(uint32_t pass_id, uint32_t image_id, PresentationImageType presentation_image_type)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        switch (presentation_image_type)
        {

        case PresentationImageType::SwapChain:
            {
                auto swapchain_ref = render_context->swapchain_manager->get_swapchain();
                auto image = swapchain_ref.get_images().value()[image_id];
                utils::ImageUtils::image_layout_transition(*active_command_buffer,
                                                image,
                                               VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                               VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                               0,
                                               VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                               VK_IMAGE_LAYOUT_UNDEFINED,
                                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
                break;
            }

        case PresentationImageType::DepthStencil:
            utils::ImageUtils::image_layout_transition(*active_command_buffer,
                                          depth_stencil_images[pass_id].image,                                     
                                         VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                         VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                         0,
                                         VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                          VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 });

            break;

        default:
            std::cout << "Invalid PresentationImageType\n";
        }

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::setup_color_attachment(uint32_t image, VkClearValue clear_value)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        auto swapchain_ref = render_context->swapchain_manager->get_swapchain();
        color_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };

        //TODO: Bounds check

        color_attachment_info.pNext        = VK_NULL_HANDLE;
        color_attachment_info.imageView    = swapchain_ref.get_image_views().value()[image];
        color_attachment_info.imageLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.resolveMode  = VK_RESOLVE_MODE_NONE;
        color_attachment_info.loadOp       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp      = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue   = clear_value; ;

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::setup_depth_attachment(uint32_t pass_id, VkClearValue clear_value)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        // Depth attachment
        depth_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depth_attachment_info.pNext       = VK_NULL_HANDLE;
        depth_attachment_info.imageView   = depth_stencil_images[pass_id].view;
        depth_attachment_info.imageLayout =  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
        depth_attachment_info.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment_info.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment_info.clearValue  = { {1.0f, 0} }; // depth=1, stencil=0

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::begin_rendering()
    {
#ifdef _DEBUG
        assert(active_command_buffer && swapchain_manager);
#endif

        auto swapchain_ref = swapchain_manager->get_swapchain();

        // Render area
        VkRect2D render_area = { {0, 0}, {swapchain_ref.extent.width, swapchain_ref.extent.height} };

        // Dynamic rendering info
        VkRenderingInfoKHR render_info = { VK_STRUCTURE_TYPE_RENDERING_INFO_KHR };
        render_info.renderArea          = render_area;
        render_info.layerCount          = 1;
        render_info.colorAttachmentCount = 1;
        render_info.pColorAttachments   = &color_attachment_info;
        render_info.pDepthAttachment    = &depth_attachment_info;
        render_info.pStencilAttachment  = &depth_attachment_info;

        render_context->dispatch_table.cmdBeginRenderingKHR(*active_command_buffer, &render_info);

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::end_rendering()
    {
        render_context->dispatch_table.cmdEndRenderingKHR(*active_command_buffer);
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::record_draw_batches(const std::function<void(VkCommandBuffer current_buffer, RenderContext* render_context, material::Material* material)>& func)
    {
#ifdef _DEBUG
        assert(active_command_buffer && swapchain_manager);
#endif
        func(*active_command_buffer, render_context, material_to_use);
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::end_command_buffer_recording(uint32_t image)
    {
        utils::ImageUtils::image_layout_transition
        (
             *active_command_buffer,                            // Command buffer
             render_context->swapchain_manager->get_swapchain().get_images().value()[image],    // Swapchain image
             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // Source pipeline stage
             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,     // Destination pipeline stage
             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // Source access mask
             0,                                        // Destination access mask
             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // Old layout
             VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // New layout
              VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        if (render_context->dispatch_table.endCommandBuffer(*active_command_buffer) != VK_SUCCESS)
        {
            std::cout << "failed to record command buffer\n";
        }

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::set_material(material::Material& material)
    {
        material_to_use = &material;
        return *this;
    }

    bool RenderPassBuilder::create_sync_objects()
    {
        available_semaphores.resize(max_frames_in_flight);
        finished_semaphores.resize(max_frames_in_flight);
        in_flight_fences.resize(max_frames_in_flight);
        image_in_flight.resize(swapchain_manager->get_swapchain().image_count, VK_NULL_HANDLE);

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

    bool RenderPassBuilder::draw_frame()
    {
        auto dispatch_table = render_context->dispatch_table;
        dispatch_table.waitForFences(1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

        uint32_t image_index = 0;
        VkResult result = dispatch_table.acquireNextImageKHR(swapchain_manager->get_swapchain(), UINT64_MAX, available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return swapchain_manager->recreate_swapchain();
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

        VkSemaphore wait_semaphores[] = { available_semaphores[current_frame]};
        VkPipelineStageFlags wait_stages[] =
        {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = wait_semaphores;
        submitInfo.pWaitDstStageMask = wait_stages;

        std::vector command_buffers_to_submit = { command_buffers[image_index] };
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

        VkSwapchainKHR swapChains[] = { swapchain_manager->get_swapchain().swapchain };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapChains;

        present_info.pImageIndices = &image_index;

        result = dispatch_table.queuePresentKHR(device_manager->get_present_queue(), &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            return swapchain_manager->recreate_swapchain();
        }
        if (result != VK_SUCCESS)
        {
            std::cout << "failed to present swapchain image\n";
            return false;
        }

        current_frame = (current_frame + 1) % max_frames_in_flight;
        return true;
    }

    void RenderPassBuilder::cleanup()
    {
    }
}
