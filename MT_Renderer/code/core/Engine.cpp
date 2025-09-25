//
// Created by Sid on 9/22/2025.
//

#include "Engine.h"

#include <iostream>
#include <ostream>

#include "Application.h"
#include "rendering/Renderer.h"
#include "../structs/WindowCreationParams.h"
#include "structs/RenderContext.h"
#include "Config.inl"

namespace core
{
    std::map<std::string, std::unique_ptr<Job>> Engine::jobs = {};

    void Engine::render_context_init(uint32_t max_commands)
    {
        render_context = std::make_unique<RenderContext>(max_commands);
        render_context->consume_semaphore = std::move(threading::ThreadUtils::semaphore_create(0));
        render_context->continue_semaphore = std::move(threading::ThreadUtils::semaphore_create(0));
    }

    void Engine::initialize_application_thread()
    {
        //Setup application thread
        create_job("application", [&]()
        {
            auto application = std::unique_ptr<Application>();
            application->application_setup();

            //Application thread finishes
            threading::ThreadUtils::semaphore_post(jobs["application"]->semaphore_continue.get(), 1);
        });

        //Wait for the application thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["application"]->semaphore_continue.get());
    }

    void Engine::initialize_graphics_subsystem()
    {
        render_context_init(max_commands);

        WindowCreationParams window_creation_params{};
        window_creation_params.window_width = window_width;
        window_creation_params.window_height = window_height;
        window_creation_params.window_title = window_title;

        //Setup graphics thread
        create_job("rendering", []( WindowCreationParams window_creation_params, RenderContext* p_render_context)
        {
            auto renderer = std::make_unique<Renderer>();
            auto os_manager = std::make_unique<platform::OS_Manager>();

            os_manager->init_window(window_creation_params);
            renderer->renderer_init(p_render_context, os_manager.get());

            //Post the semaphore here so the main thread can resume. We have finished initializing the graphics thread here
            threading::ThreadUtils::semaphore_post(jobs["rendering"]->semaphore_continue.get(), 1);

            renderer->renderer_update(p_render_context);
        }, window_creation_params, render_context.get());

        //Wait for the rendering thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["rendering"]->semaphore_continue.get());
    }

    void Engine::init()
    {
        initialize_application_thread();

        initialize_graphics_subsystem();

        std::cout << "Engine Setup" << std::endl;
    }

    void Engine::update()
    {
        
    }

    void Engine::shutdown()
    {
        jobs["application"]->thread.join();
        jobs["rendering"]->thread.join();
    }
} // core