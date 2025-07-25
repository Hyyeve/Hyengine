#pragma once
#include <array>
#include <tracy/Tracy.hpp>

namespace hyengine
{
    template <typename T, int capacity>
    class ring_buffer
    {
    public:
        void push(T value)
        {
            ZoneScoped;
            elements[write_index] = value;
            write_index++;
            write_index %= capacity;
            max_read_index++;
        }

        [[nodiscard]] T read()
        {
            ZoneScoped;
            T elem = elements[read_index];
            read_index++;
            read_index %= capacity;
            return elem;
        }

        [[nodiscard]] T operator[](const size_t index) const
        {
            ZoneScoped;
            return elements[index % capacity];
        }

        [[nodiscard]] static int size()
        {
            ZoneScoped;
            return capacity;
        }

        [[nodiscard]] int max_index() const
        {
            ZoneScoped;
            return max_read_index >= capacity ? capacity - 1 : max_read_index;
        }

    private:
        std::array<T, capacity> elements;
        int write_index = 0;
        int read_index = 0;
        int max_read_index = -1;
    };
}
