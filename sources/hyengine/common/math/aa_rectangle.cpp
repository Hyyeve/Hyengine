#include "aa_rectangle.hpp"

namespace hyengine
{
    using namespace glm;

    aa_rectangle::aa_rectangle(const vec2 initial_position, const vec2 initial_size) : position(initial_position), extents(initial_size)
    {
        check_inverted();
    }

    aa_rectangle::aa_rectangle(vec4 left_right_top_bottom) : position({left_right_top_bottom.x, left_right_top_bottom.z}), extents({left_right_top_bottom.y - left_right_top_bottom.x, left_right_top_bottom.w - left_right_top_bottom.z})
    {
        check_inverted();
    }

    void aa_rectangle::set(vec4 left_right_top_bottom)
    {
        position = left_right_top_bottom.xz();
        extents = left_right_top_bottom.yw() - left_right_top_bottom.xz();
        check_inverted();
    }

    void aa_rectangle::expand_by(vec4 left_right_top_bottom)
    {
        position -= left_right_top_bottom.xz();
        extents += left_right_top_bottom.xz() + left_right_top_bottom.yw();
    }

    void aa_rectangle::shrink_by(vec4 left_right_top_bottom)
    {
        position += left_right_top_bottom.xz();
        extents -= left_right_top_bottom.xz() + left_right_top_bottom.yw();
    }

    void aa_rectangle::expand_to(const aa_rectangle& other)
    {
        extents = max(extents, other.extents);
    }

    void aa_rectangle::shrink_to(const aa_rectangle& other)
    {
        extents = min(extents, other.extents);
    }

    vec2 aa_rectangle::min_corner() const
    {
        return position;
    }

    vec2 aa_rectangle::max_corner() const
    {
        return position + extents;
    }

    f32 aa_rectangle::left() const
    {
        return position.x;
    }

    f32 aa_rectangle::right() const
    {
        return position.x + extents.x;
    }

    f32 aa_rectangle::up() const
    {
        return position.y;
    }

    f32 aa_rectangle::down() const
    {
        return position.y + extents.y;
    }

    aa_rectangle aa_rectangle::expanded_by(vec4 left_right_top_bottom) const
    {
        return aa_rectangle {
            vec2(position.x - left_right_top_bottom.x, position.y - left_right_top_bottom.z),
            vec2(extents + left_right_top_bottom.xz() + left_right_top_bottom.yw())
        };
    }

    aa_rectangle aa_rectangle::expanded_to(const aa_rectangle& other) const
    {
        return aa_rectangle {
            position,
            max(extents, other.extents)
        };
    }

    aa_rectangle aa_rectangle::shrunk_by(const vec4 left_right_top_bottom) const
    {
        return {
            vec2(position.x + left_right_top_bottom.x, position.y + left_right_top_bottom.z),
            vec2(extents.x - left_right_top_bottom.x - left_right_top_bottom.y, extents.y - left_right_top_bottom.z - left_right_top_bottom.w)
        };
    }

    aa_rectangle aa_rectangle::shrunk_to(const aa_rectangle& other) const
    {
        return {
            position,
            min(extents, other.extents)
        };
    }

    aa_rectangle aa_rectangle::union_with(const aa_rectangle& other) const
    {
        vec2 new_position = min(position, other.position);
        vec2 new_size = max(position + extents, other.position + other.extents) - new_position;
        return {new_position, new_size};
    }

    aa_rectangle aa_rectangle::intersection_with(const aa_rectangle& other) const
    {
        vec2 new_position = max(position, other.position);
        vec2 new_size = min(position + extents, other.position + other.extents) - new_position;
        return {new_position, new_size};
    }

    bool aa_rectangle::contains(const vec2 point) const
    {
        return point.x >= position.x && point.x <= position.x + extents.x && point.y >= position.y && point.y <= position.y + extents.y;
    }

    bool aa_rectangle::intersects(const aa_rectangle& other) const
    {
        return position.x < other.position.x + other.extents.x && position.x + extents.x > other.position.x && position.y < other.position.y + other.extents.y && position.y + extents.y > other.position.y;
    }

    bool aa_rectangle::is_inside(const aa_rectangle& other) const
    {
        return position.x >= other.position.x && position.x + extents.x <= other.position.x + other.extents.x && position.y >= other.position.y && position.y + extents.y <= other.position.y + other.extents.y;
    }

    bool aa_rectangle::is_smaller_than(const aa_rectangle& other) const
    {
        return extents.x < other.extents.x && extents.y < other.extents.y;
    }

    void aa_rectangle::check_inverted()
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
