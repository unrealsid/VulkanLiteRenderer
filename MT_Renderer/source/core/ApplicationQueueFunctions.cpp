//This is where all functions go that write to queues
#include "core/Application.h"
#include "enums/ResourceFlags.h"
#include "structs/engine/ApplicationQueueStructs.h"
#include "structs/engine/RenderCommand.h"

namespace core
{
    uint32_t Application::renderer_create_clear_state(const ClearState& cs) const
    {

        return 1;
    }

    uint32_t Application::renderer_create_raster_state(const RasterStateCreationParams& rscp) const
    {

        return 1;
    }
}
