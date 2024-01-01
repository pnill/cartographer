#pragma once
#include "particle_system_definition.h"
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/objects/object_placement.h"

#define k_max_particle_systems 128

enum e_particle_system_flags : int16
{
	_particle_system_scale_with_sky_render_model_bit = 10,
};

struct s_particle_system_update_timings
{
	real32 some_delta_calc;
	real32 current_delta;
	datum particle_system_location_index;
};
CHECK_STRUCT_SIZE(s_particle_system_update_timings, 0xC);

class c_particle_system
{
public:
	int32 datum_salt;
	real32 accumulated_time;
	real32 duration;
	e_particle_system_flags flags;
	int16 event_particle_system_index;
	datum tag_index;
	int16 effect_event_index;
	int8 gap_16[2];
	datum parent_effect_index;
	s_location location;
	datum definition_location_index;
	int8 gap_28[8];
	datum particle_system_location_index;
	int8 gap_34[4];
	int32 field_38;
	int8 gap_3C[4];
	datum next_particle_system;
	datum datum_44;
	c_particle_system* parent_system;
	int32 first_particle_index;
	pixel32 color;

	bool static __stdcall update(c_particle_system* thisx, real32 delta_time);
	static void __stdcall update_location_time(c_particle_system* thisx, s_particle_system_update_timings* timings, real_matrix4x3* matrix, int unused);
	static void __cdecl destroy(datum particle_system_index);
};
CHECK_STRUCT_SIZE(c_particle_system, 0x54);

s_data_array* get_particle_system_table();

void __cdecl particle_system_remove_from_effects_cache(datum effect_index, datum particle_system_index);

void apply_particle_system_patches();