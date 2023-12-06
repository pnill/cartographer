#include "stdafx.h"
#include "animation_interpolation.h"

c_interpolator_control::c_interpolator_control()
{
	this->ticks_remaining = 0;
	this->duration_ticks = 0;
	this->interpolation_type = _interpolation_type_none;
	this->flags = _interpolator_control_flag_none;
}

real32 c_interpolator_control::get_ratio(void)
{
	typedef real32(__thiscall* get_ratio_t)(c_interpolator_control*);
	get_ratio_t get_ratio = Memory::GetAddress<get_ratio_t>(0x11205B, 0x103579);
	return get_ratio(this);
}

bool c_interpolator_control::enabled() const
{
	return this->duration_ticks != 0;
}

bool c_interpolator_control::finished() const
{
	return this->flags & _interpolator_control_finished;
}

void c_interpolator_control::disable()
{
	this->ticks_remaining = 0;
	this->duration_ticks = 0;
	this->flags = _interpolator_control_flag_none;
}

void c_interpolator_control::set_interpolation_type(e_interpolation_type interpolation_type)
{
	this->interpolation_type = interpolation_type;
}

void c_interpolator_control::set_status(bool a2, real32 duration)
{
	typedef void(__thiscall* set_status_t)(c_interpolator_control*, bool, real32);
	set_status_t set_status = Memory::GetAddress<set_status_t>(0x111F01, 0x10341F);
	set_status(this, a2, duration);
	return;
}
