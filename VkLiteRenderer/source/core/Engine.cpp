#include "../../include/core/Engine.h"

#include <iostream>
#include <ostream>

#include "core/Application.h"
#include "structs/engine/WindowCreateParams.h"
#include "structs/engine/FrameContext.h"
#include "Config.inl"
#include "platform/WindowManager.h"
#include "renderer/Renderer.h"

namespace core
{
    std::map<std::string, std::unique_ptr<Job>> Engine::jobs = {};

    Engine::Engine(std::function<void(Application*)>&& p_application_init_callback, std::function<void(Application*)>&& p_application_update_callback) :
        frame_context(nullptr),
        application_init_callback(std::move(p_application_init_callback)),
        application_update_callback(std::move(p_application_update_callback)){ }

    void Engine::init_render_context()
    {
        frame_context = std::make_unique<FrameContext>();
    }

    void Engine::initialize_application_thread()
    {
        //Setup application thread
        create_job("application",
            [&](FrameContext* p_render_context,
            const std::function<void(Application*)>& p_application_init_callback,
            const std::function<void(Application*)>& p_application_update_callback)
        {
            auto application = std::make_unique<Application>(p_render_context);
            application->application_setup(p_application_init_callback, p_application_update_callback);

            //Application thread finishes
            threading::ThreadUtils::semaphore_post(jobs["application"]->semaphore_continue.get(), 1);
        },
        frame_context.get(),
        application_init_callback,
        application_update_callback);

        //Wait for the application thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["application"]->semaphore_continue.get());
    }

    void Engine::initialize_graphics_subsystem()
    {
        //Setup graphics thread
        create_job("rendering", []( FrameContext* p_frame_context)
        {
            const auto renderer = std::make_unique<renderer::Renderer>();
            renderer->renderer_init();

            //Post the semaphore here so the main thread can resume. We have finished initializing the graphics thread here
            threading::ThreadUtils::semaphore_post(jobs["rendering"]->semaphore_continue.get(), 1);

            renderer->renderer_update();
        }, frame_context.get());

        //Wait for the rendering thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["rendering"]->semaphore_continue.get());
    }

    void Engine::init()
    {
        init_render_context();

        initialize_application_thread();

        initialize_graphics_subsystem();

        std::cout << "\nEngine Setup" << std::endl;
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