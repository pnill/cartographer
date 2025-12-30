#pragma once

/* enums */

enum e_interpolation_type : uint8
{
	_interpolation_type_none = 0,
	_interpolation_type_ease_in_ease_out = 1,
	_interpolation_type_ease_in_ease_out_heavy = 2,
	_interpolation_type_three = 3,
	_interpolation_type_four = 4
};

enum e_interpolator_control_flags : uint8
{
	_interpolator_control_flag_none = 0,

	_interpolator_control_bit0 = 0,
	_interpolator_control_finished_bit = 1,

};

/* classes */

class c_interpolator_control
{
	int8 m_ticks_remaining;
	uint8 m_duration_ticks;
	e_interpolation_type m_interpolation_type;
	e_interpolator_control_flags m_flags;

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
ASSERT_STRUCT_SIZE(c_interpolator_control, 4);
