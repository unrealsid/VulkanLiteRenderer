//
// Created by Sid on 9/22/2025.
//

#pragma once
#include <functional>
#include <semaphore>
#include <thread>

#include "enums/ThreadStartFlags.h"
#include "structs/Job.h"

namespace threading
{
   class JobUtils
    {
    public:
        static Job create_job(const std::function<void(void*)>& thread_func, void* thread_params, ThreadStartFlags start_flags);
    };
} // threading
