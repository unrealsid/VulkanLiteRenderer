
#pragma once

#include <map>
#include <string>
#include <vector>
#include "render/Renderer.h"
#include "platform/threading/JobUtils.h"
#include "structs/engine/RenderContext.h"
#include "structs/engine/Job.h"

struct RenderCommand;

namespace core
{
    class Engine
    {
    public:
        template<typename Func, typename... Args>
        static Job* create_job(const std::string& job_name, Func&& thread_func, Args&&... thread_args)
        {
            jobs[job_name] = threading::JobUtils::create_job(std::forward<Func>(thread_func), std::forward<Args>(thread_args)...);;
            return jobs[job_name].get();
        }

        void initialize_application_thread();
        void initialize_graphics_subsystem();
        void init ();

        static void update ();
        static void shutdown ();

        [[nodiscard]] const RenderContext* get_render_context() const { return render_context.get(); }

    private:

        //RenderContext is shared between Application and Render Threads
        std::unique_ptr<RenderContext> render_context;

        static std::map<std::string, std::unique_ptr<Job>> jobs;

        void render_context_init(uint32_t max_commands);
    };
} // core
