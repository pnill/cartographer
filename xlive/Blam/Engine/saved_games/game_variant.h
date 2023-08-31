#pragma once
#include "Blam/Common/Common.h"

enum e_game_engine_index
{
	_game_engine_campaign = 0,
	_game_engine_ctf = 1,
	_game_engine_slayer = 2,
	_game_engine_oddball = 3,
	_game_engine_koth = 4,
	_game_engine_juggernaut = 7,
	_game_engine_territories = 8,
	_game_engine_assault = 9,
};
enum e_game_variant_description_index : char
{
	_game_variant_description_slayer = 0,
	_game_variant_description_oddball = 1,
	_game_variant_description_juggernaut = 2,
	_game_variant_description_king = 3,
	_game_variant_description_ctf = 4,
	_game_variant_description_invasion = 5,
	_game_variant_description_territories = 6,
	k_variant_count
};
enum e_game_engine_flags
{
	_game_engine_teams_bit = 0,
	_game_engine_motion_sensor_bit = 1,
	_game_engine_always_invisible_bit = 2,
	_game_engine_round_switch_resets_map_bit = 3,
	_game_engine_tie_resolution_bit = 4,
	_game_engine_observers_bit = 5,
	_game_engine_changing_teams_bit = 6,
	_game_engine_friendly_fire_bit = 7,
	_game_engine_overshields_on_map_bit = 8,
	_game_engine_invisibility_on_map_bit = 9,
	_game_engine_grenades_on_map_bit = 10,
	_game_engine_starting_grenades_bit = 11,
	_game_engine_extra_damage_bit = 12,
	_game_engine_damage_resistant_bit = 13,
	_game_engine_force_even_teams_bit = 14,
};
enum e_game_engine_round_setting
{
	_game_engine_round_setting_1_round = 0,
	_game_engine_round_setting_2_rounds = 1,
	_game_engine_round_setting_4_rounds = 2,
	_game_engine_round_setting_6_rounds = 3,
	_game_engine_round_setting_first_to_2_rounds = 4,
	_game_engine_round_setting_first_to_3_rounds = 5,
	_game_engine_round_setting_first_to_4_rounds = 6,
};
enum e_game_engine_join_in_progress
{
	_game_engine_join_in_progress_on = 0,
	_game_engine_join_in_progress_uneven = 1,
	_game_engine_join_in_progress_off = 2,
};
enum e_game_engine_shield_setting
{
	_game_engine_shield_normal = 0,
	_game_engine_shield_off = 1,
	_game_engine_shield_supershield = 2,
};
enum e_game_engine_team_score
{
	_game_engine_team_score_sum = 0,
	_game_engine_team_score_min = 1,
	_game_engine_team_score_max = 2,
};
enum e_game_engine_team_respawn
{
	_game_engine_team_respawn_inheritance = 0,
	_game_engine_team_respawn_cycling = 1,
	_game_engine_team_respawn_standard = 2,
};
enum e_game_engine_respawn : char
{
	_game_engine_respawn_setting_on = 0,
	_game_engine_respawn_setting_off = 1,
	_game_engine_respawn_setting_half = 2,
	_game_engine_respawn_setting_double = 3,
};
enum e_game_engine_light_land_vehicle : char
{
	_game_engine_light_land_vehicle_default = 0,
	_game_engine_light_land_vehicle_warthog = 1,
	_game_engine_light_land_vehicle_gauss_warthog = 2,
	_game_engine_light_land_vehicle_ghost = 3,
	_game_engine_light_land_vehicle_mongoose = 4,
	_game_engine_light_land_vehicle_shadow = 5,
	_game_engine_light_land_vehicle_random = 6,
	_game_engine_light_land_vehicle_none = 7,
};
enum e_game_engine_heavy_land_vehicle : char
{
	_game_engine_heavy_land_vehicle_default = 0,
	_game_engine_heavy_land_vehicle_scorpion = 1,
	_game_engine_heavy_land_vehicle_wraith = 2,
	_game_engine_heavy_land_vehicle_random = 3,
	_game_engine_heavy_land_vehicle_none = 4,
};
enum e_game_engine_flying_vehicle : char
{
	_game_engine_flying_vehicle_default = 0,
	_game_engine_flying_vehicle_banshee = 1,
	_game_engine_flying_vehicle_falcon = 2,
	_game_engine_flying_vehicle_random = 3,
	_game_engine_flying_vehicle_none = 4,
};
enum e_game_engine_turret_vehicle : char
{
	_game_engine_turret_vehicle_default = 0,
	_game_engine_turret_vehicle_human_ap = 1,
	_game_engine_turret_vehicle_human_av = 2,
	_game_engine_turret_vehicle_covenant_ap = 3,
	_game_engine_turret_vehicle_covenant_av = 4,
	_game_engine_turret_vehicle_random = 5,
	_game_engine_turret_vehicle_none = 6,
};
enum e_game_engine_weapon_set : char
{
	_game_engine_weapon_set_default = 0,
	_game_engine_weapon_set_none = 1,
	_game_engine_weapon_set_rockets = 2,
	_game_engine_weapon_set_shotguns = 3,
	_game_engine_weapon_set_swords = 4,
	_game_engine_weapon_set_brute_shots = 5,
	_game_engine_weapon_set_halo_classic = 6,
	_game_engine_weapon_set_new_classic = 7,
	_game_engine_weapon_set_heavy_weapons = 8,
	_game_engine_weapon_set_all_duals = 9,
	_game_engine_weapon_set_no_duals = 10,
	_game_engine_weapon_set_rifles = 11,
	_game_engine_weapon_set_sniping = 12,
	_game_engine_weapon_set_no_sniping = 13,
	_game_engine_weapon_set_pistols = 14,
	_game_engine_weapon_set_plasma = 15,
	_game_engine_weapon_set_human = 16,
	_game_engine_weapon_set_covenant = 17,
	_game_engine_weapon_set_sentinel = 18,
	_game_engine_weapon_set_random = 19,
};
enum e_game_engine_starting_weapon : char
{
	_game_engine_starting_weapon_default = 0,
	_game_engine_starting_weapon_none = 1,
	_game_engine_starting_weapon_random = 2,
	_game_engine_starting_weapon_battle_rifle = 3,
	_game_engine_starting_weapon_magnum = 4,
	_game_engine_starting_weapon_smg = 5,
	_game_engine_starting_weapon_plasma_pistol = 6,
	_game_engine_starting_weapon_plasma_rifle = 7,
	_game_engine_starting_weapon_rocket_launcher = 8,
	_game_engine_starting_weapon_shotgun = 9,
	_game_engine_starting_weapon_sniper_rifle = 10,
	_game_engine_starting_weapon_brute_shot = 11,
	_game_engine_starting_weapon_needler = 12,
	_game_engine_starting_weapon_carbine = 13,
	_game_engine_starting_weapon_beam_rifle = 14,
	_game_engine_starting_weapon_disintegrator = 15,
	_game_engine_starting_weapon_fuel_rod_gun = 16,
	_game_engine_starting_weapon_plasma_sword = 17,
	_game_engine_starting_weapon_brute_plasma = 18,
};
enum e_game_engine_player_speed
{
	_game_engine_player_speed_slow = 0,
	_game_engine_player_speed_normal = 1,
	_game_engine_player_speed_fast = 2,
};
enum e_game_engine_weapon_hit
{
	_game_engine_multiplayer_weapon_hit_extra_damage = 0,
	_game_engine_multiplayer_weapon_hit_normal_damage = 1,
};
enum e_slayer_engine_flags
{
	_slayer_engine_bonus_points_bit = 0,
	_slayer_engine_suicide_point_loss_bit = 1,
	_slayer_engine_death_point_loss_bit = 2,
};
enum e_ctf_game_type
{
	_ctf_game_type_multi_flag = 0,
	_ctf_game_type_single_flag = 1,
	_ctf_game_type_neutral_flag = 2,
};
enum e_ctf_engine_home_flag_waypoint_flags
{
	_ctf_engine_home_flag_waypoint_neutral = 0,
	_ctf_engine_home_flag_waypoint_on = 1,
	_ctf_engine_home_flag_waypoint_away_from_home = 2,
	_ctf_engine_home_flag_waypoint_off = 3,
};
enum e_ctf_engine_enemy_bomb_waypoint_flags
{
	_ctf_engine_enemy_bomb_waypoint_always_on = 0,
	_ctf_engine_enemy_bomb_waypoint_uncontrolled = 1,
	_ctf_engine_enemy_bomb_waypoint_armed = 2,
	_ctf_engine_enemy_bomb_waypoint_off = 3,
};
enum e_ctf_engine_flags : __int8
{
	_ctf_engine_sudden_death_bit = 1,
	_ctf_engine_flag_can_be_returned_bit = 2,
	_ctf_engine_flag_at_home_to_score_bit = 3,
	_ctf_engine_carriers_can_drive_and_gun_bit = 4,
	_ctf_engine_sticky_arming_bit = 5,
	_ctf_engine_carriers_damage_resistant_bit = 6,
	_ctf_engine_carriers_invisible_bit = 7,
};
enum e_odball_engine_waypoint_flags
{
	_oddball_waypoint_type_always_on = 0,
	_oddball_waypoint_type_neutral = 1,
	_oddball_waypoint_type_team_control = 2,
	_oddball_waypoint_type_off = 3,
};
enum e_king_engine_flags
{
	_king_engine_uncontested_hill_to_score_bit = 0,
	_king_engine_team_multiplier_bit = 1,
	_king_engine_extra_damage_on_hill_bit = 2,
	_king_engine_damage_resistance_on_hill_bit = 3,
	_king_engine_invis_on_hill_bit = 4,
};
enum e_oddball_engine_flags
{
	_oddball_engine_carriers_can_drive_and_gun_bit = 0,
	_oddball_engine_carriers_always_invis_bit = 1,
	_oddball_engine_carriers_damage_resistance_bit = 2,
};
enum e_juggernaut_engine_flags
{
	_juggernaut_has_motion_sensor_bit = 0,
	_juggernaut_has_supershield_bit = 1,
	_juggernaut_has_active_camo_bit = 2,
	_juggernaut_betrayal_point_loss_bit = 3,
	_juggernaut_does_extra_damage_bit = 4,
	_juggernaut_has_infinite_ammo_bit = 5,
	_juggernaut_is_damage_resistant_bit = 6,
};

struct s_variant_description_map
{
	const char* name;
	e_game_variant_description_index index;
};

#pragma pack(push,1)
struct s_game_variant
{
	WORD variant_flag;
	PAD(1);
	e_game_variant_description_index description_index;
	wchar_t variant_name[16];
	PAD(32);
	e_game_engine_index variant_game_engine_index;
	int game_engine_flags;
	e_game_engine_round_setting round_setting;
	int score_to_win_round;
	int round_time_limit;
	e_game_engine_join_in_progress join_in_progress_setting;
	PAD(24);
	int max_players;
	int max_living_players;
	int lives_per_round;
	int respawn_time;
	int suicide_penalty;
	e_game_engine_shield_setting shield_setting;
	PAD(24);
	e_game_engine_team_score team_score_setting;
	e_game_engine_team_respawn team_respawn_setting;
	int betrayal_penalty;
	PAD(4);
	int maximum_allowable_teams;
	PAD(20);
	e_game_engine_respawn vehicle_respawn_setting;
	e_game_engine_light_land_vehicle primary_light_land_vehicle;
	e_game_engine_light_land_vehicle secondary_light_land_vehicle;
	e_game_engine_heavy_land_vehicle primary_heavy_land_vehicle;
	e_game_engine_flying_vehicle primary_flying_vehicle;
	e_game_engine_heavy_land_vehicle secondary_heavy_land_vehicle;
	e_game_engine_turret_vehicle primary_turret_vehicle;
	e_game_engine_turret_vehicle secondary_turret_vehicle;
	e_game_engine_weapon_set weapon_set;
	e_game_engine_respawn weapon_respawn_setting;
	e_game_engine_starting_weapon starting_equipment_primary;
	e_game_engine_starting_weapon starting_equipment_secondary;

	bool is_team_play()
	{
		return TEST_FLAG(this->game_engine_flags, _game_engine_teams_bit);
	}

	//the area below this seems like a big union
	//TODO : figure this out
	PAD(0x130 - 0xD8);
};
CHECK_STRUCT_SIZE(s_game_variant, 0x130);
#pragma pack(pop)

void game_variant_build_default(s_game_variant* variant, e_game_variant_description_index game_variant_type);
