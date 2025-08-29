#pragma once
#include <array>

#include "../sized_numerics.hpp"
#include "../../library/glm.hpp"

namespace hyengine
{
    class frustum
    {
    public:
        void set_from_matrices(glm::mat4 projection, glm::mat4 view, glm::dvec3 position);
        void set_from_origin_and_window(glm::dvec3 origin, glm::vec3 forward, std::array<glm::dvec3, 4> window, f32 near, f32 far);
        [[nodiscard]] bool visible(glm::dvec3 sphere_pos, f32 sphere_radius) const;

        std::array<glm::vec3, 6> normals = {};
        std::array<f32, 6> distances = {};
        glm::dvec3 offset = {};
    };
}
