#include "../../include/core/Engine.h"

#include <iostream>
#include <ostream>

#include "../../include/core/Application.h"
#include "../../include/render/Renderer.h"
#include "structs/engine/WindowCreateParams.h"
#include "structs/engine/FrameContext.h"
#include "Config.inl"
#include "platform/WindowManager.h"

namespace core
{
    std::map<std::string, std::unique_ptr<Job>> Engine::jobs = {};

    void Engine::init_render_context(uint32_t max_commands)
    {
        render_context = std::make_unique<FrameContext>(max_commands);
    }

    void Engine::initialize_application_thread()
    {
        //Setup application thread
        create_job("application", [&](FrameContext* p_render_context)
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
        //Setup graphics thread
        create_job("rendering", []( FrameContext* p_render_context)
        {
            auto renderer = std::make_unique<renderer::Renderer>();
            renderer->renderer_init(p_render_context);

            //Post the semaphore here so the main thread can resume. We have finished initializing the graphics thread here
            threading::ThreadUtils::semaphore_post(jobs["rendering"]->semaphore_continue.get(), 1);

            renderer->renderer_update();
        }, render_context.get());

        //Wait for the rendering thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["rendering"]->semaphore_continue.get());
    }

    void Engine::init()
    {
        init_render_context(max_commands);

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