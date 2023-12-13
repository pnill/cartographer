#pragma once
#include "particle.h"
#include "particle_location.h"
#include "particle_system.h"


enum e_particle_state_flags : uint32 {
	_particle_update_flag_bit_0 = 1 << 0,
	_particle_update_flag_bit_1 = 1 << 1,
	_particle_update_flag_bit_2 = 1 << 2,
	_particle_update_flag_bit_3 = 1 << 3,
	_particle_update_flag_bit_4 = 1 << 4,
	_particle_update_flag_bit_5 = 1 << 5,
	_particle_update_flag_bit_6 = 1 << 6,
	_particle_update_flag_bit_7 = 1 << 7,
	_particle_update_flag_bit_8 = 1 << 8,
	_particle_update_flag_bit_9 = 1 << 9,
	_particle_update_flag_bit_10 = 1 << 10,
	_particle_update_flag_bit_11 = 1 << 11,
	_particle_update_flag_bit_12 = 1 << 12,
	_particle_update_flag_bit_13 = 1 << 13,
	_particle_update_flag_bit_14 = 1 << 14,
	_particle_update_flag_bit_15 = 1 << 15,
	_particle_update_flag_bit_16 = 1 << 16,
	_particle_update_flag_bit_17 = 1 << 17,
	_particle_update_flag_bit_18 = 1 << 18,
	_particle_update_flag_bit_19 = 1 << 19,
	_particle_update_flag_bit_20 = 1 << 20,
	_particle_update_flag_bit_21 = 1 << 21,
	_particle_update_flag_bit_22 = 1 << 22,
	_particle_update_flag_bit_23 = 1 << 23,
	_particle_update_flag_bit_24 = 1 << 24,
	_particle_update_flag_bit_25 = 1 << 25,
	_particle_update_flag_bit_26 = 1 << 26,
	_particle_update_flag_bit_27 = 1 << 27,
	_particle_update_flag_bit_28 = 1 << 28,
	_particle_update_flag_bit_29 = 1 << 29,
	_particle_update_flag_bit_30 = 1 << 30,
};

struct s_particle_state
{
	int8 data[68];
	e_particle_state_flags flags;
	c_particle_system* particle_system;
	c_particle_location* particle_location;
	c_particle* particle;
};

void __cdecl particle_state_update(uint32 flags, c_particle_system* particle_system, c_particle_location* particle_location, c_particle* a4, s_particle_state* particle_state);
