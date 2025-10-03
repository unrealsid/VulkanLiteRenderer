#pragma once
#include <functional>
#include <semaphore>
#include <thread>
#include "core/Types.h"

namespace threading
{
   class ThreadUtils
    {
    public:
        template<typename Func, typename... Args>
        static std::thread create_thread(Func&& thread_func, bool p_start_detached, Args&&... thread_params)
        {
            std::thread thread(std::forward<Func>(thread_func), std::forward<Args>(thread_params)...);

            if (p_start_detached)
            {
                thread.detach();
            }

            return thread;
        }

        static std::unique_ptr<Semaphore> semaphore_create(uint32_t initial_count);
        static bool semaphore_try_wait(Semaphore* semaphore);
        static void semaphore_wait(Semaphore* semaphore);
        static void semaphore_post(Semaphore* semaphore, uint32_t count);
    };
}
