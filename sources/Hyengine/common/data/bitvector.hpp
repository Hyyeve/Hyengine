#pragma once
#include <vector>
#include "../sized_numerics.hpp"

namespace hyengine
{
    class bitvector
    {
    public:
        void clear();
        void shrink_to_fit();
        void reserve(const u64 count_bits);
        void resize(const u64 count_bits);

        void push_back_bits(const u64 bits, const u8 bits_count);
        void pop_back_bits(const u64 count_bits);

        [[nodiscard]] bool empty() const;
        [[nodiscard]] u64 size_in_bits() const;
        [[nodiscard]] u64 capacity_in_bits() const;

        void insert_bits(const u64 first_bit, const u64 bits_count, const u8 bits_data);
        [[nodiscard]] u64 extract_bits(const u64 first_bit, const u64 bits_count) const;

    private:
        std::vector<u64> data;
        u64 total_bits_count = 0;
    };
}