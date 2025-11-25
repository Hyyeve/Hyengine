#pragma once
#include "../sized_numerics.hpp"
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
            line operator half(const f32 other) const; \
            line& operator full(const line& other); \
            line& operator full(const glm::vec2 other); \
            line& operator full(const f32 other);
        OP(+, +=);
        OP(-, -=);
        OP(*, *=);
        OP(/, /=);

        #undef OP

        [[nodiscard]] glm::vec2 point_at(const f32 percent) const;
        [[nodiscard]] f32 percent_of(const glm::vec2 point) const;
        [[nodiscard]] f32 sdf(const glm::vec2 point) const;
        [[nodiscard]] f32 length() const;
        [[nodiscard]] f32 left() const;
        [[nodiscard]] f32 right() const;
        [[nodiscard]] f32 up() const;
        [[nodiscard]] f32 down() const;

        [[nodiscard]] glm::vec2 vector() const;
        [[nodiscard]] glm::vec2 direction() const;
        [[nodiscard]] glm::vec2 normal() const;
        [[nodiscard]] glm::vec2 intersection_with(const line& other) const;
    };
}
