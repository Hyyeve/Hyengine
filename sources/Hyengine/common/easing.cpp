#include <cmath>
#include "easing.hpp"
#include <tracy/Tracy.hpp>

#include "math.hpp"


float hyengine::ease_power_in(const float value, const float power)
{
    ZoneScoped;
    return pow(value, power);
}

float hyengine::ease_power_out(const float value, const float power)
{
    ZoneScoped;
    return 1. - ease_power_in(1. - value, power);
}

float hyengine::ease_power_both(const float value, const float power)
{
    ZoneScoped;
    if (value < 0.5) return ease_power_in(value * 2, power) / 2;
    return ease_power_out(value * 2 - 1, power) * 0.5 + 0.5;
}

float hyengine::ease_sine_in(const float value)
{
    ZoneScoped;
    return sin(1.5707963 * value);
}

float hyengine::ease_sine_out(const float value)
{
    ZoneScoped;
    return 1. - ease_sine_in(1. - value);
}

float hyengine::ease_sine_both(const float value)
{
    ZoneScoped;
    if (value < 0.5) return ease_sine_in(value * 2) / 2;
    return ease_sine_out(value * 2 - 1) * 0.5 + 0.5;
}

float hyengine::ease_root_in(const float value, const float power)
{
    ZoneScoped;
    if (value > 0 && value < 1) return 1. - sqrt(1. - pow(value, power));
    return value;
}

float hyengine::ease_root_out(const float value, const float power)
{
    ZoneScoped;
    return 1. - ease_root_in(1. - value, power);
}

float hyengine::ease_root_both(const float value, const float power)
{
    ZoneScoped;
    if (value < 0.5) return ease_root_in(value * 2, power) / 2;
    return ease_root_out(value * 2 - 1, power) * 0.5 + 0.5;
}

float hyengine::ease_overshoot_out(const float value, const float overshoot)
{
    ZoneScoped;
    return 1 - ease_overshoot_in(1 - value, overshoot);
}

float hyengine::ease_overshoot_in(const float value, const float overshoot)
{
    ZoneScoped;
    return value * value * ((1 + overshoot) * value - overshoot);
}

float hyengine::ease_overshoot_both(const float value, const float overshoot)
{
    ZoneScoped;
    if (value < 0.5) return ease_overshoot_in(value * 2, overshoot) / 2;
    return ease_overshoot_out(value * 2 - 1, overshoot) * 0.5 + 0.5;
}

float hyengine::ease_spring_in(const float value, const float power, const float frequency)
{
    ZoneScoped;
    return pow(value, power) * sin(value * PI * (2 * frequency + 0.5));
}

float hyengine::ease_spring_out(const float value, const float power, const float frequency)
{
    ZoneScoped;
    return 1. - ease_spring_in(1. - value, power, frequency);
}

float hyengine::ease_spring_both(const float value, const float power, const float frequency)
{
    ZoneScoped;
    if (value < 0.5) return ease_spring_in(value * 2, power, frequency) / 2;
    return ease_spring_out(value * 2 - 1, power, frequency) * 0.5 + 0.5;
}

float hyengine::ease_bounce_in(const float value, const float power, const float frequency)
{
    ZoneScoped;
    return pow(2, power * (value - 1)) * abs(sin(value * PI * (frequency + 0.5)));
}

float hyengine::ease_bounce_out(const float value, const float power, const float frequency)
{
    ZoneScoped;
    return 1. - ease_bounce_in(1. - value, power, frequency);
}

float hyengine::ease_bounce_both(const float value, const float power, const float frequency)
{
    ZoneScoped;
    if (value < 0.5) return ease_bounce_in(value * 2, power, frequency) / 2;
    return ease_bounce_out(value * 2 - 1, power, frequency) * 0.5 + 0.5;
}

float hyengine::ease_accelerate_in(const float value, const float power)
{
    ZoneScoped;
    return value + pow(value, power) * (1 - value);
}

float hyengine::ease_accelerate_out(const float value, const float power)
{
    ZoneScoped;
    return 1. - ease_accelerate_in(1. - value, power);
}

float hyengine::ease_accelerate_both(const float value, const float power)
{
    ZoneScoped;
    if (value < 0.5) return ease_accelerate_in(value * 2, power) / 2;
    return ease_accelerate_out(value * 2 - 1, power) * 0.5 + 0.5;
}
