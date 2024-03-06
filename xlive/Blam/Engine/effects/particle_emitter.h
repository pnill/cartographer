#pragma once
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/memory/data.h"

struct c_particle_emitter
{
	bool active;
	int8 gap_1[1];
	int16 m_particle_count;
	datum particle_index;
	datum m_next_emitter_index;
	real32 emission_time;
	real_matrix3x3 matrix;
	real_point3d position;
	real_point3d previous_position;

	void adjust_matrix_and_vector_to_effect_camera(bool use_effect_camera, real_matrix3x3* out_matrix, real_point3d* out_point) const;
};
CHECK_STRUCT_SIZE(c_particle_emitter, 0x4C);

s_data_array* get_particle_emitter_table();
