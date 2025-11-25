#include "bitvector.hpp"
#include <vector>
#include <tracy/Tracy.hpp>
#include "../sized_numerics.hpp"

namespace hyengine
{
    void bitvector::clear()
    {
        ZoneScoped;
        data.clear();
        total_bits_count = 0;
    }

    void bitvector::shrink_to_fit()
    {
        ZoneScoped;
        data.shrink_to_fit();
    }

    void bitvector::reserve(const u64 count_bits)
    {
        ZoneScoped;
        data.reserve(count_bits * 64);
    }

    void bitvector::resize(const u64 count_bits)
    {
        ZoneScoped;
        data.resize((63 + count_bits) / 64);
        total_bits_count = count_bits;
    }

    void bitvector::push_back_bits(const u64 bits, const u8 bits_count)
    {
        if (bits_count > 64)
        {
            return;
        }

        const u64 insert_position = total_bits_count;
        total_bits_count += bits_count;

        const u64 size_elements = data.size();
        const u64 new_size_elements = (63 + total_bits_count) / 64;
        if (new_size_elements > size_elements) data.push_back(0);

        insert_bits(insert_position, bits_count, bits);
    }

    void bitvector::pop_back_bits(const u64 count_bits)
    {
        if (count_bits >= total_bits_count)
        {
            clear();
            return;
        }

        total_bits_count -= count_bits;
        resize(total_bits_count);
    }

    bool bitvector::empty() const
    {
        return total_bits_count == 0;
    }

    u64 bitvector::size_in_bits() const
    {
        return total_bits_count;
    }

    u64 bitvector::capacity_in_bits() const
    {
        return data.capacity() * 64;
    }


    void bitvector::insert_bits(const u64 first_bit, const u64 bits_count, const u8 bits_data)
    {
        if (bits_count > 64)
        {
            return;
        }

        const u64 last_bit = first_bit + bits_count - 1;
        const u64 bits_offset = first_bit % 64;

        const u64 index_start = first_bit / 64;
        const u64 index_end = last_bit / 64;

        const u64 mask = (1ull << bits_count) - 1ull;
        const u64 masked_bits = bits_data & mask;

        data[index_start] &= ~(mask << bits_offset);
        data[index_start] |= masked_bits << bits_offset;

        if (index_start == index_end) return;

        data[index_end] &= ~(mask >> (64 - bits_offset));
        data[index_end] |= masked_bits >> (64 - bits_offset);
    }

    u64 bitvector::extract_bits(const u64 first_bit, const u64 bits_count) const
    {
        if (bits_count > 64)
        {
            return 0;
        }

        const u64 last_bit = first_bit + bits_count - 1;
        const u64 bits_offset = first_bit % 64;
        const u64 mask = (1ull << bits_count) - 1;

        const u64 index_start = first_bit / 64;
        const u64 index_end = last_bit / 64;

        u64 result = (data[index_start] >> bits_offset) & mask;
        if (index_start == index_end) return result;

        result |= (data[index_end] << (64 - bits_offset)) & mask;
        return result;
    }
}
