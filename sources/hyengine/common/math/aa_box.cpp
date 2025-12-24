#include "aa_box.hpp"

namespace hyengine
{
    aa_box::aa_box(const glm::vec3 initial_position, const glm::vec3 initial_extents) : position(initial_position), extents(initial_extents) {}

    void aa_box::expand_to(const aa_box& other)
    {
        extents = max(extents, other.extents);
    }

    void aa_box::shrink_to(const aa_box& other)
    {
        extents = min(extents, other.extents);
    }

    glm::vec3 aa_box::min_corner() const
    {
        return position;
    }

    glm::vec3 aa_box::max_corner() const
    {
        return position + extents;
    }

    f32 aa_box::left() const
    {
        return position.x;
    }

    f32 aa_box::right() const
    {
        return position.x + extents.x;
    }

    f32 aa_box::up() const
    {
        return position.y;
    }

    f32 aa_box::down() const
    {
        return position.y + extents.y;
    }

    f32 aa_box::near() const
    {
        return position.z;
    }

    f32 aa_box::far() const
    {
        return position.z + extents.z;
    }

    aa_box aa_box::expanded_to(const aa_box& other) const
    {
        return {
            position,
            max(extents, other.extents)
        };
    }

    aa_box aa_box::shrunk_to(const aa_box& other) const
    {
        return {
            position,
            min(extents, other.extents)
        };
    }

    aa_box aa_box::union_with(const aa_box& other) const
    {
        glm::vec3 new_position = min(position, other.position);
        glm::vec3 new_size = max(position + extents, other.position + other.extents) - new_position;
        return {new_position, new_size};
    }

    aa_box aa_box::intersection_with(const aa_box& other) const
    {
        glm::vec3 new_position = max(position, other.position);
        glm::vec3 new_size = min(position + extents, other.position + other.extents) - new_position;
        return {new_position, new_size};
    }

    bool aa_box::contains(const glm::vec3 point) const
    {
        return point.x >= position.x && point.x <= position.x + extents.x
               && point.y >= position.y && point.y <= position.y + extents.y
               && point.z >= position.z && point.z <= position.z + extents.z;
    }

    bool aa_box::intersects(const aa_box& other) const
    {
        return position.x < other.position.x + other.extents.x && position.x + extents.x > other.position.x
               && position.y < other.position.y + other.extents.y && position.y + extents.y > other.position.y
               && position.z < other.position.z + other.extents.z && position.z + extents.z > other.position.z;
    }

    bool aa_box::is_inside(const aa_box& other) const
    {
        return position.x >= other.position.x && position.x + extents.x <= other.position.x + other.extents.x
               && position.y >= other.position.y && position.y + extents.y <= other.position.y + other.extents.y
               && position.z >= other.position.z && position.z + extents.z <= other.position.z + other.extents.z;
    }

    bool aa_box::is_smaller_than(const aa_box& other) const
    {
        return extents.x < other.extents.x && extents.y < other.extents.y && extents.z < other.extents.z;
    }

    void aa_box::check_inverted()
    {
        if (extents.x < 0)
        {
            extents.x = -extents.x;
            position.x += extents.x;
        }

        if (extents.y < 0)
        {
            extents.y = -extents.y;
            position.y += extents.y;
        }

        if (extents.z < 0)
        {
            extents.z = -extents.z;
            position.z += extents.z;
        }
    }
}
