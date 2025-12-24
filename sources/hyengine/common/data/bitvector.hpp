#pragma once
#include <vector>
#include "../sized_numerics.hpp"

namespace hyengine
{
    ///Tightly-packed vector of bitstrings, stored as u64s
    class bitvector
    {
    public:
        void clear();
        void shrink_to_fit();

        ///@param num The size, in bits, to reserve in the vector.
        void reserve(const u64 num);

        ///@param num The size, in bits, to resize the vector to.
        void resize(const u64 num);

        ///Writes num bits onto the end of the bitstream. num must be <= 64 (the most bits that can be written at once)
        void push_back_bits(const u8 num, const u64 values);

        ///Removes num bits from the end of the bitstream.
        void pop_back_bits(const u64 num);

        [[nodiscard]] bool empty() const;
        [[nodiscard]] u64 size_in_bits() const;
        [[nodiscard]] u64 capacity_in_bits() const;

        ///Writes num bits, starting from index at. num must be <= 64 (the most bits that can be written at once)
        void write_bits(const u64 at, const u64 num, const u64 values);

        ///Reads num bits, start from index at. num must be <= 64 (the most bits that can be returned at once)
        [[nodiscard]] u64 read_bits(const u64 at, const u64 num) const;

    private:
        std::vector<u64> data;
        u64 total_bits_count = 0;
    };
}
