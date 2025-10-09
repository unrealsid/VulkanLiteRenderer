
#include "render/Renderer.h"
#include "structs/engine/RenderContext.h"
#include "vulkanapp/VulkanCleanupQueue.h"

namespace core::renderer
{
    void Renderer::init_vulkan()
    {
        render_context = std::make_unique<RenderContext>();

        create_window();
        create_device();
        create_swapchain();
    }

    void Renderer::create_window()
    {
        render_context->window_manager = std::make_unique<platform::WindowManager>();
        render_context->window_manager->create_window_glfw(
     {
            window_width, window_height, window_title
        });
    }

    void Renderer::create_swapchain() const
    {
        render_context->swapchain_manager = std::make_unique<vulkanapp::SwapchainManager>(*render_context);
        render_context->swapchain_manager->create_swapchain();
    }

    void Renderer::create_device() const
    {
        render_context->device_manager = std::make_unique<vulkanapp::DeviceManager>(*render_context);
        render_context->device_manager->device_init();
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
