#include <cmath>
#include "easing.hpp"
#include "math.hpp"

namespace hyengine
{
    f32 ease_power_in(const f32 value, const f32 power)
    {
        return pow(value, power);
    }

    f32 ease_power_out(const f32 value, const f32 power)
    {
        return 1. - ease_power_in(1. - value, power);
    }

    f32 ease_power_both(const f32 value, const f32 power)
    {
        if (value < 0.5) return ease_power_in(value * 2, power) / 2;
        return ease_power_out(value * 2 - 1, power) * 0.5 + 0.5;
    }

    f32 ease_sine_in(const f32 value)
    {
        return sin(1.5707963 * value);
    }

    f32 ease_sine_out(const f32 value)
    {
        return 1. - ease_sine_in(1. - value);
    }

    f32 ease_sine_both(const f32 value)
    {
        if (value < 0.5) return ease_sine_in(value * 2) / 2;
        return ease_sine_out(value * 2 - 1) * 0.5 + 0.5;
    }

    f32 ease_root_in(const f32 value, const f32 power)
    {
        if (value > 0 && value < 1) return 1. - sqrt(1. - pow(value, power));
        return value;
    }

    f32 ease_root_out(const f32 value, const f32 power)
    {
        return 1. - ease_root_in(1. - value, power);
    }

    f32 ease_root_both(const f32 value, const f32 power)
    {
        if (value < 0.5) return ease_root_in(value * 2, power) / 2;
        return ease_root_out(value * 2 - 1, power) * 0.5 + 0.5;
    }

    f32 ease_overshoot_in(const f32 value, const f32 overshoot)
    {
        return value * value * ((1 + overshoot) * value - overshoot);
    }

    f32 ease_overshoot_out(const f32 value, const f32 overshoot)
    {
        return 1 - ease_overshoot_in(1 - value, overshoot);
    }

    f32 ease_overshoot_both(const f32 value, const f32 overshoot)
    {
        if (value < 0.5) return ease_overshoot_in(value * 2, overshoot) / 2;
        return ease_overshoot_out(value * 2 - 1, overshoot) * 0.5 + 0.5;
    }

    f32 ease_spring_in(const f32 value, const f32 power, const f32 frequency)
    {
        return pow(value, power) * sin(value * PI * (2 * frequency + 0.5));
    }

    f32 ease_spring_out(const f32 value, const f32 power, const f32 frequency)
    {
        return 1. - ease_spring_in(1. - value, power, frequency);
    }

    f32 ease_spring_both(const f32 value, const f32 power, const f32 frequency)
    {
        if (value < 0.5) return ease_spring_in(value * 2, power, frequency) / 2;
        return ease_spring_out(value * 2 - 1, power, frequency) * 0.5 + 0.5;
    }

    f32 ease_bounce_in(const f32 value, const f32 power, const f32 frequency)
    {
        return pow(2, power * (value - 1)) * abs(sin(value * PI * (frequency + 0.5)));
    }

    f32 ease_bounce_out(const f32 value, const f32 power, const f32 frequency)
    {
        return 1. - ease_bounce_in(1. - value, power, frequency);
    }

    f32 ease_bounce_both(const f32 value, const f32 power, const f32 frequency)
    {
        if (value < 0.5) return ease_bounce_in(value * 2, power, frequency) / 2;
        return ease_bounce_out(value * 2 - 1, power, frequency) * 0.5 + 0.5;
    }

    f32 ease_accelerate_in(const f32 value, const f32 power)
    {
        return value + pow(value, power) * (1 - value);
    }

    f32 ease_accelerate_out(const f32 value, const f32 power)
    {
        return 1. - ease_accelerate_in(1. - value, power);
    }

    f32 ease_accelerate_both(const f32 value, const f32 power)
    {
        if (value < 0.5) return ease_accelerate_in(value * 2, power) / 2;
        return ease_accelerate_out(value * 2 - 1, power) * 0.5 + 0.5;
    }
}
