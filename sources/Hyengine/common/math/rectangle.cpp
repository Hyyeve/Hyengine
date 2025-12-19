#include "rectangle.hpp"

namespace hyengine
{
    using namespace glm;

    rectangle::rectangle(const vec2 initial_position, const vec2 initial_size) : position(initial_position), extents(initial_size)
    {
        check_inverted();
    }

    rectangle::rectangle(vec4 left_right_top_bottom) : position({left_right_top_bottom.x, left_right_top_bottom.z}), extents({left_right_top_bottom.y - left_right_top_bottom.x, left_right_top_bottom.w - left_right_top_bottom.z})
    {
        check_inverted();
    }

    void rectangle::set(vec4 left_right_top_bottom)
    {
        position = left_right_top_bottom.xz();
        extents = left_right_top_bottom.yw() - left_right_top_bottom.xz();
        check_inverted();
    }

    void rectangle::expand_by(vec4 left_right_top_bottom)
    {
        position -= left_right_top_bottom.xz();
        extents += left_right_top_bottom.xz() + left_right_top_bottom.yw();
    }

    void rectangle::shrink_by(vec4 left_right_top_bottom)
    {
        position += left_right_top_bottom.xz();
        extents -= left_right_top_bottom.xz() + left_right_top_bottom.yw();
    }

    void rectangle::expand_to(const rectangle& other)
    {
        extents = max(extents, other.extents);
    }

    void rectangle::shrink_to(const rectangle& other)
    {
        extents = min(extents, other.extents);
    }

    vec2 rectangle::min_corner() const
    {
        return position;
    }

    vec2 rectangle::max_corner() const
    {
        return position + extents;
    }

    f32 rectangle::left() const
    {
        return position.x;
    }

    f32 rectangle::right() const
    {
        return position.x + extents.x;
    }

    f32 rectangle::up() const
    {
        return position.y;
    }

    f32 rectangle::down() const
    {
        return position.y + extents.y;
    }

    rectangle rectangle::expanded_by(vec4 left_right_top_bottom) const
    {
        return rectangle {
            vec2(position.x - left_right_top_bottom.x, position.y - left_right_top_bottom.z),
            vec2(extents + left_right_top_bottom.xz() + left_right_top_bottom.yw())
        };
    }

    rectangle rectangle::expanded_to(const rectangle& other) const
    {
        return rectangle {
            position,
            max(extents, other.extents)
        };
    }

    rectangle rectangle::shrunk_by(const vec4 left_right_top_bottom) const
    {
        return {
            vec2(position.x + left_right_top_bottom.x, position.y + left_right_top_bottom.z),
            vec2(extents.x - left_right_top_bottom.x - left_right_top_bottom.y, extents.y - left_right_top_bottom.z - left_right_top_bottom.w)
        };
    }

    rectangle rectangle::shrunk_to(const rectangle& other) const
    {
        return {
            position,
            min(extents, other.extents)
        };
    }

    rectangle rectangle::union_with(const rectangle& other) const
    {
        vec2 new_position = min(position, other.position);
        vec2 new_size = max(position + extents, other.position + other.extents) - new_position;
        return {new_position, new_size};
    }

    rectangle rectangle::intersection_with(const rectangle& other) const
    {
        vec2 new_position = max(position, other.position);
        vec2 new_size = min(position + extents, other.position + other.extents) - new_position;
        return {new_position, new_size};
    }

    bool rectangle::contains(const vec2 point) const
    {
        return point.x >= position.x && point.x <= position.x + extents.x && point.y >= position.y && point.y <= position.y + extents.y;
    }

    bool rectangle::intersects(const rectangle& other) const
    {
        return position.x < other.position.x + other.extents.x && position.x + extents.x > other.position.x && position.y < other.position.y + other.extents.y && position.y + extents.y > other.position.y;
    }

    bool rectangle::is_inside(const rectangle& other) const
    {
        return position.x >= other.position.x && position.x + extents.x <= other.position.x + other.extents.x && position.y >= other.position.y && position.y + extents.y <= other.position.y + other.extents.y;
    }

    bool rectangle::is_smaller_than(const rectangle& other) const
    {
        return extents.x < other.extents.x && extents.y < other.extents.y;
    }

    void rectangle::check_inverted()
    {
        if (extents.x < 0)
        {
            position.x += extents.x;
            extents.x = -extents.x;
        }
        if (extents.y < 0)
        {
            position.y += extents.y;
            extents.y = -extents.y;
        }
    }
}
