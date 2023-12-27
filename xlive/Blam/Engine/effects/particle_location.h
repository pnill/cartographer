#pragma once
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/memory/data.h"

struct c_particle_location
{
	int8 gap_0[2];
	bool parent_effect_has_bit_15_set;
	int8 gap_3;
	datum particle_emitter_index;
	int8 gap_8[4];
	int32 next_particle_location;
	real32 field_10;
	int32 field_14;
	real_point3d position;
	int8 gap_20[16];
};
CHECK_STRUCT_SIZE(c_particle_location, 0x34);

s_data_array* get_particle_location_table();