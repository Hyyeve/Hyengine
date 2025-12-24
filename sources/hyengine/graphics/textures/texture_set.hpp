#pragma once

#include <unordered_map>
#include "../../common/id_generator.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    class texture_set
    {
    public:
        u32 allocate_slot(const u32 texture_id, const u32 sampler_state_id = 0);
        void set_sampler_state(const u32 texture_id, const u32 sampler_state_id);

        void free_slot(const u32 texture_id);

        [[nodiscard]] u32 get_slot(const u32 texture_id) const;
        [[nodiscard]] bool has_space() const;

        static u32 max_slots();

        void clear();
        void bind_state() const;

    private:
        struct texture_slot_data
        {
            u32 slot;
            u32 sampler_id;
        };

        id_generator<u32> slot_allocator;
        std::unordered_map<u32, texture_slot_data> slot_map;
    };
}
