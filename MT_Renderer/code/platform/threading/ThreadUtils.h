//
// Created by Sid on 9/22/2025.
//

#pragma once
#include <functional>
#include <semaphore>
#include <thread>

#include "core/Types.h"
#include "enums/ThreadStartFlags.h"

namespace threading
{
   class ThreadUtils
    {
    public:
        template<typename Func, typename... Args>
        static std::thread create_thread(Func&& thread_func, Args&&... thread_params)
        {
            std::thread thread(std::forward<Func>(thread_func), std::forward<Args>(thread_params)...);
            thread.detach();

            return thread;
        }

        static std::unique_ptr<semaphore> semaphore_create(uint32_t initial_count);
        static bool semaphore_try_wait(semaphore* semaphore);
        static void semaphore_wait(semaphore* semaphore);
        static void semaphore_post(semaphore* semaphore, uint32_t count);
    };
}
