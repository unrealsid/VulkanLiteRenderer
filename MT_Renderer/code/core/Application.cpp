//
// Created by Sid on 9/23/2025.
//

#include "Application.h"

#include <iostream>
#include <ostream>
#include <thread>

namespace core
{
    void Application::application_setup()
    {
        std::cout << "Application Setup" << std::endl;

        //Start Application update
        application_update();
    }

    void Application::application_update()
    {

    }
} // core