#pragma once

#include <atomic>
#include <cstdint>
#include <optional>
#include <vector>

namespace core::render_utils
{
    template <typename T>
    class RenderBuffer
    {
    public:
        RenderBuffer() = default;

        explicit RenderBuffer(uint32_t capacity) : get_pos(0), put_pos(0), capacity(capacity)
        {
            this->capacity = capacity;
            data.reserve(capacity);
        }

        void put(const T& item)
        {
            data[put_pos] = item;
            put_pos = (put_pos + 1) % capacity;
        }

        std::optional<T> get()
        {
            size_t gp = get_pos.load();
            if (gp == put_pos.load())
            {
                return std::nullopt;
            }

            get_pos = (gp + 1) % capacity;
            return data[gp];
        }

        std::optional<T>  check()
        {
            size_t gp = get_pos.load();
            if (gp == put_pos.load())
            {
                return std::nullopt;
            }

            return data[gp];
        }

    private:
        std::vector<T> data;

        std::atomic<uint32_t>  get_pos;
        std::atomic<uint32_t>  put_pos;
        std::atomic<size_t>    capacity;
    };
}