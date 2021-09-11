#pragma once

#include "Blam/Common/Common.h"
#include "Blam/Engine/Game/GameEngineEnum.h"

struct __declspec(align(4)) s_game_variant
{
	WORD variant_flag;
	char gap[1];
	BYTE description_index;
	wchar_t variant_name[16];
	char pad0[32];
	e_game_engine_index variant_game_engine_index;
	e_game_engine_flags game_engine_flags;
	e_game_engine_round_setting round_setting;
	int score_to_win_round;
	int round_time_limit;
	e_game_engine_join_in_progress join_in_progress_setting;
	char pad3[24];
	int max_players;
	int max_living_players;
	int lives_per_round;
	int respawn_time;
	int suicide_penalty;
	e_game_engine_shield_setting shield_setting;
	char pad4[24];
	e_game_engine_team_score team_score_setting;
	e_game_engine_team_respawn team_respawn_setting;
	int betrayal_penalty;
	char pad5[4];
	int maximum_allowable_teams;
	char pad1[20];
	char vehicle_respawn_setting;
	char primary_light_land_vehicle;
	char secondary_light_land_vehicle;
	char primary_heavy_land_vehicle;
	char primary_flying_vehicle;
	char secondary_heavy_land_vehicle;
	char primary_turret_vehicle;
	char secondary_turret_vehicle;
	char weapon_set;
	char weapon_respawn_setting;
	char starting_equipment_primary;
	char starting_equipment_secondary;
	char pad2[24];
	int flags_or_territory_count_or_territory_contest_time_or_juggy;
	int flag_reset_time_or_ball_count;
	int speed_with_flag_or_ball_hit_damage;
	int flag_hit_damage_speed_with_ball;
	int waypoint_to_home_flag_type;
	int game_type;
	int bomb_arming_time;
	int bomb_fuse_time;
	char pad6[32];
};
CHECK_STRUCT_SIZE(s_game_variant, 0x130);

struct __declspec(align(4)) s_game_engine_settings
{
	e_engine_type map_type;
	char simulation_protocol;
	char network_protocol_related;
	BYTE session_host_is_dedicated;
	__int16 tickrate;
	BYTE gap_C[4];
	BYTE gap_E[2];
	char rand_bytes[8];
	DWORD rand_number;
	BYTE field_1C;
	__declspec(align(4)) BYTE gap_20[96];
	DWORD field_80;
	DWORD field_84;
	wchar_t scenario_path[260];
	DWORD field_290;
	BYTE gap_294[4];
	BYTE field_298;
	WORD single_player_difficulty_level;
	BYTE multiple_players;
	char field_29D;
	BYTE gap_29E[2];
	s_game_variant game_variant;
	DWORD main_menu_context;
	DWORD field_3D4;
	DWORD field_3D8;
	BYTE gap_3DC[98];
	char field_43E;
	char field_43F[6];
	DWORD field_448;
	BYTE gap_44C[3388];
};
CHECK_STRUCT_SIZE(s_game_engine_settings, 0x1188);
