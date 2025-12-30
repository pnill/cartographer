#include "stdafx.h"
#include "animation_interpolation.h"

/* public code */

c_interpolator_control::c_interpolator_control(void)
{
	this->disable();
	this->m_interpolation_type = _interpolation_type_none;
	return;
}

real32 c_interpolator_control::get_ratio(void)
{
	return INVOKE_TYPE(0x11205B, 0x103579, real32(__thiscall*)(c_interpolator_control*), this);
}

bool c_interpolator_control::enabled(void) const
{
	return this->m_duration_ticks != 0;
}

bool c_interpolator_control::finished(void) const
{
	return TEST_BIT(this->m_flags, _interpolator_control_finished_bit);
}

void c_interpolator_control::disable(void)
{
	this->m_ticks_remaining = 0;
	this->m_duration_ticks = 0;
	this->m_flags = _interpolator_control_flag_none;
	return;
}

void c_interpolator_control::set_interpolation_type(e_interpolation_type interpolation_type)
{
	this->m_interpolation_type = interpolation_type;
	return;
}

void c_interpolator_control::set_status(bool a2, real32 duration)
{
	INVOKE_TYPE(0x111F01, 0x10341F, void(__thiscall*)(c_interpolator_control*, bool, real32), this, a2, duration);
	return;
}
