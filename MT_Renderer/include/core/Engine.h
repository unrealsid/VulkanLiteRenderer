﻿
#pragma once

#include <map>
#include <string>
#include <vector>
#include "render/Renderer.h"
#include "platform/threading/JobUtils.h"
#include "structs/engine/RenderContext.h"
#include "structs/engine/FrameContext.h"
#include "structs/engine/Job.h"

struct RenderCommand;

namespace core
{
    class Engine
    {
    public:
        Engine() = default;

        //Constructor mainly used to pass custom functions to initialize application behavior and application update
        Engine(std::function<void()>&& p_application_init_callback, std::function<void()>&& p_application_update_callback);

        template<typename Func, typename... Args>
        static Job* create_job(const std::string& job_name, Func&& thread_func, Args&&... thread_args)
        {
            jobs[job_name] = threading::JobUtils::create_job(std::forward<Func>(thread_func), std::forward<Args>(thread_args)...);;
            return jobs[job_name].get();
        }

        void initialize_application_thread();

        void initialize_graphics_subsystem();

        void init ();

        void update ();
        void shutdown ();

        [[nodiscard]] const FrameContext* get_render_context() const { return frame_context.get(); }

    private:
        //FrameContext is shared between Application and Render Threads
        std::unique_ptr<FrameContext> frame_context;

        //Callbacks to be set while setting up the engine
        std::function<void()> application_init_callback;
        std::function<void()> application_update_callback;

        static std::map<std::string, std::unique_ptr<Job>> jobs;

        void init_render_context(uint32_t max_commands);
    };
} // core
