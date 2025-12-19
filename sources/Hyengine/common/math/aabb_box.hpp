#pragma once
#include "hyengine/library/glm.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    struct aabb_box
    {
        glm::vec3 position, extents;

        aabb_box(glm::vec3 initial_position, glm::vec3 initial_extents);
        void expand_to(const aabb_box& other);
        void shrink_to(const aabb_box& other);

        [[nodiscard]] glm::vec3 min_corner() const;
        [[nodiscard]] glm::vec3 max_corner() const;

        [[nodiscard]] f32 left() const;
        [[nodiscard]] f32 right() const;
        [[nodiscard]] f32 up() const;
        [[nodiscard]] f32 down() const;
        [[nodiscard]] f32 near() const;
        [[nodiscard]] f32 far() const;

        [[nodiscard]] aabb_box expanded_to(const aabb_box& other) const;
        [[nodiscard]] aabb_box shrunk_to(const aabb_box& other) const;

        [[nodiscard]] aabb_box union_with(const aabb_box& other) const;
        [[nodiscard]] aabb_box intersection_with(const aabb_box& other) const;

        [[nodiscard]] bool contains(const glm::vec3 point) const;
        [[nodiscard]] bool intersects(const aabb_box& other) const;
        [[nodiscard]] bool is_inside(const aabb_box& other) const;
        [[nodiscard]] bool is_smaller_than(const aabb_box& other) const;

        void check_inverted();
    };
}
