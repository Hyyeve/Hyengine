#pragma once
#include "shader_batching.hpp"

#include <unordered_set>

namespace hyengine {

    static std::unordered_map<std::string, std::shared_ptr<shader>> shared_shader_lookup;

    std::shared_ptr<shader> create_shader_instance(const std::string_view& asset_id, const bool require_unique)
    {
        std::shared_ptr<shader> result = nullptr;
        if (require_unique)
        {
            return std::make_shared<shader>(asset_id);
        }

        const std::string id_copy = std::string(asset_id); //Want to hold actual strings in the lookup table to make sure they don't accidentally get dropped

        if (shared_shader_lookup.contains(id_copy))
        {
            result = shared_shader_lookup[id_copy];
        }
        else
        {
            result = std::make_shared<shader>(id_copy);
            shared_shader_lookup[id_copy] = result;
        }

        return result;
    }

    void release_shared_instances()
    {
        shared_shader_lookup.clear();
    }
}
