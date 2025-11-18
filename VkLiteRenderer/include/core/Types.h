
#pragma once
#include <semaphore>
#include <functional>

namespace core
{
    class Application;
}

using Semaphore = std::counting_semaphore<1>;
using ApplicationFunc = std::function<void(core::Application*)>;
