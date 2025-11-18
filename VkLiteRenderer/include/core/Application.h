#pragma once
#include <cstdint>
#include <functional>
#include <utility>
#include "structs/engine/EngineContext.h"
#include "Types.h"

namespace core
{
    class Application
    {
    public:
        explicit Application(std::shared_ptr<EngineContext> engine_context)
            : engine_context(std::move(engine_context))
        {
        }

        void application_setup(const ApplicationFunc& p_application_init_callback,
                               const ApplicationFunc& p_application_update_callback);
        void application_update();

    private:
        std::shared_ptr<EngineContext> engine_context;

        //Update function
        ApplicationFunc application_update_callback;

    };
}