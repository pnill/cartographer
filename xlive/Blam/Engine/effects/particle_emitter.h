#pragma once
#include "Blam/Engine/math/matrix_math.h"

struct c_particle_emitter
{
	int8 gap_0[4];
	datum particle_index;
	datum parent_emitter_index;
	int8 gap_C[4];
	real_matrix3x3 matrix;
	real_point3d position;
	int8 gap_40[4];
	int8 gap_44[8];
};
CHECK_STRUCT_SIZE(c_particle_emitter, 0x4C);