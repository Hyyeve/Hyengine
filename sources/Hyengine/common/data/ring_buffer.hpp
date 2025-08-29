#pragma once
#include <array>

#include "../sized_numerics.hpp"

namespace hyengine
{
    template <typename T, i32 capacity>
    class ring_buffer
    {
    public:
        void push(T value)
        {
            elements[write_index] = value;
            write_index++;
            write_index %= capacity;
            max_read_index++;
        }

        [[nodiscard]] T read()
        {
            T elem = elements[read_index];
            read_index++;
            read_index %= capacity;
            return elem;
        }

        [[nodiscard]] T operator[](const size_t index) const
        {
            return elements[index % capacity];
        }

        [[nodiscard]] static i32 size()
        {
            return capacity;
        }

        [[nodiscard]] i32 max_index() const
        {
            return max_read_index >= capacity ? capacity - 1 : max_read_index;
        }

    private:
        std::array<T, capacity> elements;
        i32 write_index = 0;
        i32 read_index = 0;
        i32 max_read_index = -1;
    };
}
