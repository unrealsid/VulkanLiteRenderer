#pragma once

#include <memory>
#include "vulkanapp/SwapchainManager.h"
#include "platform/WindowManager.h"
#include "vulkanapp/DeviceManager.h"

/*
 * Stores Rendering content. Only used on the Rendering Thread
 */

struct RenderContext
{
    std::unique_ptr<platform::WindowManager> window_manager;
    std::unique_ptr<vulkanapp::DeviceManager> device_manager;
    std::unique_ptr<vulkanapp::SwapchainManager> swapchain_manager;

    vkb::InstanceDispatchTable instance_dispatch_table;
    vkb::DispatchTable dispatch_table;
};
