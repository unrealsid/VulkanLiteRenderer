//
// Created by Sid on 9/22/2025.
//

#include "ThreadUtils.h"

namespace threading
{
    std::thread ThreadUtils::create_thread(const std::function<void(void*)>& thread_func, void* thread_params, ThreadStartFlags flags)
    {
        std::thread thread(thread_func, thread_params);
        thread.detach();

        return thread;
    }

    std::unique_ptr<semaphore> ThreadUtils::semaphore_create(uint32_t initial_count)
    {
        return std::make_unique<semaphore>(initial_count);
    }

    bool ThreadUtils::semaphore_try_wait(semaphore* semaphore)
    {
        return semaphore->try_acquire();
    }

    void ThreadUtils::semaphore_wait(semaphore* semaphore)
    {
        semaphore->acquire();
    }

    void ThreadUtils::semaphore_post(semaphore* semaphore, uint32_t count)
    {
        semaphore->release(count);
    }
}
