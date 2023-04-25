#pragma once
#include "Blam/Common/Common.h"

enum e_interpolation_type : byte
{
	interpolation_type_none = 0,
	interpolation_type_ease_in_ease_out = 1,
	interpolation_type_ease_in_ease_out_heavy = 2,
	interpolation_type_three = 3,
	interpolation_type_four = 4
};

enum e_interpolator_control_flags : byte
{
	interpolator_control_flag_0 = FLAG(0),
	interpolator_control_flag_finished = FLAG(1)
};

class c_interpolator_control
{
	char ticks_remaining;
	byte duration_ticks;
	e_interpolation_type interpolation_type;
	e_interpolator_control_flags flags;

public:
	c_interpolator_control();
	~c_interpolator_control() = default;

	bool enabled();
	bool finished();
	void disable();
	void set_interpolation_type(e_interpolation_type interpolation_type);
};
