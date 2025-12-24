#pragma once
#include "hyengine/library/glm.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    ///Simple struct representing a axis-aligned 3D box and useful related operations. Origin is at (-X, -Y, -Z)
    struct aa_box
    {
        glm::vec3 position, extents;

        aa_box(glm::vec3 initial_position, glm::vec3 initial_extents);

        ///Resize the box to at least the size of other
        void expand_to(const aa_box& other);

        ///Resize the box to at most the size of other
        void shrink_to(const aa_box& other);

        [[nodiscard]] glm::vec3 min_corner() const;
        [[nodiscard]] glm::vec3 max_corner() const;

        [[nodiscard]] f32 left() const;
        [[nodiscard]] f32 right() const;
        [[nodiscard]] f32 up() const;
        [[nodiscard]] f32 down() const;
        [[nodiscard]] f32 near() const;
        [[nodiscard]] f32 far() const;

        ///Create a copy of this box and resize it to at least the size of other
        [[nodiscard]] aa_box expanded_to(const aa_box& other) const;

        ///Create a copy of this box and resize it to at most the size of other
        [[nodiscard]] aa_box shrunk_to(const aa_box& other) const;

        ///Create a new box that encompasses both this box and other
        [[nodiscard]] aa_box union_with(const aa_box& other) const;

        ///Create a new box that encompasses the overlap between this box and other
        [[nodiscard]] aa_box intersection_with(const aa_box& other) const;

        [[nodiscard]] bool contains(const glm::vec3 point) const;
        [[nodiscard]] bool intersects(const aa_box& other) const;
        [[nodiscard]] bool is_inside(const aa_box& other) const;
        [[nodiscard]] bool is_smaller_than(const aa_box& other) const;

        ///Updates the position and extents to fix any negative extents.
        void check_inverted();
    };
}
