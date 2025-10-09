#include <iostream>
#include "core/Application.h"
#include "include/core/Engine.h"

void setup_triangle(core::Application* application)
{
    std::cout << "Setup Triangle" << std::endl;

    //Setup clear state
    ClearState clear_state{glm::vec4(1.0f, 0.5f, .5f, 1.0f), 1.0f, 0};
    application->clear_state_resource = application->renderer_create_clear_state(clear_state);
    application->create_raster_state_resource = application->renderer_create_raster_state({});
}

void application_update(core::Application* application)
{
    std::cout << "Application Update" << std::endl;
}

int main()
{
    core::Engine engine(setup_triangle, application_update);
    engine.init();
    engine.update();
    engine.shutdown();

    return 0;
}