#include "Engine.h"

#include <iostream>
#include <ostream>

#include "Application.h"
#include "rendering/Renderer.h"
#include "../structs/WindowCreateParams.h"
#include "structs/RenderContext.h"
#include "Config.inl"

namespace core
{
    std::map<std::string, std::unique_ptr<Job>> Engine::jobs = {};

    void Engine::render_context_init(uint32_t max_commands)
    {
        render_context = std::make_unique<RenderContext>(max_commands);
    }

    void Engine::initialize_application_thread()
    {
        //Setup application thread
        create_job("application", [&](RenderContext* p_render_context)
        {
            auto application = std::make_unique<Application>(p_render_context);
            application->application_setup();

            //Application thread finishes
            threading::ThreadUtils::semaphore_post(jobs["application"]->semaphore_continue.get(), 1);
        }, render_context.get());

        //Wait for the application thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["application"]->semaphore_continue.get());
    }

    void Engine::initialize_graphics_subsystem()
    {
        WindowCreateParams window_create_params{};
        window_create_params.window_width = window_width;
        window_create_params.window_height = window_height;
        window_create_params.window_title = window_title;

        //Setup graphics thread
        create_job("rendering", []( WindowCreateParams window_creation_params, RenderContext* p_render_context)
        {
            auto renderer = std::make_unique<renderer::Renderer>();
            auto os_manager = std::make_unique<platform::OS_Manager>();

            os_manager->init_window(window_creation_params);
            renderer->renderer_init(p_render_context, os_manager.get());

            //Post the semaphore here so the main thread can resume. We have finished initializing the graphics thread here
            threading::ThreadUtils::semaphore_post(jobs["rendering"]->semaphore_continue.get(), 1);

            renderer->renderer_update(p_render_context);
        }, window_create_params, render_context.get());

        //Wait for the rendering thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["rendering"]->semaphore_continue.get());
    }

    void Engine::init()
    {
        render_context_init(max_commands);

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