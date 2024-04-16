#pragma once
#include "math/matrix_math.h"
#include "memory/data.h"

#include "particle_system.h"
#include "particle_location.h"

struct c_particle_emitter
{
	uint16 datum_salt;
	int16 m_particle_count;
	datum particle_index;
	datum m_next_emitter_index;
	real32 particles_to_emit;
	real_matrix3x3 matrix;
	real_point3d position;
	real_point3d previous_position;

	void adjust_matrix_and_vector_to_effect_camera(bool use_effect_camera, real_matrix3x3* out_matrix, real_point3d* out_point) const;

	void __stdcall pulse(
		c_particle_emitter* _this,
		real32 delta,
		c_particle_system* particle_system,
		c_particle_emitter_definition* emitter_definition,
		s_particle_state* particle_state,
		const real_matrix4x3* in_martix,
		real32 alpha
	);

	void spawn_particle(
		s_particle_state* particle_state,
		c_particle_system* particle_system,
		c_particle_emitter_definition* emitter_definition,
		real32 a5,
		real32 a6,
		real32 delta,
		real32 a7
	);

	void adjust_initial_particle_position(
		c_particle_system* particle_system,
		c_particle_emitter_definition* emitter_definition,
		s_particle_state* particle_state,
		const real_matrix4x3* matrix,
		real32 dt
	);

	void calc_matrix(
		c_particle_emitter_definition* definition,
		c_particle_system* particle_system,
		real32 scale,
		const real_matrix4x3* matrix
	);
};
ASSERT_STRUCT_SIZE(c_particle_emitter, 0x4C);

s_data_array* get_particle_emitter_table();
