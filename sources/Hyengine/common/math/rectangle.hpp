#pragma once
#include "../sized_numerics.hpp"
#include "../../library/glm.hpp"

namespace hyengine
{
    class rectangle
    {
    public:
        glm::vec2 position, size;

        rectangle(glm::vec2 initial_position, glm::vec2 initial_size);
        explicit rectangle(glm::vec4 left_right_top_bottom);

        void set(glm::vec2 position, glm::vec2 size);
        void set(glm::vec4 left_right_top_bottom);
        void expand_by(glm::vec4 left_right_top_bottom);
        void shrink_by(glm::vec4 left_right_top_bottom);
        void expand_to(const rectangle& other);
        void shrink_to(const rectangle& other);

        [[nodiscard]] glm::vec2 min_corner() const;
        [[nodiscard]] glm::vec2 max_corner() const;

        [[nodiscard]] f32 left() const;
        [[nodiscard]] f32 right() const;
        [[nodiscard]] f32 up() const;
        [[nodiscard]] glm::f32 down() const;

        [[nodiscard]] rectangle expanded_by(glm::vec4 left_right_top_bottom) const;
        [[nodiscard]] rectangle expanded_to(const rectangle& other) const;
        [[nodiscard]] rectangle shrunk_by(glm::vec4 left_right_top_bottom) const;
        [[nodiscard]] rectangle shrunk_to(const rectangle& other) const;

        [[nodiscard]] rectangle union_with(const rectangle& other) const;
        [[nodiscard]] rectangle intersection_with(const rectangle& other) const;

        [[nodiscard]] bool contains(glm::vec2 poi32) const;
        [[nodiscard]] bool intersects(const rectangle& other) const;
        [[nodiscard]] bool is_inside(const rectangle& other) const;
        [[nodiscard]] bool is_smaller_than(const rectangle& other) const;

        void check_flipped();
    };
}
