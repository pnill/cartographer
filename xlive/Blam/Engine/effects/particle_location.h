#pragma once

#include "particle_system.h"

struct c_particle_location
{
	uint16 datum_salt;
	bool parent_effect_has_bit_15_set;
	int8 gap_3;
	datum particle_emitter_index;
	int8 gap_8[4];
	datum next_particle_location;
	real_point3d position;
	real32 field_1C;
	int32 field_20;
	int8 gap_24[16];

	void frame_advance(c_particle_system* particle_system, c_particle_system_definition* particle_system_definition, real32 delta);

	void c_particle_location::adjust_emitter_particle_intial_position(
		c_particle_system* particle_system,
		real32 dt,
		const real_matrix4x3* matrix
	);
};
ASSERT_STRUCT_SIZE(c_particle_location, 0x34);

s_data_array* get_particle_location_table();