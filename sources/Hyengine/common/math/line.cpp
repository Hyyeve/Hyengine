#include "line.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine
{
    using namespace glm;

    #define OP(half, full) \
        line line::operator half(const line& other) const { \
            return { \
            start half other.start, \
            end half other.end \
            }; \
        } \
        line line::operator half(const vec2 other) const { \
            return { \
            start half other, \
            end half other \
            }; \
        } \
        line line::operator half(const f32 other) const { \
            return { \
            start half other, \
            end half other \
            }; \
        } \
        line& line::operator full(const line& other) { \
            start = start half other.start; \
            end = end half other.end; \
            return *this; \
        } \
        line& line::operator full(const vec2 other) { \
            start = start half other; \
            end = end half other; \
            return *this; \
        } \
        line& line::operator full(const f32 other) { \
            start = start half other; \
            end = end half other; \
            return *this; \
        }
    OP(+, +=)
    OP(-, -=)
    OP(*, *=)
    OP(/, /=)

    #undef OP

    vec2 line::point_at(const f32 percent) const
    {
        return mix(start, end, percent);
    }

    f32 distance_of(const vec2 poi32)
    {
        vec2 start;
        vec2 end;
        vec2 delta_line = end - start;
        vec2 delta_poi32 = poi32 - start;
        f32 h = dot(delta_poi32, delta_line) / dot(delta_line, delta_line);
        return length(delta_poi32 - h * delta_line);
    }

    //Note: 0 < interpolation_percent < 1 only guarantees that the poi32 is within the bounding box of the line segment.
    f32 line::percent_of(const vec2 poi32) const
    {
        vec2 delta = end - start;
        vec2 poi32_delta = poi32 - start;
        if (length2(poi32_delta) < 1e-7) return 0;
        return dot(poi32_delta, delta) / dot(delta, delta);
    }

    f32 line::sdf(const vec2 poi32) const
    {
        const vec2 poi32_start = poi32 - start;
        const vec2 start_end = end - start;
        f32 h = clamp(dot(poi32_start, start_end) / dot(start_end, start_end), 0.0f, 1.0f);
        return glm::length(poi32_start - start_end * h);
    }

    f32 line::length() const
    {
        return glm::length(end - start);
    }

    f32 line::left() const
    {
        return min(start.x, end.x);
    }

    f32 line::right() const
    {
        return max(start.x, end.x);
    }

    f32 line::up() const
    {
        return max(start.y, end.y);
    }

    f32 line::down() const
    {
        return min(start.y, end.y);
    }

    //Treats the lines as infinitely long - must be checked to see if it's on the line segments.
    vec2 line::intersection_with(const line& other) const
    {
        ZoneScoped;

        f32 delta_ax = start.x - end.x;
        f32 delta_ay = end.y - start.y;
        f32 delta_bx = other.start.x - other.end.x;
        f32 delta_by = other.end.y - other.start.y;

        f32 c1 = delta_ay * (start.x) + delta_ax * (start.y);
        f32 c2 = delta_by * (other.start.x) + delta_bx * (other.start.y);
        f32 det = delta_ay * delta_bx - delta_by * delta_ax;

        if (det == 0)
        {
            //Parallel lines - no real intersection, but for physics purposes we still want to return a reasonable value.
            //This is the midpoi32 between the two closest poi32s on the lines.
            vec2 greater_min = max(min(start, end), min(other.start, other.end));
            vec2 lower_max = min(max(start, end), max(other.start, other.end));
            return mix(greater_min, lower_max, 0.5);
        }

        return {(delta_bx * c1 - delta_ax * c2) / det, (delta_ay * c2 - delta_by * c1) / det};
    }

    vec2 line::direction() const
    {
        vec2 ex = vector();
        if (glm::length(ex) == 0) return vec2(0);
        return normalize(ex);
    }

    vec2 line::vector() const
    {
        return end - start;
    }

    vec2 line::normal() const
    {
        vec2 dir = direction();
        return {dir.y, -dir.x};
    }
}
