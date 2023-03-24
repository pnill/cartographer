#include "stdafx.h"
#include "interpolator_control.h"

c_interpolator_control::c_interpolator_control()
{
	this->unk0 = 0;
	this->unk1 = 0;
	this->interpolation_type = 0;
	this->unk3 = 0;
}

c_interpolator_control::~c_interpolator_control()
{

}

void c_interpolator_control::disable()
{
	this->unk0 = 0;
	this->unk1 = 0;
	this->unk3 = 0;
}

void c_interpolator_control::set_interpolation_type(byte value)
{
	this->interpolation_type = value;
}