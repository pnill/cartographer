#pragma once

#define k_max_particles 1024
#include "particle_system.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/memory/data.h"

enum e_particle_flags : int16
{
	_particle_flag_bit_0 = 0x0,
	_particle_flag_bit_1 = 0x1,
	_particle_flag_bit_2 = 0x2,
	_particle_flag_bit_3 = 0x3,
	_particle_flag_spawned_bit = 0x4,
	_particle_flag_bit_5 = 0x5,
	_particle_flag_bit_6 = 0x6,
	_particle_flag_bit_7 = 0x7,
	_particle_flag_bit_8 = 0x8,
	_particle_flag_bit_9 = 0x9,
	_particle_flag_bit_10 = 0xA,
	_particle_flag_bit_11 = 0xB,
	_particle_flag_bit_12 = 0xC,
	_particle_flag_bit_13 = 0xD,
	_particle_flag_bit_14 = 0xE,
	_particle_flag_bit_15 = 0xF,
};

struct c_particle
{
	int8 gap_0[2];
	e_particle_flags flags;
	int32 field_4;
	real32 field_8;
	real32 field_C;
	int8 gap_10[12];
	real_point3d m_position;
	real_vector3d m_velocity;
	int8 gap_34[12];
};

s_data_array* get_particle_table();