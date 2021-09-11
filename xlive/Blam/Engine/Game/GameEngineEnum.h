#pragma once

enum e_game_engine_index
{
	_engine_campaign = 0x0,
	_engine_ctf = 0x1,
	_engine_slayer = 0x2,
	_engine_oddball = 0x3,
	_engine_koth = 0x4,
	_engine_juggernaut = 0x7,
	_engine_territories = 0x8,
	_engine_assault = 0x9,
};
enum e_game_engine_flags
{
	_game_engine_teams_bit = 0x0,
	_game_engine_motion_sensor_bit = 0x1,
	_game_engine_always_invisible_bit = 0x2,
	_game_engine_round_switch_resets_map_bit = 0x3,
	_game_engine_tie_resolution_bit = 0x4,
	_game_engine_observers_bit = 0x5,
	_game_engine_changing_teams_bit = 0x6,
	_game_engine_friendly_fire_bit = 0x7,
	_game_engine_overshields_on_map_bit = 0x8,
	_game_engine_invisibility_on_map_bit = 0x9,
	_game_engine_grenades_on_map_bit = 0xA,
	_game_engine_starting_grenades_bit = 0xB,
	_game_engine_extra_damage_bit = 0xC,
	_game_engine_damage_resistant_bit = 0xD,
	_game_engine_force_even_teams_bit = 0xE,
};
enum e_game_engine_round_setting
{
	_game_engine_round_setting_1_round = 0x0,
	_game_engine_round_setting_2_rounds = 0x1,
	_game_engine_round_setting_4_rounds = 0x2,
	_game_engine_round_setting_6_rounds = 0x3,
	_game_engine_round_setting_first_to_2_rounds = 0x4,
	_game_engine_round_setting_first_to_3_rounds = 0x5,
	_game_engine_round_setting_first_to_4_rounds = 0x6,
};
enum e_game_engine_join_in_progress
{
	_game_engine_join_in_progress_on = 0x0,
	_game_engine_join_in_progress_uneven = 0x1,
	_game_engine_join_in_progress_off = 0x2,
};
enum e_game_engine_shield_setting
{
	_game_engine_shield_normal = 0x0,
	_game_engine_shield_off = 0x1,
	_game_engine_shield_supershield = 0x2,
};
enum e_game_engine_team_score
{
	_game_engine_team_score_sum = 0x0,
	_game_engine_team_score_min = 0x1,
	_game_engine_team_score_max = 0x2,
};
enum e_game_engine_team_respawn
{
	_game_engine_team_respawn_inheritance = 0x0,
	_game_engine_team_respawn_cycling = 0x1,
	_game_engine_team_respawn_standard = 0x2,
};