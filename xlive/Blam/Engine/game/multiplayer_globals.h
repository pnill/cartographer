#pragma once
#include "game/game_engine.h"

/* constants */

enum
{
	MULTIPLAYER_GLOBALS_DEFINITION_TAG = 'mulg',
	MULTIPLAYER_GLOBALS_DEFINITION_VERSION = 1,
};

enum
{
	k_maximum_status_response_count = 32
};

/* macros */

#define multiplayer_globals_definition_get(index)	((struct s_multiplayer_globals_definition*)tag_get(MULTIPLAYER_GLOBALS_DEFINITION_TAG, (index)))

/* enums */

enum e_game_engine_status_response_flags : uint16
{
	_game_engine_status_response_flag_unused = 0,
	k_game_engine_status_response_flag_count
};

enum e_game_engine_status_state : uint16
{
	_game_engine_status_state_waiting_for_space_to_clear = 0,
	_game_engine_status_state_observing = 1,
	_game_engine_status_state_respawning_soon = 2,
	_game_engine_status_state_sitting_out = 3,
	_game_engine_status_state_out_of_lives = 4,
	_game_engine_status_state_playing_winning = 5,
	_game_engine_status_state_playing_tied = 6,
	_game_engine_status_state_playing_losing = 7,
	_game_engine_status_state_game_over_won = 8,
	_game_engine_status_state_game_over_tied = 9,
	_game_engine_status_state_game_over_lost = 10,
	_game_engine_status_state_you_have_flag = 11,
	_game_engine_status_state_enemy_has_flag = 12,
	_game_engine_status_state_flag_not_home = 13,
	_game_engine_status_state_carrying_oddball = 14,
	_game_engine_status_state_you_are_juggernaut = 15,
	_game_engine_status_state_you_control_hill = 16,
	_game_engine_status_state_switching_sides_soon = 17,
	_game_engine_status_state_player_recently_started = 18,
	_game_engine_status_state_you_have_bomb = 19,
	_game_engine_status_state_flag_contested = 20,
	_game_engine_status_state_bomb_contested = 21,
	_game_engine_status_state_limited_lives_left_multiple = 22,
	_game_engine_status_state_limited_lives_left_single = 23,
	_game_engine_status_state_limited_lives_left_final = 24,
	_game_engine_status_state_playing_winning_unlimited = 25,
	_game_engine_status_state_playing_tied_unlimited = 26,
	_game_engine_status_state_playing_losing_unlimited = 27,
};

enum e_multiplayer_globals_dynamic_spawn_area_type
{
	_multiplayer_globals_dynamic_spawn_area_enemy,
	_multiplayer_globals_dynamic_spawn_area_friend,
	_multiplayer_globals_dynamic_spawn_area_enemy_vehicle,
	_multiplayer_globals_dynamic_spawn_area_friendly_vehicle,
	_multiplayer_globals_dynamic_spawn_area_empty_vehicle,
	_multiplayer_globals_dynamic_spawn_area_oddball_inclusion,
	_multiplayer_globals_dynamic_spawn_area_oddball_exclusion,
	_multiplayer_globals_dynamic_spawn_area_hill_inclusion,
	_multiplayer_globals_dynamic_spawn_area_hill_exclusion,
	_multiplayer_globals_dynamic_spawn_area_last_race_flag,
	_multiplayer_globals_dynamic_spawn_area_dead_ally,
	_multiplayer_globals_dynamic_spawn_area_controlled_territory,
	k_multiplayer_globals_dynamic_spawn_area_count
};

/* structs */

struct s_multiplayer_universal_globals_definition
{
	tag_reference random_player_names;
	tag_reference team_names;

	tag_block<real_rgb_color> team_colors;

	tag_reference multiplayer_text;
};
ASSERT_STRUCT_SIZE(s_multiplayer_universal_globals_definition, 32);

struct s_multiplayer_runtime_globals_dynamic_spawn_area
{
	real_bounds radius;
	real32 weight;
	int8 pad[16];
};
ASSERT_STRUCT_SIZE(s_multiplayer_runtime_globals_dynamic_spawn_area, 28);

struct s_multiplayer_constants
{
	real32 maximum_random_spawn_bias;
	real32 teleporter_recharge_time;

	real32 grenade_danger_weight;
	real_bounds grenade_danger_radius;
	real32 grenade_danger_lead_time;

	real32 vehicle_danger_min_speed_world_units_seconds;
	real32 vehicle_danger_weight;
	real32 vehicle_danger_radius;
	real32 vehicle_danger_lead_time_seconds;

	/// how nearby a player is to count a vehicle as 'occupied'
	real32 vehicle_nearby_player_distance;

	int8 pad_1[148];

	tag_reference hill_shader;

	int8 pad_2[16];

	real32 flag_reset_stop_distance;

	tag_reference bomb_explode_effect;
	tag_reference bomb_explode_damage_effect;
	tag_reference bomb_defuse_effect;

	string_id bomb_defusal_string;
	string_id blocked_teleporter_string;

	int8 pad_3[100];
};
ASSERT_STRUCT_SIZE(s_multiplayer_constants, 352);

// max: k_maximum_status_response_count
struct s_game_engine_status_response
{
	c_flags_no_init<e_game_engine_status_response_flags, uint16, k_game_engine_status_response_flag_count> flags;
	int16 pad_1;

	e_game_engine_status_state state;
	int16 pad_2;

	string_id free_for_all_message;
	string_id team_message;

	tag_reference unused;

	int32 pad_3;
};
ASSERT_STRUCT_SIZE(s_game_engine_status_response, 28);

struct s_multiplayer_runtime_globals_definition
{
	tag_reference flag;
	tag_reference ball;
	tag_reference unit;
	tag_reference flag_shader;
	tag_reference hill_shader;
	tag_reference head;
	tag_reference juggernaut_powerup;
	tag_reference da_bomb;

	tag_reference unused[5];

	// max: 20
	s_tag_block weapons;	// tag_reference
	// max: 20
	s_tag_block vehicles;	// tag_reference
	// max: 20
	s_tag_block grenades;	// tag_reference
	// max: 20
	s_tag_block powerups;	// tag_reference

	tag_reference in_game_text;

	// max: 60
	s_tag_block sounds;		// tag_reference

	s_tag_block events[k_multiplayer_event_response_game_type_count];	// s_multiplayer_event_response_definition

	tag_reference default_item_collection_1;
	tag_reference default_item_collection_2;

	uint32 default_frag_grenade_count;
	uint32 default_plasma_grenade_count;

	int8 pad_1[40];

	real32 dynamic_spawn_zone_upper_height;
	real32 dynamic_spawn_zone_lower_height;

	int8 pad_2[40];

	s_multiplayer_runtime_globals_dynamic_spawn_area dynamic_spawn_area[k_multiplayer_globals_dynamic_spawn_area_count];

	int8 pad_3[608];

	s_tag_block multiplayer_constants;	// s_multiplayer_constants
	s_tag_block state_responses;		// s_game_engine_status_response

	tag_reference scoreboard_hud_definition;
	tag_reference scoreboard_emblem_shader;
	tag_reference scoreboard_emblem_bitmap;
	tag_reference scoreboard_dead_emblem_shader;
	tag_reference scoreboard_dead_emblem_bitmap;
};
ASSERT_STRUCT_SIZE(s_multiplayer_runtime_globals_definition, 1384);

struct s_multiplayer_globals_definition
{
	s_tag_block universal;		// s_multiplayer_universal_globals_definition
	s_tag_block runtime;		// s_multiplayer_runtime_globals_definition
};
ASSERT_STRUCT_SIZE(s_multiplayer_globals_definition, 16);
