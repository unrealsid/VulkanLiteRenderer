#pragma once

#include <cstdint>
#include <vector>
#include <optional>

#include "enums/RenderCommands.h"
#include "structs/engine/FreeSlot.h"

namespace core::render_utils
{
    class SlotResources
    {
    public:
        explicit SlotResources(uint32_t initial_capacity = 2048)
        {
            init(initial_capacity);
        }

        uint32_t get_next()
        {
            if (head == 0)
                grow();

            uint32_t idx = head;
            auto& slot = slots[idx];

            // pop head
            head = slot.next;

            // mark used
            slot.flags = ResourceFlags::Used;

            return idx;
        }

        bool free(uint32_t slot_index)
        {
            // reserved null slot
            if (slot_index == 0)
                return false;

            auto& slot = slots[slot_index];
            if (slot.flags == ResourceFlags::Free)
                return false; // double free

            slot.flags = ResourceFlags::Free;
            slot.next = head;
            head = slot_index;

            return true;
        }

        [[nodiscard]] uint32_t capacity() const noexcept { return static_cast<uint32_t>(slots.size()); }

    private:
        std::vector<FreeSlot> slots{};
        uint32_t head = 0;

        void init(uint32_t num)
        {
            slots.resize(num);

            // slot 0 is reserved as null handle
            for (uint32_t i = num - 1; i > 0; --i)
            {
                slots[i].index = i;
                slots[i].next = head;
                slots[i].flags = ResourceFlags::Free;
                head = i;
            }
        }

        void grow()
        {
            uint32_t cur_cap = capacity();
            uint32_t new_cap = cur_cap * 2;
            slots.resize(new_cap);

            // link new slots into free list
            for (uint32_t i = new_cap - 1; i > cur_cap; --i)
            {
                slots[i].index = i;
                slots[i].next = head;
                slots[i].flags = ResourceFlags::Free;
                head = i;
            }
        }
    };
}
