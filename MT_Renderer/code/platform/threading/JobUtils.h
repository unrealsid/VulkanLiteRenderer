//
// Created by Sid on 9/22/2025.
//

#pragma once
#include <functional>
#include <semaphore>
#include <thread>
#include "ThreadUtils.h"
#include "structs/Job.h"

namespace threading
{
   class JobUtils
    {
    public:
       template<typename... Args>
       static Job create_job(const std::function<void()>& thread_func, Args&&... thread_params)
       {
           Job job;
           job.thread = ThreadUtils::create_thread(thread_func, std::forward<Args>(thread_params)...);

           job.semaphore_continue = ThreadUtils::semaphore_create(0);
           job.semaphore_consume = ThreadUtils::semaphore_create(0);
           job.semaphore_exit = ThreadUtils::semaphore_create(0);
           job.semaphore_terminated = ThreadUtils::semaphore_create(0);

           ThreadUtils::semaphore_wait(job.semaphore_continue.get());

           return job;
       }
    };
} // threading
