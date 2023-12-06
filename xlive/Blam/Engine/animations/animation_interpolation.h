#pragma once

enum e_interpolation_type : byte
{
	_interpolation_type_none = 0,
	_interpolation_type_ease_in_ease_out = 1,
	_interpolation_type_ease_in_ease_out_heavy = 2,
	_interpolation_type_three = 3,
	_interpolation_type_four = 4
};

enum e_interpolator_control_flags : byte
{
	_interpolator_control_flag_none = 0,
	_interpolator_control_flag_0 = FLAG(0),
	_interpolator_control_finished = FLAG(1)
};

class c_interpolator_control
{
	int8 ticks_remaining;
	uint8 duration_ticks;
	e_interpolation_type interpolation_type;
	e_interpolator_control_flags flags;

public:
	c_interpolator_control(void);
	~c_interpolator_control(void) = default;

	real32 get_ratio(void);
	bool enabled(void) const;
	bool finished(void) const;
	void disable(void);
	void set_interpolation_type(e_interpolation_type interpolation_type);
	void set_status(bool a2, real32 duration);
};
CHECK_STRUCT_SIZE(c_interpolator_control, 4);