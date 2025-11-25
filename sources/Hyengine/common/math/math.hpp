#pragma once

#include <tracy/Tracy.hpp>
#include "../sized_numerics.hpp"
#include "../../library/glm.hpp"

namespace hyengine
{
    constexpr f64 PI = 3.1415926535897f;
    constexpr f64 TAU = 6.2831853071795f;

    //Linear interpolation
    template <typename number>
    number lerp(number start, number end, number value)
    {
        return start + (end - start) * value;
    }

    //Gives the percentage of a value in a range (aka value 5 in range 0-10 = 0.5)
    template <typename number>
    number inverse_lerp(number start, number end, number value)
    {
        return (start - value) / (end - value);
    }

    //Framerate-independent lerp smoothing
    template <typename number>
    number smooth_follow(number current, number target, number delta, number half_life)
    {
        return target + (current - target) * glm::exp2(-delta / half_life);
    }

    //Moves current towards target by delta. (Will not overshoot)
    template <typename number>
    number delta_follow(number current, number target, number delta)
    {
        if (glm::abs(target - current) < delta) return target;
        return current > target ? current - delta : current + delta;
    }

    //Range remapping
    template <typename number>
    number remap_range(number start_in, number end_in, number start_out, number end_out, number value)
    {
        return lerp(start_out, end_out, inverse_lerp(start_in, end_in, value));
    }

    //Number with lower absolute value
    template <typename number>
    number abs_min(number value_a, number value_b)
    {
        return glm::abs(value_a) < glm::abs(value_b) ? value_a : value_b;
    }

    //Number with higher absolute value
    template <typename number>
    number abs_max(number value_a, number value_b)
    {
        return glm::abs(value_a) > glm::abs(value_b) ? value_a : value_b;
    }

    //floor() for positive, ceil() for negative
    template <typename number>
    number abs_floor(number value)
    {
        return value < 0 ? glm::ceil(value) : glm::floor(value);
    }

    //ceil() for positive values, floor() for negative values
    template <typename number>
    number abs_ceil(number value)
    {
        return value > 0 ? glm::ceil(value) : glm::floor(value);
    }

    //abs() of difference
    template <typename number>
    number abs_difference(number value_a, number value_b)
    {
        return glm::abs(value_a - value_b);
    }

    //Min if check > 0, otherwise max
    template <typename number>
    number min_max(number value_a, number value_b, number check)
    {
        return check > 0 ? glm::min(value_a, value_b) : glm::max(value_a, value_b);
    }

    //Min if check < 0, otherwise max
    template <typename number>
    number max_min(number value_a, number value_b, number check)
    {
        return check < 0 ? glm::min(value_a, value_b) : glm::max(value_a, value_b);
    }

    //Moves value towards 0 by amount. (Can overshoot)
    template <typename number>
    number value_reduce(number value, number amount)
    {
        return value < 0 ? value + amount : value - amount;
    }

    //Moves value away from 0 by amount. (Can overshoot)
    template <typename number>
    number value_expand(number value, number amount)
    {
        return value > 0 ? value + amount : value - amount;
    }

    //Clamps within -range to +range
    template <typename number>
    number value_constrain(number value, number range)
    {
        return glm::clamp(value, -range, +range);
    }

    //Snapped if abs(value) < abs(epsilon), else value
    template <typename number>
    number epsilon_snap(number value, number epsilon, number snapped)
    {
        return abs(value) < abs(epsilon) ? snapped : value;
    }

    //Clamps value inside the range as long as check is already inside the range
    template <typename number>
    number clamp_inside(number value, number check, number start, number end)
    {
        return check >= start && check <= end ? glm::clamp(value, start, end) : value;
    }

    //Clamps value to the outside of the range as long as check is already outside the range
    template <typename number>
    number clamp_outside(number value, number check, number start, number end)
    {
        if (check < start) return glm::min(value, start);
        if (check > end) return glm::max(value, end);
        return value;
    }

    //Quaternion out of euler rotations
    inline glm::quat make_euler_rotation_zxy(glm::vec3 euler, glm::vec3 y_axis = glm::vec3(0, 1, 0), glm::vec3 x_axis = glm::vec3(1, 0, 0))
    {
        ZoneScoped;
        return glm::angleAxis(euler.z, glm::cross(y_axis, x_axis)) * glm::angleAxis(euler.x, x_axis) * glm::angleAxis(euler.y, y_axis);
    }
}
