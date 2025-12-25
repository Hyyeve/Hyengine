#pragma once
#include "shader.hpp"

namespace hyengine
{
    std::shared_ptr<shader> create_shader_instance(const std::string_view& asset_id, bool require_unique);
    void release_shared_instances();
    void clear_all_shader_instance_caches();
}
