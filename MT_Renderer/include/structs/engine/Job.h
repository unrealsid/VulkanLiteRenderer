
#pragma once
#include <semaphore>
#include <thread>
#include "core/Types.h"

struct Job
{
    std::thread thread;
    std::unique_ptr<Semaphore> semaphore_consume;
    std::unique_ptr<Semaphore> semaphore_continue;
    std::unique_ptr<Semaphore> semaphore_exit;
    std::unique_ptr<Semaphore> semaphore_terminated;
};
