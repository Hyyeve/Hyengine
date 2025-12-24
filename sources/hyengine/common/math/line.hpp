#pragma once
#include "../sized_numerics.hpp"
#include "../../library/glm.hpp"

namespace hyengine
{
    ///Simple struct representing a line between two points and useful related operations
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

        ///Evaluates the sdf (signed distance field) of the line at a point
        [[nodiscard]] f32 sdf(const glm::vec2 point) const;
        [[nodiscard]] f32 length() const;
        [[nodiscard]] f32 left() const;
        [[nodiscard]] f32 right() const;
        [[nodiscard]] f32 up() const;
        [[nodiscard]] f32 down() const;

        ///Vector from the start to the end
        [[nodiscard]] glm::vec2 vector() const;
        ///Normalized direction from the start to the end
        [[nodiscard]] glm::vec2 direction() const;
        ///'Forwards' normal pointing perpendicular to the line
        [[nodiscard]] glm::vec2 normal() const;
        ///Compute an intersection point with other. Will return intersections that do not fall on the lines.
        ///Parallel lines will return a point as close to the midpoint of both lines as possible.
        [[nodiscard]] glm::vec2 intersection_with(const line& other) const;
    };
}
