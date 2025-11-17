#pragma once
#include <cstdint>
#include <functional>
#include <utility>
#include "structs/engine/EngineContext.h"

namespace core
{
    class Application
    {
    public:
        explicit Application(std::shared_ptr<EngineContext> engine_context)
            : engine_context(std::move(engine_context))
        {
        }

        void application_setup(const std::function<void(Application*)>& p_application_init_callback,
                               const std::function<void(Application*)>& p_application_update_callback);
        void application_update();

    private:
        std::shared_ptr<EngineContext> engine_context;

        //Update function
        std::function<void(Application*)> application_update_callback;

    };
}