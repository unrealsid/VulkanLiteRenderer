
#include "Config.inl"
#include "renderer/Renderer.h"
#include "structs/engine/RenderContext.h"
#include "vulkanapp/VulkanCleanupQueue.h"
#include "vulkanapp/utils/MemoryUtils.h"

namespace core::renderer
{
    void Renderer::init_vulkan()
    {
        render_context = std::make_unique<RenderContext>();

        create_window();
        create_device();
        create_swapchain();
        utils::MemoryUtils::create_vma_allocator(*render_context->device_manager);
    }

    void Renderer::create_window()
    {
        render_context->window_manager = std::make_unique<platform::WindowManager>();
        render_context->window_manager->create_window_glfw(
     {
            window_width, window_height, window_title
        });
    }

    void Renderer::create_swapchain()
    {
        render_context->swapchain_manager = std::make_unique<vulkanapp::SwapchainManager>(*render_context);
        render_context->swapchain_manager->create_swapchain();

        max_frames_in_flight = render_context->swapchain_manager->get_swapchain().image_count;
    }

    void Renderer::create_device() const
    {
        render_context->device_manager = std::make_unique<vulkanapp::DeviceManager>(*render_context);
        render_context->device_manager->device_init();
        render_context->device_manager->init_queues();
    }

    void Renderer::init_cleanup() const
    {
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(render_context->swapchain_manager->cleanup()));
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(render_context->device_manager->cleanup()));
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(render_context->window_manager->destroy_window_glfw()));
    }

    void Renderer::cleanup()
    {
        vulkanapp::VulkanCleanupQueue::flush();
    }
}
