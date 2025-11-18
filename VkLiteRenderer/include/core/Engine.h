
#pragma once

#include <map>
#include <string>
#include <vector>

#include "Application.h"
#include "platform/threading/JobUtils.h"
#include "structs/engine/RenderContext.h"
#include "structs/engine/EngineContext.h"
#include "structs/engine/Job.h"

struct RenderCommand;

namespace core
{
    class Engine
    {
    public:
        Engine() = default;

        //Constructor used to pass custom functions to initialize application behavior and application update
        Engine(ApplicationFunc&& p_application_init_callback,
               ApplicationFunc&& p_application_update_callback);

        template<typename Func, typename... Args>
        static Job* create_job(const std::string& job_name, Func&& thread_func, Args&&... thread_args)
        {
            jobs[job_name] = threading::JobUtils::create_job(std::forward<Func>(thread_func), std::forward<Args>(thread_args)...);
            return jobs[job_name].get();
        }

        void init ();

        void update();
        void shutdown();

        [[nodiscard]] std::shared_ptr<EngineContext> get_engine_context() const { return engine_context; }

    private:
        //FrameContext is shared between Application and Render Threads
        std::shared_ptr<EngineContext> engine_context;

        //Callbacks to be set while setting up the engine
        ApplicationFunc application_init_callback;
        ApplicationFunc application_update_callback;

        static std::map<std::string, std::unique_ptr<Job>> jobs;

        void initialize_application_thread();

        void initialize_graphics_thread();

        void init_engine_context();
    };
} // core
