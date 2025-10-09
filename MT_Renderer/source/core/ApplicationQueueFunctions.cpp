//This is where all functions go that write to queues
#include "core/Application.h"
#include "enums/ResourceFlags.h"
#include "structs/engine/ApplicationQueueStructs.h"
#include "structs/engine/RenderCommand.h"

namespace core
{
    uint32_t Application::renderer_create_clear_state(const ClearState& cs) const
    {
        RenderCommand command;

        uint32_t resource_slot = frame_context->slot_resources.get_next();

        command.command_index = CMD_CREATE_CLEAR_STATE;
        command.render_data = cs;
        command.resource_slot = resource_slot;

        add_cmd(command);

        return resource_slot;
    }

    uint32_t Application::renderer_create_raster_state(const RasterStateCreationParams& rscp) const
    {
        RenderCommand command;

        command.command_index = CMD_CREATE_RASTER_STATE;
        uint32_t resource_slot = frame_context->slot_resources.get_next();
        command.resource_slot = resource_slot;

        add_cmd(command);

        return resource_slot;
    }
}
