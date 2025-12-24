#pragma once
#include "../sized_numerics.hpp"
#include "../../library/glm.hpp"

namespace hyengine
{
    ///Simple struct representing an axis-aligned 2D rectangle and useful related operations. Origin is at (-X, -Y)
    struct aa_rectangle
    {
        glm::vec2 position, extents;

        aa_rectangle(glm::vec2 initial_position, glm::vec2 initial_size);
        explicit aa_rectangle(glm::vec4 left_right_top_bottom);

        ///Recompute the rectangle based on left/right/top/bottom values
        void set(glm::vec4 left_right_top_bottom);

        ///Offsets the sides of the rectangle outwards by the given amounts
        void expand_by(glm::vec4 left_right_top_bottom);

        ///Offsets the sides of the rectangle inwards by the given amounts
        void shrink_by(glm::vec4 left_right_top_bottom);

        ///Resizes the rectangle to at least the size of other
        void expand_to(const aa_rectangle& other);

        ///Resizes the rectangle to at most the size of other
        void shrink_to(const aa_rectangle& other);

        [[nodiscard]] glm::vec2 min_corner() const;
        [[nodiscard]] glm::vec2 max_corner() const;

        [[nodiscard]] f32 left() const;
        [[nodiscard]] f32 right() const;
        [[nodiscard]] f32 up() const;
        [[nodiscard]] glm::f32 down() const;


        ///Creates a copy of this rectangle and offsets the sides outwards by the given amounts
        [[nodiscard]] aa_rectangle expanded_by(glm::vec4 left_right_top_bottom) const;

        ///Creates a copy of this rectangle and resizes it to at least the size of other
        [[nodiscard]] aa_rectangle expanded_to(const aa_rectangle& other) const;

        ///Creates a copy of this rectangle and offsets the sides inwards by the given amounts
        [[nodiscard]] aa_rectangle shrunk_by(glm::vec4 left_right_top_bottom) const;

        ///Creates a copy of this rectangle and resizes it to at most the size of other
        [[nodiscard]] aa_rectangle shrunk_to(const aa_rectangle& other) const;

        ///Creates a new rectangle that encompasses both this and other
        [[nodiscard]] aa_rectangle union_with(const aa_rectangle& other) const;

        ///Creates a new rectangle that encompasses the overlap between this and other
        [[nodiscard]] aa_rectangle intersection_with(const aa_rectangle& other) const;

        [[nodiscard]] bool contains(const glm::vec2 point) const;
        [[nodiscard]] bool intersects(const aa_rectangle& other) const;
        [[nodiscard]] bool is_inside(const aa_rectangle& other) const;
        [[nodiscard]] bool is_smaller_than(const aa_rectangle& other) const;

        ///Updates the position and extents to fix any negative extents.
        void check_inverted();
    };
}
