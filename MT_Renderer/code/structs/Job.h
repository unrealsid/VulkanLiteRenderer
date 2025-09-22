//
// Created by Sid on 9/22/2025.
//

#pragma once
#include <semaphore>
#include <thread>

using semaphore = std::counting_semaphore<1>;

struct Job
{
    std::thread thread;
    std::unique_ptr<semaphore> semaphore_consume;
    std::unique_ptr<semaphore> semaphore_continue;
    std::unique_ptr<semaphore> semaphore_exit;
    std::unique_ptr<semaphore> semaphore_terminated;
};
