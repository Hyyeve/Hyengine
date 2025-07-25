#pragma once
#include "../../library/glm.hpp"

namespace hyengine
{
    class line
    {
    public:
        glm::vec2 start, end;

        #define OP(half, full) \
            line operator half(const line& other) const; \
            line operator half(const glm::vec2 other) const; \
            line operator half(const float other) const; \
            line& operator full(const line& other); \
            line& operator full(const glm::vec2 other); \
            line& operator full(const float other);
        OP(+, +=);
        OP(-, -=);
        OP(*, *=);
        OP(/, /=);

        #undef OP

        [[nodiscard]] glm::vec2 point_at(float percent) const;
        [[nodiscard]] float percent_of(glm::vec2 point) const;
        [[nodiscard]] float sdf(glm::vec2 point) const;
        [[nodiscard]] float length() const;
        [[nodiscard]] float left() const;
        [[nodiscard]] float right() const;
        [[nodiscard]] float up() const;
        [[nodiscard]] float down() const;

        [[nodiscard]] glm::vec2 vector() const;
        [[nodiscard]] glm::vec2 direction() const;
        [[nodiscard]] glm::vec2 normal() const;
        [[nodiscard]] glm::vec2 intersection_with(const line& other) const;
    };
}
