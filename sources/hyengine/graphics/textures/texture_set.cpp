#include "texture_set.hpp"

#include "hyengine/graphics/gl_enums.hpp"
#include "hyengine/graphics/graphics.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace hyengine
{
    u32 texture_set::allocate_slot(const u32 texture_id, const u32 sampler_state_id)
    {
        if (!has_space()) return 0;
        if (slot_map.contains(texture_id)) return get_slot(texture_id);

        const u32 slot = slot_allocator.assign();
        texture_slot_data slot_data = {slot, sampler_state_id};
        slot_map.insert({texture_id, slot_data});

        return slot;
    }

    void texture_set::set_sampler_state(const u32 texture_id, const u32 sampler_state_id)
    {
        if (!slot_map.contains(texture_id)) return;
        slot_map[texture_id].sampler_id = sampler_state_id;
    }

    void texture_set::free_slot(const u32 texture_id)
    {
        if (!slot_map.contains(texture_id)) return;
        slot_allocator.free(get_slot(texture_id));
    }

    u32 texture_set::get_slot(const u32 texture_id) const
    {
        if (!slot_map.contains(texture_id)) return 0;
        return slot_map.at(texture_id).slot;
    }

    bool texture_set::has_space() const
    {
        return slot_map.size() < max_slots();
    }

    u32 texture_set::max_slots()
    {
        return get_gl_const_i32(gl_i32_consts::MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    }

    void texture_set::clear()
    {
        slot_allocator.clear();
        slot_map.clear();
    }

    void texture_set::bind_state() const
    {
        ZoneScoped;
        TracyGpuZone("bind texture set");
        for (auto [texture, slot_data] : slot_map)
        {
            glBindTextureUnit(slot_data.slot, texture);

            //Only overwrite sampler state if there is one specifically assigned
            if (slot_data.sampler_id != 0)
            {
                glBindSampler(slot_data.slot, slot_data.sampler_id);
            }
        }
    }
}
