//
// Created by Sid on 9/22/2025.
//

#pragma once
#include <vector>
#include "platform/threading/JobUtils.h"

namespace core
{
    class Engine
    {
    public:
        template<typename... Args>
        static void create_job(const std::function<void(void*)>& thread_func, Args&&... thread_args)
        {
            auto job = threading::JobUtils::create_job(thread_func, std::forward<Args>(thread_args)...);
            jobs.push_back(std::make_unique<Job>(std::move(job)));
        }

    private:
        static std::vector<std::unique_ptr<Job>> jobs;
    };
} // core
