#pragma once

#include <unordered_map>
#include "../../common/id_generator.hpp"

namespace hyengine
{
    class texture_set
    {
    public:
        unsigned int allocate_slot(const unsigned int texture_id, const unsigned int sampler_state_id = 0);
        void set_sampler_state(const unsigned int texture_id, const unsigned int sampler_state_id);

        void free_slot(const unsigned int texture_id);

        [[nodiscard]] unsigned int get_slot(const unsigned int texture_id) const;
        [[nodiscard]] bool has_space() const;

        static unsigned int max_slots();

        void clear();
        void bind_state() const;

    private:
        struct texture_slot_data
        {
            unsigned int slot;
            unsigned int sampler_id;
        };

        id_generator<unsigned int> slot_allocator;
        std::unordered_map<unsigned int, texture_slot_data> slot_map;
    };
}
