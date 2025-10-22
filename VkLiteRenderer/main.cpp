#include <iostream>

#include "core/Application.h"
#include "core/Engine.h"

void setup_triangle(core::Application* application)
{
    std::cout << "Setup Triangle" << std::endl;

}

void application_update(core::Application* application)
{
    std::cout << "Application Update" << std::endl;
}

int main()
{
    core::Engine engine(setup_triangle, application_update);
    engine.init();
    engine.shutdown();

    return 0;
}
