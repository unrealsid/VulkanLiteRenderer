//
// Created by Sid on 9/22/2025.
//

#include "JobUtils.h"

#include "ThreadUtils.h"

namespace threading
{
    Job JobUtils::create_job(const std::function<void(void*)>& thread_func, void* thread_params, ThreadStartFlags start_flags)
    {
        Job job;
        job.thread = ThreadUtils::create_thread(thread_func, thread_params, start_flags);

        job.semaphore_continue = ThreadUtils::semaphore_create(0);
        job.semaphore_consume = ThreadUtils::semaphore_create(0);
        job.semaphore_exit = ThreadUtils::semaphore_create(0);
        job.semaphore_terminated = ThreadUtils::semaphore_create(0);

        ThreadUtils::semaphore_wait(job.semaphore_continue.get());

        return job;
    }
} // threading