#pragma once

#include "particle_system_definition.h"
#include "math/matrix_math.h"
#include "memory/data.h"
#include "objects/object_placement.h"

#define k_max_particle_systems 128

enum e_particle_system_flags : int16
{
	_particle_system_updating_bit,
	_particle_system_bit_1,
	_particle_system_bit_2,
	_particle_system_bit_3,
	_particle_system_bit_4,
	_particle_system_bit_5,
	_particle_system_bit_6,
	_particle_system_bit_7,
	_particle_system_bit_8,
	_particle_system_bit_9,
	_particle_system_scale_with_sky_render_model_bit,
	_particle_system_bit_11,
	_particle_system_bit_12,
	_particle_system_bit_13,
	_particle_system_bit_14,
	k_particle_system_flags_count
};

struct s_particle_system_update
{
	real32 some_delta_calc;
	real32 delta_time;
	datum particle_system_location_index;
};
CHECK_STRUCT_SIZE(s_particle_system_update, 0xC);

class c_particle_system
{
public:
	int32 datum_salt;
	real32 update_dt;
	real32 duration_in_ticks;
	c_flags<e_particle_system_flags, uint16, k_particle_system_flags_count> flags;
	int16 event_particle_system_index;
	datum tag_index;
	int16 effect_event_index;
	int8 gap_16[2];
	datum parent_effect_index;
	s_location location;
	int32 definition_location_index;
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

	c_particle_system_definition* get_definition() const;

	bool get_ever_pulsed_or_frame_updated() const
	{
		return flags.test(_particle_system_bit_11) || flags.test(_particle_system_bit_9);
	}

	bool c_particle_system::get_in_sky(void) const
	{
		return this->flags.test(_particle_system_scale_with_sky_render_model_bit);
	}

	void __thiscall update_position(
		s_particle_system_update* particle_system_update,
		const real_matrix4x3* matrix,
		bool a4);

	void adjust_initial_position(datum particle_location_index, const real_matrix4x3* matrix);

	void change_parent_effect(datum* datum_1, datum* datum_2);
	void update_colors(bool v_mirrored_or_one_shot, bool one_shot, pixel32 color, pixel32 color_2);
	int32 get_particle_count() const;

	static bool __stdcall frame_advance(c_particle_system* thsix, real32 delta_time);

	void destroy_children();
	bool keep_system_alive(real32 delta_time);
	static void __stdcall restart(c_particle_system* thisx, s_particle_system_update* timings, real_matrix4x3* matrix, int unused);
	static void __cdecl destroy(datum particle_system_index);
};
CHECK_STRUCT_SIZE(c_particle_system, 0x54);

s_data_array* get_particle_system_table();

void __cdecl particle_system_remove_from_effects_cache(datum effect_index, datum particle_system_index);

void apply_particle_system_patches();