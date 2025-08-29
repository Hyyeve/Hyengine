#pragma once
#include <unordered_map>

#include "bitvector.hpp"
#include "../common.hpp"
#include "../id_generator.hpp"
#include "../sized_numerics.hpp"

namespace hyengine
{
    template<typename data_type>
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

        u64 add_to_palette(const data_type data)
        {
            const u64 id = palette_id_generator.assign();
            resize_palette(id);
            palette_data.insert({id, data});
            return id;
        }

        void resize_palette(const u64 max_palette_id)
        {
            ZoneScoped;
            const u64 new_bits_per_id = bits_required_for_value(max_palette_id);
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
                const u64 data = packed_ids.extract_bits(id * bits_per_id, bits_per_id);
                packed_ids.insert_bits(id * new_bits_per_id, new_bits_per_id, data);
            }

            bits_per_id = new_bits_per_id;
        }

        void reserve_data(const u64 max_entries)
        {
            packed_ids.reserve(max_entries * bits_per_id);
        }

        void resize_data(const u64 max_entries)
        {
            packed_ids.resize(max_entries * bits_per_id);
        }

        void remove_from_palette(const u64 palette_id)
        {
            palette_data.erase(palette_id);
            palette_id_generator.free(palette_id);
        }

        void set_palette_entry(const u64 palette_id, const data_type data)
        {
            palette_data[palette_id] = data;
        }

        [[nodiscard]] data_type get_palette_entry(const u64 palette_id)
        {
            return palette_data[palette_id];
        }

        void push_back(const u64 palette_id)
        {
            packed_ids.push_back_bits(palette_id, bits_per_id);
        }

        void pop_back()
        {
            packed_ids.pop_back_bits(bits_per_id);
        }

        [[nodiscard]] u64 get(const u32 index) const
        {
             return packed_ids.extract_bits(index * bits_per_id, bits_per_id);
        }

        void set(const u32 index, const u64 palette_id)
        {
            packed_ids.insert_bits(index * bits_per_id, bits_per_id, palette_id);
        }

    private:
        id_generator<u64> palette_id_generator;
        std::unordered_map<u64, data_type> palette_data;
        bitvector packed_ids;
        u8 bits_per_id = 0;
    };
}