#pragma once
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/memory/data.h"

struct c_particle_emitter
{
	bool active;
	int8 gap_1[3];
	datum particle_index;
	datum next_emitter_index;
	real32 emission_time;
	real_matrix3x3 matrix;
	real_point3d position;
	real_point3d previous_position;
};
CHECK_STRUCT_SIZE(c_particle_emitter, 0x4C);

s_data_array* get_particle_emitter_table();
