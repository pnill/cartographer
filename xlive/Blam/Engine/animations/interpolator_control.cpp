#include "stdafx.h"
#include "interpolator_control.h"

c_interpolator_control::c_interpolator_control()
{
	this->ticks_remaining = 0;
	this->duration_ticks = 0;
	this->interpolation_type = interpolation_type_none;
	this->flags = interpolator_control_flag_none;
}

bool c_interpolator_control::enabled()
{
	return this->duration_ticks != 0;
}

bool c_interpolator_control::finished()
{
	return this->flags & interpolator_control_finished;
}

void c_interpolator_control::disable()
{
	this->ticks_remaining = 0;
	this->duration_ticks = 0;
	this->flags = interpolator_control_flag_none;
}

void c_interpolator_control::set_interpolation_type(e_interpolation_type interpolation_type)
{
	this->interpolation_type = interpolation_type;
}