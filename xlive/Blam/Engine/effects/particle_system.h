#pragma once
#include "particle_system_definition.h"
#include "Blam/Engine/objects/object_placement.h"

class c_particle_system
{
	int32 field_0;
	int32 field_4;
	int8 gap_8[4];
	int16 flags;
	int16 event_particle_system_index;
	datum tag_index;
	int16 effect_event_index;
	int8 gap_16[2];
	int32 field_18;
	s_location location;
	int8 gap_24[12];
	datum particle_system_location_index;
	int8 gap_34[4];
	int32 field_38;
	int8 gap_3C[4];
	int32 field_40;
	int8 gap_44[4];
	c_particle_system* parent_system;
	int32 field_4C;
	pixel32 color;

	c_particle_system_definition* get_particle_system_definition();
};
CHECK_STRUCT_SIZE(c_particle_system, 0x54);

