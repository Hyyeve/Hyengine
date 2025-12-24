#pragma once
#include <array>
#include <cassert>

#include "../sized_numerics.hpp"

namespace hyengine
{
    ///Wrapper for a flat array that can be indexed with 3D coordinates. Conceptually always a cube
    template <typename data_type, const u32 side_length>
    class array_3d
    {
    public:
        ///Converts 3D coordinates into a flat index given the side length
        static u32 index_for(const u32 x, const u32 y, const u32 z)
        {
            assert(x < side_length && y < side_length && z < side_length);
            return x + y * side_length + z * side_length * side_length;
        }

        data_type& operator [](const u32 x, const u32 y, const u32 z)
        {
            return data[index_for(x, y, z)];
        }

        data_type& operator [](const u32 idx)
        {
            return data[idx];
        }

        const data_type& operator [](const u32 x, const u32 y, const u32 z) const
        {
            return data[index_for(x, y, z)];
        }

        const data_type& operator [](const u32 idx) const
        {
            return data[idx];
        }

        std::array<data_type, side_length * side_length * side_length> data {};
    };
}
