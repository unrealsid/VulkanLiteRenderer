//
// Created by Sid on 9/22/2025.
//

#include "Engine.h"

#include <iostream>
#include <ostream>

#include "Application.h"
#include "Renderer.h"
#include "structs/WindowCreationParams.h"

namespace core
{
    std::map<std::string, std::unique_ptr<Job>> Engine::jobs = {};

    void Engine::init()
    {
        app = std::make_unique<Application>();
        renderer = std::make_unique<Renderer>();

        //Setup application thread
        create_job("application", [&]()
        {
            app->application_setup();
            threading::ThreadUtils::semaphore_post(jobs["application"]->semaphore_continue.get(), 1);
        });

        threading::ThreadUtils::semaphore_wait(jobs["application"]->semaphore_continue.get());

        WindowCreationParams window_creation_params{};
        window_creation_params.window_width = 1024;
        window_creation_params.window_height = 768;
        window_creation_params.window_title = "MTRenderer";

        //Setup graphics thread
        create_job("rendering", [this]( WindowCreationParams window_creation_params)
        {
            renderer->renderer_setup(window_creation_params);

            threading::ThreadUtils::semaphore_post(jobs["rendering"]->semaphore_continue.get(), 1);
        }, window_creation_params);

        threading::ThreadUtils::semaphore_wait(jobs["rendering"]->semaphore_continue.get());

        std::cout << "Engine Setup" << std::endl;
    }

    void Engine::shutdown()
    {

    }
} // core