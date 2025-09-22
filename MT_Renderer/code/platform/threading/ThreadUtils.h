//
// Created by Sid on 9/22/2025.
//

#pragma once
#include <functional>
#include <semaphore>
#include <thread>
#include "enums/ThreadStartFlags.h"

namespace threading
{
    using semaphore = std::counting_semaphore<1>;

    class ThreadUtils
    {
    public:
        static std::thread create_thread(const std::function<void(void*)>& thread_func, void* thread_params, ThreadStartFlags flags);

        static std::unique_ptr<semaphore> semaphore_create(uint32_t initial_count);
        static bool semaphore_try_wait(semaphore* semaphore);
        static void semaphore_wait(semaphore* semaphore);
        static void semaphore_post(semaphore* semaphore, uint32_t count);
    };
}
