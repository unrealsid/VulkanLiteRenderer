//
// Created by Sid on 9/22/2025.
//

#include "Engine.h"

namespace core
{
    std::vector<std::unique_ptr<threading::JobUtils>> Jobs = {};

    void Engine::create_job(std::function<void(void*)> thread_func, void* thread_params, ThreadStartFlags start_flags)
    {

    }
} // core