#pragma once
#include <array>
#include <cassert>

namespace hyengine::common {
    template<typename data_type, const unsigned int side_length>
    class array_3d
    {
    public:
        static unsigned int index_for(const unsigned int x, const unsigned int y, const unsigned int z)
        {
            assert(x < side_length && y < side_length && z < side_length);
            return x + y * side_length + z * side_length * side_length;
        }

        data_type& operator [](const unsigned int x, const unsigned int y, const unsigned int z)
        {
            return data[index_for(x, y, z)];
        }

        data_type& operator [](const unsigned int idx)
        {
            return data[idx];
        }

        const data_type& operator [](const unsigned int x, const unsigned int y, const unsigned int z) const
        {
            return data[index_for(x, y, z)];
        }

        const data_type& operator [](const unsigned int idx) const
        {
            return data[idx];
        }

        std::array<data_type, side_length*side_length*side_length> data {};
    };
}
