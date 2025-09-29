#pragma once
#include "event_handling.h"

/* enums */

enum e_ai_sound_volume : uint16
{
	ai_sound_volume_silent = 0,
	ai_sound_volume_medium = 1,
	ai_sound_volume_loud = 2,
	ai_sound_volume_shout = 3,
	ai_sound_volume_quiet = 4
};

enum e_ai_seat_type : uint16
{
	_ai_seat_type_none = 0,
	_ai_seat_type_passenger = 1,
	_ai_seat_type_gunner = 2,
	_ai_seat_type_small_cargo = 3,
	_ai_seat_type_large_cargo = 4,
	_ai_seat_type_driver = 5
};

/* structures */

struct ai_globals_type
{
	bool ai_active;
	bool ai_initialized_for_map;
	bool ai_has_control_data;
	bool selected_firing_position_this_tick;
	bool built_path_this_tick;
	bool fast_and_dumb_ai;
	int16 last_highest_pathfinding_service_timer;
	int16 current_highest_pathfinding_service_timer;
	bool pathfinding_time_given_this_frame;
	int16 last_highest_perception_service_timer;
	int16 current_highest_perception_service_timer;
	bool perception_time_given_this_frame;
	int16 current_objective_index;
	int32 first_squadless_actor_index;
	int32 highest_clump_refresh_weight;
	real32 major_upgrade_error;
	bool dialogue_enabled;
	int16 dialogue_suppression_ticks;
	int32 last_chatter_time[2];
	int32 last_talk_time[2];
	int32 last_shout_time[2];
	int16 spatial_effects_first_index;
	int16 spatial_effects_last_index;
	ai_sound_data spatial_effects[32];
	bool grenades_enabled;
	int32 infection_allowed_time;
	int16 mounted_weapon_unit_count;
	int32 mounted_weapon_unit_indices[8];
	int32 last_squad_placed;
	int16 perception_refresh_last_index;
	int16 active_ai_count;
};
ASSERT_STRUCT_SIZE(ai_globals_type, 884);

/* prototypes */

void ai_apply_patches(void);

void __cdecl ai_update(void);

void __cdecl ai_initialize_for_new_map(void);

