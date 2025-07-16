#pragma once

#include <tracy/Tracy.hpp>

#include "../../library/glm.hpp"

namespace hyengine::common::math
{
    template<typename number>
    number lerp(number input_min, number input_max, number value) {
        ZoneScoped;
        return input_min + (input_max - input_min) * value;
    }

    template<typename num>
    num inverse_lerp(num input_min, num input_max, num value) {
        ZoneScoped;
        return (input_min - value) / (input_max - value);
    }

    template<typename number>
    number remap(number input_min, number input_max, number output_min, number output_max, number value) {
        ZoneScoped;
        return lerp(output_min, output_max, inverse_lerp(input_min, input_max, value));
    }

    inline float ease_inout_cubic(const float value)
    {
        ZoneScoped;
        return value < 0.5 ? 4 * value * value * value : 1 - glm::pow(-2 * value + 2, 3) / 2;
    }

    inline glm::quat euler_rotation_zxy(glm::vec3 euler, glm::vec3 y_axis = glm::vec3(0, 1, 0), glm::vec3 x_axis = glm::vec3(1, 0, 0))
    {
        ZoneScoped;
        return glm::angleAxis(euler.z, glm::cross(y_axis, x_axis)) * glm::angleAxis(euler.x, x_axis) * glm::angleAxis(euler.y, y_axis);
    }
}
