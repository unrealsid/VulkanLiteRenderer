
#pragma once
#include <map>
#include <vector>

#include "Application.h"
#include "Renderer.h"
#include "platform/threading/JobUtils.h"

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

        void init ();
        void shutdown ();

    private:
        std::unique_ptr<Application> app;
        std::unique_ptr<Renderer> renderer;
        static std::map<std::string, std::unique_ptr<Job>> jobs;
    };
} // core
