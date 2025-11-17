#include "renderer/Subpass.h"
#include "renderer/Renderer.h"
#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/RenderUtils.h"

namespace core::renderer
{
    Subpass::Subpass(RenderContext* render_context, uint32_t max_frames_in_flight):
                        render_context(render_context),
                        max_frames_in_flight(max_frames_in_flight),
                        command_pool(nullptr),
                        active_command_buffer(nullptr)
    {
        swapchain_manager = render_context->swapchain_manager.get();
        device_manager = render_context->device_manager.get();
    }

    VkCommandBuffer* Subpass::get_command_buffer(uint32_t image_id)
    {
        if (image_id < command_buffers.size())
            return &command_buffers[image_id];

        return nullptr;
    }

    Subpass& Subpass::cache_active_command_buffer(uint32_t image_id)
    {
        active_command_buffer = get_command_buffer(image_id);
        return *this;
    }

    void Subpass::reset_command_pool()
    {
        if (command_pool != VK_NULL_HANDLE)
        {
            render_context->dispatch_table.destroyCommandPool(command_pool, 0);
            command_buffers.clear();
        }

        render_context->dispatch_table.destroyImage(depth_stencil_image.image, nullptr);
        depth_stencil_image = {};
    }


    Subpass& Subpass::init_pass(bool use_max_frames)
    {
        if (use_max_frames)
        {
            for (int i = 0; i < max_frames_in_flight; i++)
            {
                command_buffers.push_back({});
                depth_stencil_image = {};
            }
        }
        else
        {
            command_buffers.push_back({});
        }

        return *this;
    }

    Subpass& Subpass::create_command_pool()
    {
        utils::RenderUtils::create_command_pool(*render_context, command_pool);
        return *this;
    }

    Subpass& Subpass::create_depth_stencil_image()
    {
        utils::RenderUtils::get_supported_depth_stencil_format(render_context->device_manager->get_physical_device(), &depth_stencil_image.format);
        utils::RenderUtils::create_depth_stencil_image(*render_context,
                                                       render_context->swapchain_manager->get_extent(),
                                                       render_context->device_manager->get_allocator(),
                                                       depth_stencil_image);

        return *this;
    }

    Subpass& Subpass::allocate_command_buffer(uint32_t image)
    {
        if (VkCommandBuffer* command_buffer = get_command_buffer(image))
        {
            utils::RenderUtils::allocate_command_buffer(*render_context, command_pool, *command_buffer);
        }
        return *this;
    }

    Subpass& Subpass::begin_command_buffer_recording()
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

    Subpass& Subpass::set_present_image_transition(uint32_t image_id, PresentationImageType presentation_image_type)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        switch (presentation_image_type)
        {

        case PresentationImageType::SwapChain:
            {
            auto swapchain_ref = render_context->swapchain_manager.get();
                auto image = swapchain_ref->get_images()[image_id];

                utils::ImageUtils::image_layout_transition(*active_command_buffer,
                                                image.image,
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
                                          depth_stencil_image.image,
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

    Subpass& Subpass::setup_color_attachment(uint32_t image, VkClearValue clear_value)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        auto swapchain_ref = render_context->swapchain_manager.get();
        color_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };

        //TODO: Bounds check

        color_attachment_info.pNext        = VK_NULL_HANDLE;
        color_attachment_info.imageView    = swapchain_ref->get_images()[image].image_view;
        color_attachment_info.imageLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.resolveMode  = VK_RESOLVE_MODE_NONE;
        color_attachment_info.loadOp       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp      = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue   = clear_value;

        return *this;
    }

    Subpass& Subpass::setup_depth_attachment(VkClearValue clear_value)
    {
#ifdef _DEBUG
        assert(active_command_buffer);
#endif

        // Depth attachment
        depth_attachment_info = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depth_attachment_info.pNext       = VK_NULL_HANDLE;
        depth_attachment_info.imageView   = depth_stencil_image.view;
        depth_attachment_info.imageLayout =  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
        depth_attachment_info.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment_info.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment_info.clearValue  = { {1.0f, 0} }; // depth=1, stencil=0

        return *this;
    }

    Subpass& Subpass::begin_rendering()
    {
#ifdef _DEBUG
        assert(active_command_buffer && swapchain_manager);
#endif

        // Render area
        VkRect2D render_area = { {0, 0}, {swapchain_manager->get_extent().width, swapchain_manager->get_extent().height} };

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

    Subpass& Subpass::end_rendering()
    {
        render_context->dispatch_table.cmdEndRenderingKHR(*active_command_buffer);
        return *this;
    }

    Subpass& Subpass::record_draw_batches(const std::function<void(VkCommandBuffer current_buffer, RenderContext* render_context, material::Material* material)>& func)
    {
#ifdef _DEBUG
        assert(active_command_buffer && swapchain_manager);
#endif
        func(*active_command_buffer, render_context, material_to_use);
        return *this;
    }

    Subpass& Subpass::end_command_buffer_recording(uint32_t image)
    {
        utils::ImageUtils::image_layout_transition
        (
             *active_command_buffer,                            // Command buffer
             render_context->swapchain_manager->get_images()[image].image,    // Swapchain image
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

    Subpass& Subpass::set_material(material::Material& material)
    {
        material_to_use = &material;
        return *this;
    }

    void Subpass::cleanup()
    {
    }
}
