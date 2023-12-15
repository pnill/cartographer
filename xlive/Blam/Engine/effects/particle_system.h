#pragma once
#include "particle_system_definition.h"
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/objects/object_placement.h"

enum e_particle_system_flags : int16
{
	_particle_system_scale_with_sky_render_model_bit = 10,
};

class c_particle_system
{
public:
	int32 field_0;
	int32 field_4;
	int8 gap_8[4];
	e_particle_system_flags flags;
	int16 event_particle_system_index;
	datum tag_index;
	int16 effect_event_index;
	int8 gap_16[2];
	datum parent_effect_index;
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

	c_particle_system_definition* get_particle_system_definition() const;
	bool update(real32 delta_time);
	static void __cdecl destroy(datum particle_system_index);
};
CHECK_STRUCT_SIZE(c_particle_system, 0x54);

s_data_array* get_particle_system_table();
void __cdecl particle_syste_remove_from_effects_cache(datum effect_index, datum particle_system_index);