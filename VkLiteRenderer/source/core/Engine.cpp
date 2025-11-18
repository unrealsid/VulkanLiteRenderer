#include "../../include/core/Engine.h"

#include <iostream>
#include <ostream>

#include "core/Application.h"
#include "structs/engine/WindowCreateParams.h"
#include "structs/engine/EngineContext.h"
#include "Config.inl"
#include "platform/WindowManager.h"
#include "renderer/Renderer.h"
#include "vulkanapp/VulkanCleanupQueue.h"

namespace core
{
    std::map<std::string, std::unique_ptr<Job>> Engine::jobs = {};

    Engine::Engine(std::function<void(Application*)>&& p_application_init_callback,
        std::function<void(Application*)>&& p_application_update_callback) :

        engine_context(nullptr),
        application_init_callback(std::move(p_application_init_callback)),
        application_update_callback(std::move(p_application_update_callback)){ }

    void Engine::init_engine_context()
    {
        engine_context = std::make_shared<EngineContext>();
    }

    void Engine::initialize_application_thread()
    {
        //Setup application thread
        create_job("application",
            [](const std::shared_ptr<EngineContext>& p_engine_context,
                const std::function<void(Application*)>& p_application_init_callback,
                const std::function<void(Application*)>& p_application_update_callback)
        {
            auto application = std::make_unique<Application>(p_engine_context);
            application->application_setup(p_application_init_callback, p_application_update_callback);

            //Application thread finishes
            threading::ThreadUtils::semaphore_post(jobs["application"]->semaphore_continue.get(), 1);

            //Call app update after we finish setting up the application thread. Also, let the rest of the engine continue with init
            application->application_update();
        },
        //Arguments to pass
        std::ref(engine_context), //pass engine context by ref since the function will otherwise pass it by value and I dont want to increase the ref counter here.
        application_init_callback,
        application_update_callback);

        //Wait for the application thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["application"]->semaphore_continue.get());
    }

    void Engine::initialize_graphics_thread()
    {
        //Setup graphics thread
        create_job("rendering", [](std::shared_ptr<EngineContext> p_engine_context)
        {
            const auto renderer = std::make_unique<renderer::Renderer>(std::move(p_engine_context), 2);
            renderer->renderer_init();

            //Post the semaphore here so the main thread can resume. We have finished initializing the graphics thread here
            threading::ThreadUtils::semaphore_post(jobs["rendering"]->semaphore_continue.get(), 1);

            renderer->renderer_update();
        }, std::ref(engine_context)); //pass engine context by ref since the function will otherwise pass it by value and I dont want to increase the ref counter here.

        //Wait for the rendering thread to finish initializing
        threading::ThreadUtils::semaphore_wait(jobs["rendering"]->semaphore_continue.get());
    }

    void Engine::init()
    {
        init_engine_context();

        initialize_application_thread();

        initialize_graphics_thread();

        std::cout << "\nEngine Setup" << std::endl;
    }

    void Engine::update()
    {
        
    }

    void Engine::shutdown()
    {
        jobs["application"]->thread.join();
        jobs["rendering"]->thread.join();

        vulkanapp::VulkanCleanupQueue::push_cleanup_function(CLEANUP_FUNCTION(engine_context->window_manager->destroy_window_glfw()));
    }
} // core