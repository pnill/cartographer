#pragma once
#include "main/game_preferences.h"
#include "math/color_math.h"
#include "memory/static_arrays.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

#define k_maximum_game_engine_event_responses_per_type 128
#define k_maximum_status_response_count 32

enum e_multiplayer_event_response_flags : uint16
{
	_multiplayer_event_response_quantity_message = 0,
	k_multiplayer_event_response_flags_count
};

enum e_multiplayer_event_response_game_type : uint16
{
	_multiplayer_event_response_game_type_general = 0,
	_multiplayer_event_response_game_type_flavor = 1,
	_multiplayer_event_response_game_type_slayer = 2,
	_multiplayer_event_response_game_type_capture_the_flag = 3,
	_multiplayer_event_response_game_type_oddball = 4,
	_multiplayer_event_response_game_type_headhunter = 5,
	_multiplayer_event_response_game_type_king_of_the_hill = 6,
	_multiplayer_event_response_game_type_unused_7 = 7,
	_multiplayer_event_response_game_type_juggernaut = 8,
	_multiplayer_event_response_game_type_assault = 9,
	_multiplayer_event_response_game_type_unused_10 = 10,
	_multiplayer_event_response_game_type_unused_11 = 11,
	_multiplayer_event_response_game_type_unused_12 = 12,
	_multiplayer_event_response_game_type_unused_13 = 13
};

enum e_multiplayer_event_response_event : uint16
{
	// general events
	_multiplayer_event_response_general_kill = 0,
	_multiplayer_event_response_general_suicide = 1,
	_multiplayer_event_response_general_kill_teammate = 2,
	_multiplayer_event_response_general_victory = 3,
	_multiplayer_event_response_general_team_victory = 4,
	_multiplayer_event_response_general_unused1 = 5,
	_multiplayer_event_response_general_unused2 = 6,
	_multiplayer_event_response_general_one_minute_to_win = 7,
	_multiplayer_event_response_general_team_one_minute_to_win = 8,
	_multiplayer_event_response_general_thirty_seconds_to_win = 9,
	_multiplayer_event_response_general_team_thirty_seconds_to_win = 10,
	_multiplayer_event_response_general_player_quit = 11,
	_multiplayer_event_response_general_player_joined = 12,
	_multiplayer_event_response_general_killed_by_unknown = 13,
	_multiplayer_event_response_general_thirty_minutes_left = 14,
	_multiplayer_event_response_general_fifteen_minutes_left = 15,
	_multiplayer_event_response_general_five_minutes_left = 16,
	_multiplayer_event_response_general_one_minute_left = 17,
	_multiplayer_event_response_general_time_expired = 18,
	_multiplayer_event_response_general_game_over = 19,
	_multiplayer_event_response_general_respawn_tick = 20,
	_multiplayer_event_response_general_last_respawn_tick = 21,
	_multiplayer_event_response_general_teleporter_used = 22,
	_multiplayer_event_response_general_player_changed_team = 23,
	_multiplayer_event_response_general_player_rejoined = 24,
	_multiplayer_event_response_general_gained_lead = 25,
	_multiplayer_event_response_general_gained_team_lead = 26,
	_multiplayer_event_response_general_lost_lead = 27,
	_multiplayer_event_response_general_lost_team_lead = 28,
	_multiplayer_event_response_general_tied_leader = 29,
	_multiplayer_event_response_general_tied_team_leader = 30,
	_multiplayer_event_response_general_round_over = 31,
	_multiplayer_event_response_general_thirty_seconds_left = 32,
	_multiplayer_event_response_general_ten_seconds_left = 33,
	_multiplayer_event_response_general_kill_falling = 34,
	_multiplayer_event_response_general_kill_collision = 35,
	_multiplayer_event_response_general_kill_melee = 36,
	_multiplayer_event_response_general_sudden_death = 37,
	_multiplayer_event_response_general_player_booted_player = 38,
	_multiplayer_event_response_general_kill_flag_carrier = 39,
	_multiplayer_event_response_general_kill_bomb_carrier = 40,
	_multiplayer_event_response_general_kill_sticky_grenade = 41,
	_multiplayer_event_response_general_kill_sniper = 42,
	_multiplayer_event_response_general_kill_stealth_melee = 43,
	_multiplayer_event_response_general_boarded_vehicle = 44,
	_multiplayer_event_response_general_start_team_notification = 45,
	_multiplayer_event_response_general_telefrag = 46,
	_multiplayer_event_response_general_ten_seconds_to_win = 47,
	_multiplayer_event_response_general_team_ten_seconds_to_win = 48,

	// flavor
	_multiplayer_event_response_flavor_double_kill = 0,
	_multiplayer_event_response_flavor_triple_kill = 1,
	_multiplayer_event_response_flavor_killtacular = 2,
	_multiplayer_event_response_flavor_killing_spree = 3,
	_multiplayer_event_response_flavor_running_riot = 4,
	_multiplayer_event_response_flavor_well_placed_kill = 5,
	_multiplayer_event_response_flavor_broke_killing_spree = 6,
	_multiplayer_event_response_flavor_kill_frenzy = 7,
	_multiplayer_event_response_flavor_killtrocity = 8,
	_multiplayer_event_response_flavor_killimajaro = 9,
	_multiplayer_event_response_flavor_fifteen_in_a_row = 10,
	_multiplayer_event_response_flavor_twenty_in_a_row = 11,
	_multiplayer_event_response_flavor_twenty_five_in_a_row = 12,

	// slayer
	_multiplayer_event_response_slayer_game_start = 0,
	_multiplayer_event_response_slayer_new_target = 1,

	// ctf
	_multiplayer_event_response_ctf_game_start = 0,
	_multiplayer_event_response_ctf_flag_taken = 1,
	_multiplayer_event_response_ctf_flag_dropped = 2,
	_multiplayer_event_response_ctf_flag_returned_by_player = 3,
	_multiplayer_event_response_ctf_flag_returned_by_timeout = 4,
	_multiplayer_event_response_ctf_flag_captured = 5,
	_multiplayer_event_response_ctf_flag_newDefensive_team = 6,
	_multiplayer_event_response_ctf_flag_return_faliure = 7,
	_multiplayer_event_response_ctf_side_switch_tick = 8,
	_multiplayer_event_response_ctf_side_switch_final_tick = 9,
	_multiplayer_event_response_ctf_side_switch_thirty_seconds = 10,
	_multiplayer_event_response_ctf_side_switch_ten_seconds = 11,
	_multiplayer_event_response_ctf_flag_contested = 12,
	_multiplayer_event_response_ctf_flag_capture_faliure = 13,

	// oddball
	_multiplayer_event_response_oddball_game_start = 0,
	_multiplayer_event_response_oddball_ball_spawned = 1,
	_multiplayer_event_response_oddball_ball_picked_up = 2,
	_multiplayer_event_response_oddball_ball_dropped = 3,
	_multiplayer_event_response_oddball_ball_reset = 4,
	_multiplayer_event_response_oddball_ball_tick = 5,

	// headhunter
	_multiplayer_event_response_headhunter_game_start = 0,
	_multiplayer_event_response_headhunter_hill_controlled = 1,
	_multiplayer_event_response_headhunter_hill_contested = 2,
	_multiplayer_event_response_headhunter_hill_tick = 3,
	_multiplayer_event_response_headhunter_hill_move = 4,
	_multiplayer_event_response_headhunter_hill_controlled_team = 5,
	_multiplayer_event_response_headhunter_hill_contested_team = 6,
	_multiplayer_event_response_headhunter_head_grabbed = 7,

	// king
	_multiplayer_event_response_king_game_start = 0,
	_multiplayer_event_response_king_hill_controlled = 1,
	_multiplayer_event_response_king_hill_contested = 2,
	_multiplayer_event_response_king_hill_tick = 3,
	_multiplayer_event_response_king_hill_move = 4,
	_multiplayer_event_response_king_hill_controlled_team = 5,
	_multiplayer_event_response_king_hill_contested_team = 6,

	// juggernaut
	_multiplayer_event_response_juggernaut_game_start = 0,
	_multiplayer_event_response_juggernaut_new_juggernaut = 1,
	_multiplayer_event_response_juggernaut_juggernaut_killed = 2,

	// territories
	_multiplayer_event_response_territories_game_start = 0,
	_multiplayer_event_response_territories_territory_control_gained = 1,
	_multiplayer_event_response_territories_territory_contest_lost = 2,
	_multiplayer_event_response_territories_all_territories_controlled = 3,
	_multiplayer_event_response_territories_team_territory_ctrl_gained = 4,
	_multiplayer_event_response_territories_team_territory_ctrl_lost = 5,
	_multiplayer_event_response_territories_team_all_territories_cntrld = 6,

	// assault
	_multiplayer_event_response_assault_game_start = 0,
	_multiplayer_event_response_assault_bomb_taken = 1,
	_multiplayer_event_response_assault_bomb_dropped = 2,
	_multiplayer_event_response_assault_bomb_returned_by_player = 3,
	_multiplayer_event_response_assault_bomb_returned_by_timeout = 4,
	_multiplayer_event_response_assault_bomb_captured = 5,
	_multiplayer_event_response_assault_bomb_new_defensive_team = 6,
	_multiplayer_event_response_assault_bomb_return_faliure = 7,
	_multiplayer_event_response_assault_side_switch_tick = 8,
	_multiplayer_event_response_assault_side_switch_final_tick = 9,
	_multiplayer_event_response_assault_side_switch_thirty_seconds = 10,
	_multiplayer_event_response_assault_side_switch_ten_seconds = 11,
	_multiplayer_event_response_assault_bomb_returned_by_defusing = 12,
	_multiplayer_event_response_assault_bomb_placed_on_enemy_post = 13,
	_multiplayer_event_response_assault_bomb_arming_started = 14,
	_multiplayer_event_response_assault_bomb_arming_completed = 15,
	_multiplayer_event_response_assault_bomb_contested = 16,
};

enum e_multiplayer_event_response_audience : uint16
{
	_multiplayer_event_response_audience_cause_player = 0,
	_multiplayer_event_response_audience_cause_team = 1,
	_multiplayer_event_response_audience_effect_player = 2,
	_multiplayer_event_response_audience_effect_team = 3,
	_multiplayer_event_response_audience_all = 4
};

enum e_multiplayer_event_response_audience_filter : uint16
{
	_multiplayer_event_response_audience_filter_none = 0,
	_multiplayer_event_response_audience_filter_cause_player = 1,
	_multiplayer_event_response_audience_filter_cause_team = 2,
	_multiplayer_event_response_audience_filter_effect_player = 3,
	_multiplayer_event_response_audience_filter_effect_team = 4,
};

enum e_multiplayer_response_sound_flags : uint16
{
	_multiplayer_event_response_sound_announcer_sound = 0,
	k_multiplayer_event_response_sound_flags_count
};

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

struct s_multiplayer_universal_globals_definition
{
	tag_reference random_player_names;
	tag_reference team_names;

	tag_block<real_rgb_color> team_colors;

	tag_reference multiplayer_text;
};
ASSERT_STRUCT_SIZE(s_multiplayer_universal_globals_definition, 32);

struct s_multiplayer_event_response_sound
{
	c_flags_no_init<e_multiplayer_response_sound_flags, uint16, k_multiplayer_event_response_sound_flags_count> sound_flags;

	int16 pad_1;

	tag_reference sound[k_language_count];

	real32 probability;
};

// max: k_maximum_game_engine_event_responses_per_type
struct s_multiplayer_event_response_definition
{
	c_flags_no_init<e_multiplayer_event_response_flags, uint16, k_multiplayer_event_response_flags_count> flags;
	e_multiplayer_event_response_game_type type;
	e_multiplayer_event_response_event event;
	e_multiplayer_event_response_audience audience;

	int16 pad_1;

	string_id display_string;

	e_multiplayer_event_response_audience_filter required_field;
	e_multiplayer_event_response_audience_filter excluded_audience;

	string_id primary_string;
	uint32 primary_string_duration;

	string_id plural_display_string;

	int8 pad_2[28];

	// for announcer sound only
	real32 sound_delay;

	s_multiplayer_event_response_sound primary_sound;
	int8 pad_3[16];
	tag_block<s_multiplayer_event_response_sound> sound_permutations;
};
ASSERT_STRUCT_SIZE(s_multiplayer_event_response_definition, 168);

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
	tag_block<tag_reference> weapons;
	// max: 20
	tag_block<tag_reference> vehicles;
	// max: 20
	tag_block<tag_reference> grenades;
	// max: 20
	tag_block<tag_reference> powerups;

	tag_reference in_game_text;

	// max: 60
	tag_block<tag_reference> sounds;

	tag_block<s_multiplayer_event_response_definition> general_events;
	tag_block<s_multiplayer_event_response_definition> flavor_events;
	tag_block<s_multiplayer_event_response_definition> slayer_events;
	tag_block<s_multiplayer_event_response_definition> ctf_events;
	tag_block<s_multiplayer_event_response_definition> oddball_events;
	tag_block<s_multiplayer_event_response_definition> headhunter_events;
	tag_block<s_multiplayer_event_response_definition> king_events;
	tag_block<s_multiplayer_event_response_definition> unused_7_events;
	tag_block<s_multiplayer_event_response_definition> juggernaut_events;
	tag_block<s_multiplayer_event_response_definition> assault_events;
	tag_block<s_multiplayer_event_response_definition> territories_events;
	tag_block<s_multiplayer_event_response_definition> unused_10_events;
	tag_block<s_multiplayer_event_response_definition> unused_11_events;
	tag_block<s_multiplayer_event_response_definition> unused_12_events;
	tag_block<s_multiplayer_event_response_definition> unused_13_events;

	tag_reference default_item_collection_1;
	tag_reference default_item_collection_2;

	uint32 default_frag_grenade_count;
	uint32 default_plasma_grenade_count;

	int8 pad_1[40];

	real32 dynamic_spawn_zone_upper_height;
	real32 dynamic_spawn_zone_lower_height;

	int8 pad_2[40];

	s_multiplayer_runtime_globals_dynamic_spawn_area enemy;
	s_multiplayer_runtime_globals_dynamic_spawn_area friend_;
	s_multiplayer_runtime_globals_dynamic_spawn_area enemy_vehicle;
	s_multiplayer_runtime_globals_dynamic_spawn_area friendly_vehicle;
	s_multiplayer_runtime_globals_dynamic_spawn_area empty_vehicle;
	s_multiplayer_runtime_globals_dynamic_spawn_area oddball_inclusion;
	s_multiplayer_runtime_globals_dynamic_spawn_area oddball_exclusion;
	s_multiplayer_runtime_globals_dynamic_spawn_area hill_inclusion;
	s_multiplayer_runtime_globals_dynamic_spawn_area hill_exclusion;
	s_multiplayer_runtime_globals_dynamic_spawn_area last_race_flag;
	s_multiplayer_runtime_globals_dynamic_spawn_area dead_ally;
	s_multiplayer_runtime_globals_dynamic_spawn_area controlled_territory;

	int8 pad_3[608];

	tag_block<s_multiplayer_constants> multiplayer_constants;
	tag_block<s_game_engine_status_response> state_responses;

	tag_reference scoreboard_hud_definition;
	tag_reference scoreboard_emblem_shader;
	tag_reference scoreboard_emblem_bitmap;
	tag_reference scoreboard_dead_emblem_shader;
	tag_reference scoreboard_dead_emblem_bitmap;
};
ASSERT_STRUCT_SIZE(s_multiplayer_runtime_globals_definition, 1384);

struct s_multiplayer_globals_definition
{
	tag_block<s_multiplayer_universal_globals_definition> universal;
	tag_block<s_multiplayer_runtime_globals_definition> runtime;
};
ASSERT_STRUCT_SIZE(s_multiplayer_globals_definition, 16);

s_multiplayer_globals_definition* get_multiplayer_globals();