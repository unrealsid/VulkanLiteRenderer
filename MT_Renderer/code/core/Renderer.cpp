#include "Renderer.h"

#include <iostream>
#include <ostream>
#include "platform/OS_Manager.h"

void core::Renderer::renderer_setup(WindowCreationParams creation_params)
{
    std::cout << "Renderer Setup" << std::endl;

    os_manager = std::make_unique<platform::OS_Manager>();
    os_manager->init_window(creation_params);
    os_manager->load_opengl();

    renderer_update();
}

void core::Renderer::renderer_update()
{
    std::cout << "Renderer Update" << std::endl;
    os_manager->os_update();
}
