#pragma once
#include "library/glm.hpp"

template<typename number>
number lerp(number input_min, number input_max, number value) {
    return input_min + (input_max - input_min) * value;
}

template<typename num>
num inverse_lerp(num input_min, num input_max, num value) {
    return (input_min - value) / (input_max - value);
}

template<typename number>
number remap(number input_min, number input_max, number output_min, number output_max, number value) {
    return lerp(output_min, output_max, inverse_lerp(input_min, input_max, value));
}

inline float ease_inout_cubic(float value)
{
    return value < 0.5 ? 4 * value * value * value : 1 - glm::pow(-2 * value + 2, 3) / 2;
}