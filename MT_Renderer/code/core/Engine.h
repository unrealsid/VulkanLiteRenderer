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
        void create_job(std::function<void(void*)> thread_func, void* thread_params, ThreadStartFlags start_flags);

    private:
        static std::vector<std::unique_ptr<threading::JobUtils>> Jobs;
    };
} // core
