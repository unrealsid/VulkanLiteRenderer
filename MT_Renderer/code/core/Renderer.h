#pragma once
#include <memory>

#include "platform/OS_Manager.h"

struct WindowCreationParams;

namespace core
{
    class Renderer
    {
    public:
        void renderer_setup(WindowCreationParams creation_params);
        void renderer_update();

    private:
        std::unique_ptr<platform::OS_Manager> os_manager;
    };
}
