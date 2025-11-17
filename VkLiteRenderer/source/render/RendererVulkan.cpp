
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

        create_device();
        create_swapchain();
        utils::MemoryUtils::create_vma_allocator(*render_context->device_manager);
    }

    void Renderer::create_swapchain() const
    {
        render_context->swapchain_manager = std::make_unique<vulkanapp::SwapchainManager>(render_context.get());
        render_context->swapchain_manager->initialize(render_context->device_manager->get_physical_device(),
                                                      render_context->device_manager->get_device(),
                                                      //Windowing
                                                      render_context->device_manager->get_surface(),
                                                      2,
                                                      engine_context->window_manager->get_window_width(),
                                                      engine_context->window_manager->get_window_height());
    }

    void Renderer::create_device() const
    {
        render_context->device_manager = std::make_unique<vulkanapp::DeviceManager>(*render_context, engine_context);
        render_context->device_manager->device_init();
        render_context->device_manager->init_queues();
    }

    void Renderer::init_cleanup() const
    {
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(render_context->swapchain_manager->cleanup()));
        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(render_context->device_manager->cleanup()));
    }

    void Renderer::cleanup()
    {
        vulkanapp::VulkanCleanupQueue::flush();
    }
}
