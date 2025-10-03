#pragma once
#include <memory>
#include <VkBootstrapDispatch.h>
#include "vulkanapp/SwapchainManager.h"
#include "platform/OS_Manager.h"
#include "vulkanapp/DeviceManager.h"

struct EngineContext
{
    std::unique_ptr<platform::OS_Manager> os_manager;
    std::unique_ptr<vulkanapp::DeviceManager> device_manager;
    std::unique_ptr<vulkanapp::SwapchainManager> swapchain_manager;

    vkb::InstanceDispatchTable instance_dispatch_table;
    vkb::DispatchTable dispatch_table;
};
