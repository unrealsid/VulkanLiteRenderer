#include "renderer/RenderPassBuilder.h"
#include "renderer/Renderer.h"
#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/RenderUtils.h"

namespace core::renderer
{
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
        if (depth_stencil_images.size() <= pass_id)
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
            assert(false);
        }

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::set_image_layout_transitions_for_active_command_buffer(uint32_t pass_id, uint32_t image)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        utils::ImageUtils::image_layout_transition(*active_command_buffer,
                                       render_context->swapchain_manager->get_swapchain().get_images().value()[image],
                                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                       0,
                                       VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        utils::ImageUtils::image_layout_transition(*active_command_buffer,
                                     depth_stencil_images[pass_id].image,
                                     VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                     VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                     0,
                                     VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                      VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 });

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::setup_attachments(uint32_t pass_id, uint32_t image)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        auto swapchain = render_context->swapchain_manager->get_swapchain();

        // Color attachment
        VkRenderingAttachmentInfoKHR color_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        color_attachment_info.pNext        = VK_NULL_HANDLE;
        color_attachment_info.imageView    = swapchain.get_image_views().value()[image];
        color_attachment_info.imageLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.resolveMode  = VK_RESOLVE_MODE_NONE;
        color_attachment_info.loadOp       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp      = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue   = { {0.1f, 0.1f, 0.1f, 1.0f} };

        // Depth attachment
        VkRenderingAttachmentInfoKHR depth_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depth_attachment_info.pNext       = VK_NULL_HANDLE;
        depth_attachment_info.imageView   = depth_stencil_images[pass_id].view;
        depth_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
        depth_attachment_info.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment_info.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment_info.clearValue  = { {1.0f, 0} }; // depth=1, stencil=0

        // Render area
        VkRect2D render_area = { {0, 0}, {swapchain.extent.width, swapchain.extent.height} };

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

    RenderPassBuilder& RenderPassBuilder::record_draw_batches(const std::function<void()>& func)
    {
        func();
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::end_command_buffer_recording(uint32_t image)
    {
        render_context->dispatch_table.cmdEndRenderingKHR(*active_command_buffer);

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

    void RenderPassBuilder::draw_frame()
    {
        // for (const auto& [material_id, draw_batch] : draw_batches)
        // {
        // //Pipeline object binding
        // draw_batch.material->get_shader_object()->set_initial_state(render_context->dispatch_table, render_context->swapchain_manager->get_swapchain().extent,*active_command_buffer,
        //                                                             Vertex::get_binding_description(), Vertex::get_attribute_descriptions(), render_context->swapchain_manager->get_swapchain().extent);
        // draw_batch.material->get_shader_object()->bind_material_shader(render_context->dispatch_table, command_buffers[i]);
        //
        // render_context->dispatch_table.cmdBindDescriptorSets(*active_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, draw_batch.material->get_pipeline_layout(), 0, 1, &draw_batch.material->get_descriptor_set(), 0, nullptr);

        //Passing Buffer Addresses
        // PushConstantBlock references{};
        // // Pass a pointer to the global matrix via a buffer device address
        // references.scene_buffer_address = engine_context.renderer->get_gpu_scene_buffer().scene_buffer_address;
        // references.material_params_address = engine_context.material_manager->get_material_params_address();

        //Binds and draws meshes
        // for (auto draw_item : draw_batch.items)
        // {
        //     references.object_model_transform_addr = draw_item.entity->get_transform_buffer_sub_address();
        //
        //     dispatch_table.cmdPushConstants(command_buffers[i], draw_batch.material->get_pipeline_layout(),
        //         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantBlock), &references);
        //
        //     VkBuffer vertex_buffers[] = {draw_item.vertex_buffer};
        //     VkDeviceSize offsets[] = {0};
        //     dispatch_table.cmdBindVertexBuffers(command_buffers[i], 0, 1, vertex_buffers, offsets);
        //     dispatch_table.cmdBindIndexBuffer(command_buffers[i], draw_item.index_buffer, 0, VK_INDEX_TYPE_UINT32);
        //
        //     // Issue the draw call using the index buffer
        //     dispatch_table.cmdDrawIndexed(command_buffers[i], draw_item.index_count, 1, draw_item.index_range.first, 0,0);
        // }
        //}
        //}
    }

    void RenderPassBuilder::cleanup()
    {
    }
}
