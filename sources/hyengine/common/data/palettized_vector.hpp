#pragma once
#include <unordered_map>

#include "bitvector.hpp"
#include "../common.hpp"
#include "../id_generator.hpp"
#include "../sized_numerics.hpp"

namespace hyengine
{
    ///Memory-compression structure that stores tightly packed indexes into an array (palette) of data values.
    template <typename data_type>
    class palettized_vector
    {
    public:
        void clear()
        {
            ZoneScoped;
            palette_id_generator.clear();
            palette_data.clear();
            packed_ids.clear();
            bits_per_id = 0;
        }

        ///Adds a value to the palette. May cause a palette resize if needed
        u64 add_to_palette(const data_type data)
        {
            const u64 id = palette_id_generator.assign();
            resize_palette(id);
            palette_data.insert({id, data});
            return id;
        }

        ///Resizes the palette to ensure a maximum number of palette entries
        void resize_palette(const u64 max_palette_entries)
        {
            ZoneScoped;
            const u64 new_bits_per_id = bits_required_for_value(max_palette_entries);
            if (new_bits_per_id <= bits_per_id) return; //Not allowed to downsize palette

            if (packed_ids.empty()) //No data stored - just update the id size
            {
                bits_per_id = new_bits_per_id;
                return;
            }

            //Data is being stored, so we have to iterate through it and re-write it with the new size
            const u64 total_ids = packed_ids.size_in_bits() / bits_per_id;

            packed_ids.resize(new_bits_per_id * total_ids);

            //Expanding the palette will only ever push values further into the array, so we can iterate over it backwards and re-position each ID
            for (i64 id = total_ids - 1; id >= 0; id--)
            {
                const u64 data = packed_ids.read_bits(id * bits_per_id, bits_per_id);
                packed_ids.write_bits(id * new_bits_per_id, new_bits_per_id, data);
            }

            bits_per_id = new_bits_per_id;
        }

        ///Reserves space for at least max_entries values in the index vector
        void reserve_data(const u64 max_entries)
        {
            packed_ids.reserve(max_entries * bits_per_id);
        }

        ///Resizes the index vector to store at least max_entries
        void resize_data(const u64 max_entries)
        {
            packed_ids.resize(max_entries * bits_per_id);
        }

        ///Removes an index from the palette, allowing it to be replaced with a different value via @code add_to_palette@endcode. Does not resize the palette or update the index table.
        void remove_from_palette(const u64 palette_index)
        {
            palette_data.erase(palette_index);
            palette_id_generator.free(palette_index);
        }

        ///Replaces an index in the palette with a new value
        void set_palette_entry(const u64 palette_index, const data_type data)
        {
            palette_data[palette_index] = data;
        }

        [[nodiscard]] data_type get_palette_entry(const u64 palette_index)
        {
            return palette_data[palette_index];
        }

        ///Adds the given palette_index to the end of the index vector
        void push_back(const u64 palette_index)
        {
            packed_ids.push_back_bits(bits_per_id, palette_index);
        }

        ///Removes a palette index from the end of the index vector
        void pop_back()
        {
            packed_ids.pop_back_bits(bits_per_id);
        }

        ///Gets the palette index from the given index into the index vector
        [[nodiscard]] u64 get(const u32 index) const
        {
            return packed_ids.read_bits(index * bits_per_id, bits_per_id);
        }

        ///Sets the palette index at the given index into the index vector
        void set(const u32 index, const u64 palette_index)
        {
            packed_ids.write_bits(index * bits_per_id, bits_per_id, palette_index);
        }

    private:
        id_generator<u64> palette_id_generator;
        std::unordered_map<u64, data_type> palette_data;
        bitvector packed_ids;
        u8 bits_per_id = 0;
    };
}
