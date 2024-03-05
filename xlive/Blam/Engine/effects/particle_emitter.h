#pragma once
#include "particle_state.h"

#include "math/matrix_math.h"
#include "memory/data.h"

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

	void adjust_matrix_and_vector_to_effect_camera(
		bool use_effect_camera,
		real_matrix3x3* out_matrix,
		real_point3d* out_point) const;

	void update_children(
		s_particle_state* particle_state,
		c_particle_system* particle_system,
		c_particle_emitter_definition* emitter_definition,
		real32 a5,
		real32 a6,
		real32 delta,
		real32 a7);

	void update_particles(
		c_particle_system* particle_system,
		c_particle_location* particle_location,
		c_particle_system_definition* particle_system_definition,
		c_particle_emitter_definition* particle_emitter_definition,
		real32 delta_time,
		real_vector3d* particle_location_position);

	void destroy_particles();
	static void __stdcall calc_matrix(c_particle_emitter* _this,
		real32 delta,
		c_particle_system* particle_system,
		c_particle_emitter_definition* emitter_definition,
		s_particle_state* particle_state,
		real_matrix4x3* in_martix,
		real32 alpha
	);

};
CHECK_STRUCT_SIZE(c_particle_emitter, 0x4C);

s_data_array* get_particle_emitter_table(void);

void particle_emitter_apply_patches(void);