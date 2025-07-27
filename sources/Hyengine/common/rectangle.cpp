#include "rectangle.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine
{
    using namespace glm;

    rectangle::rectangle(const vec2 position, const vec2 size) : position(position), size(size)
    {
        check_flipped();
    }

    rectangle::rectangle(vec4 left_right_top_bottom) : position({left_right_top_bottom.x, left_right_top_bottom.z}), size({left_right_top_bottom.y - left_right_top_bottom.x, left_right_top_bottom.w - left_right_top_bottom.z})
    {
        check_flipped();
    }

    void rectangle::set(const vec2 position, const vec2 size)
    {
        this->position = position;
        this->size = size;
        check_flipped();
    }

    void rectangle::set(vec4 left_right_top_bottom)
    {
        position = left_right_top_bottom.xz();
        size = left_right_top_bottom.yw() - left_right_top_bottom.xz();
        check_flipped();
    }

    void rectangle::expand_by(vec4 left_right_top_bottom)
    {
        position -= left_right_top_bottom.xz();
        size += left_right_top_bottom.xz() + left_right_top_bottom.yw();
    }

    void rectangle::shrink_by(vec4 left_right_top_bottom)
    {
        position += left_right_top_bottom.xz();
        size -= left_right_top_bottom.xz() + left_right_top_bottom.yw();
    }

    void rectangle::expand_to(const rectangle& other)
    {
        size = max(size, other.size);
    }

    void rectangle::shrink_to(const rectangle& other)
    {
        size = min(size, other.size);
    }

    vec2 rectangle::min_corner() const
    {
        return position;
    }

    vec2 rectangle::max_corner() const
    {
        return position + size;
    }

    f32 rectangle::left() const
    {
        return position.x;
    }

    f32 rectangle::right() const
    {
        return position.x + size.x;
    }

    f32 rectangle::up() const
    {
        return position.y;
    }

    f32 rectangle::down() const
    {
        return position.y + size.y;
    }

    rectangle rectangle::expanded_by(vec4 left_right_top_bottom) const
    {
        return rectangle{
            vec2(position.x - left_right_top_bottom.x, position.y - left_right_top_bottom.z),
            vec2(size + left_right_top_bottom.xz() + left_right_top_bottom.yw())
        };
    }

    rectangle rectangle::expanded_to(const rectangle& other) const
    {
        return rectangle{
            position,
            max(size, other.size)
        };
    }

    rectangle rectangle::shrunk_by(const vec4 left_right_top_bottom) const
    {
        return rectangle{
            vec2(position.x + left_right_top_bottom.x, position.y + left_right_top_bottom.z),
            vec2(size.x - left_right_top_bottom.x - left_right_top_bottom.y, size.y - left_right_top_bottom.z - left_right_top_bottom.w)
        };
    }

    rectangle rectangle::shrunk_to(const rectangle& other) const
    {
        return rectangle{
            position,
            min(size, other.size)
        };
    }

    rectangle rectangle::union_with(const rectangle& other) const
    {
        vec2 new_position = min(position, other.position);
        vec2 new_size = max(position + size, other.position + other.size) - new_position;
        return {new_position, new_size};
    }

    rectangle rectangle::intersection_with(const rectangle& other) const
    {
        vec2 new_position = max(position, other.position);
        vec2 new_size = min(position + size, other.position + other.size) - new_position;
        return {new_position, new_size};
    }

    bool rectangle::is_inside(const rectangle& other) const
    {
        return position.x >= other.position.x && position.x + size.x <= other.position.x + other.size.x && position.y >= other.position.y && position.y + size.y <= other.position.y + other.size.y;
    }

    bool rectangle::is_smaller_than(const rectangle& other) const
    {
        return size.x < other.size.x && size.y < other.size.y;
    }

    bool rectangle::intersects(const rectangle& other) const
    {
        return position.x < other.position.x + other.size.x && position.x + size.x > other.position.x && position.y < other.position.y + other.size.y && position.y + size.y > other.position.y;
    }

    bool rectangle::contains(const vec2 poi32) const
    {
        return poi32.x >= position.x && poi32.x <= position.x + size.x && poi32.y >= position.y && poi32.y <= position.y + size.y;
    }

    void rectangle::check_flipped()
    {
        if (size.x < 0)
        {
            position.x += size.x;
            size.x = -size.x;
        }
        if (size.y < 0)
        {
            position.y += size.y;
            size.y = -size.y;
        }
    }
}
