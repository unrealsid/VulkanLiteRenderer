//
// Created by Sid on 9/22/2025.
//

#include "Engine.h"

namespace core
{
    std::vector<std::unique_ptr<Job>> Engine::jobs = {};

    void Engine::create_job(const std::function<void(void*)>& thread_func, void* thread_params, ThreadStartFlags start_flags)
    {
        auto job = threading::JobUtils::create_job(thread_func, thread_params, start_flags);
        jobs.push_back(std::make_unique<Job>(std::move(job)));
    }
} // core