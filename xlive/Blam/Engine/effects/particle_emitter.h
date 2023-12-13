#pragma once
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/memory/data.h"

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
	void adjust_matrix_and_vector_to_effect_camera(bool use_effect_camera, real_matrix3x3* out_matrix, real_point3d* out_point) const;
};
CHECK_STRUCT_SIZE(c_particle_emitter, 0x4C);

s_data_array* get_particle_emitter_table();
