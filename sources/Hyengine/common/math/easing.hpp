#pragma once
#include "../sized_numerics.hpp"

namespace hyengine
{
    f32 ease_power_in(const f32 value, const f32 power = 2);
    f32 ease_power_out(const f32 value, const f32 power = 2);
    f32 ease_power_both(const f32 value, const f32 power = 2);

    f32 ease_sine_in(const f32 value);
    f32 ease_sine_out(const f32 value);
    f32 ease_sine_both(const f32 value);

    f32 ease_root_in(const f32 value, const f32 power = 2);
    f32 ease_root_out(const f32 value, const f32 power = 2);
    f32 ease_root_both(const f32 value, const f32 power = 2);

    f32 ease_overshoot_in(const f32 value, const f32 overshoot = 2);
    f32 ease_overshoot_out(const f32 value, const f32 overshoot = 2);
    f32 ease_overshoot_both(const f32 value, const f32 overshoot = 2);

    f32 ease_spring_in(const f32 value, const f32 power = 3, const f32 frequency = 3);
    f32 ease_spring_out(const f32 value, const f32 power = 3, const f32 frequency = 3);
    f32 ease_spring_both(const f32 value, const f32 power = 3, const f32 frequency = 3);

    f32 ease_bounce_in(const f32 value, const f32 power = 6, const f32 frequency = 3);
    f32 ease_bounce_out(const f32 value, const f32 power = 6, const f32 frequency = 3);
    f32 ease_bounce_both(const f32 value, const f32 power = 6, const f32 frequency = 3);

    f32 ease_accelerate_in(const f32 value, const f32 power = 0.4);
    f32 ease_accelerate_out(const f32 value, const f32 power = 0.4);
    f32 ease_accelerate_both(const f32 value, const f32 power = 0.4);
}
