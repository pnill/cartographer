#pragma once

class c_interpolator_control
{
	byte unk0;
	byte unk1;
	byte interpolation_type;
	byte unk3;

public:
	c_interpolator_control();
	~c_interpolator_control() = default;

	void disable();
	void set_interpolation_type(byte value);
};
