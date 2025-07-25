#include "texture_set.hpp"

#include "Hyengine/graphics/graphics.hpp"

namespace hyengine
{
    unsigned int texture_set::allocate_slot(const unsigned int texture_id, const unsigned int sampler_state_id)
    {
        ZoneScoped;
        if (!has_space()) return 0;
        if (slot_map.contains(texture_id)) return get_slot(texture_id);

        const unsigned int slot = slot_allocator.assign();
        texture_slot_data slot_data = {slot, sampler_state_id};
        slot_map.insert({texture_id, slot_data});

        return slot;
    }

    void texture_set::set_sampler_state(const unsigned int texture_id, const unsigned int sampler_state_id)
    {
        ZoneScoped;
        if (!slot_map.contains(texture_id)) return;
        slot_map[texture_id].sampler_id = sampler_state_id;
    }

    void texture_set::free_slot(const unsigned int texture_id)
    {
        ZoneScoped;
        if (!slot_map.contains(texture_id)) return;
        slot_allocator.free(get_slot(texture_id));
    }

    unsigned int texture_set::get_slot(const unsigned int texture_id) const
    {
        ZoneScoped;
        if (!slot_map.contains(texture_id)) return 0;
        return slot_map.at(texture_id).slot;
    }

    bool texture_set::has_space() const
    {
        ZoneScoped;
        return slot_map.size() < max_slots();
    }

    unsigned int texture_set::max_slots()
    {
        ZoneScoped;
        return get_max_texture_units();
    }

    void texture_set::clear()
    {
        ZoneScoped;
        slot_allocator.clear();
        slot_map.clear();
    }

    void texture_set::bind_state() const
    {
        ZoneScoped;
        for (auto [texture, slot_data] : slot_map)
        {
            glBindTextureUnit(slot_data.slot, texture);

            //Only overwrite sampler state if there is one specifically assigned
            if (slot_data.sampler_id != 0)
                glBindSampler(slot_data.slot, slot_data.sampler_id);
        }
    }
}
