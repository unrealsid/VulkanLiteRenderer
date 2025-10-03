//
// Created by Sid on 9/22/2025.
//

#include "platform/threading/ThreadUtils.h"

namespace threading
{
    std::unique_ptr<Semaphore> ThreadUtils::semaphore_create(uint32_t initial_count)
    {
        return std::make_unique<Semaphore>(initial_count);
    }

    bool ThreadUtils::semaphore_try_wait(Semaphore* semaphore)
    {
        return semaphore->try_acquire();
    }

    void ThreadUtils::semaphore_wait(Semaphore* semaphore)
    {
        semaphore->acquire();
    }

    void ThreadUtils::semaphore_post(Semaphore* semaphore, uint32_t count)
    {
        semaphore->release(count);
    }
}
