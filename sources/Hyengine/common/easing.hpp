#pragma once

namespace hyengine
{
	float ease_power_in(const float value, const float power = 2);
	float ease_power_out(const float value, const float power = 2);
	float ease_power_both(const float value, const float power = 2);

	float ease_sine_in(const float value);
	float ease_sine_out(const float value);
	float ease_sine_both(const float value);

	float ease_root_in(const float value, const float power = 2);
	float ease_root_out(const float value, const float power = 2);
	float ease_root_both(const float value, const float power = 2);

	float ease_overshoot_in(const float value, const float overshoot = 2);
	float ease_overshoot_out(const float value, const float overshoot = 2);
	float ease_overshoot_both(const float value, const float overshoot = 2);

	float ease_spring_in(const float value, const float power = 3, const float frequency = 3);
	float ease_spring_out(const float value, const float power = 3, const float frequency = 3);
	float ease_spring_both(const float value, const float power = 3, const float frequency = 3);

	float ease_bounce_in(const float value, const float power = 6, const float frequency = 3);
	float ease_bounce_out(const float value, const float power = 6, const float frequency = 3);
	float ease_bounce_both(const float value, const float power = 6, const float frequency = 3);

	float ease_accelerate_in(const float value, const float power = 0.4);
	float ease_accelerate_out(const float value, const float power = 0.4);
	float ease_accelerate_both(const float value, const float power = 0.4);
}
