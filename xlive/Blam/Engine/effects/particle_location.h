#pragma once
struct c_particle_location
{
	int8 gap_0[2];
	bool parent_effect_has_bit_15_set;
	int8 gap_3;
	datum particle_emitter_index;
	int8 gap_8[4];
	int32 particle_system_location_index_unused_maybe;
	real32 field_10;
	int32 field_14;
	int32 field_18;
	real32 field_1C;
	int8 gap_20[20];
};
CHECK_STRUCT_SIZE(c_particle_location, 0x34);