#pragma once

/* constants */

enum
{
	k_game_engine_oddball_maximum_balls = 3,
	k_multiplayer_variant_headhunter_parameter_count = 7
};

/* enums */

enum e_game_engine_type : int32
{
	_game_engine_type_none = 0,
	_game_engine_type_ctf,
	_game_engine_type_slayer,
	_game_engine_type_oddball,
	_game_engine_type_koth,
	_game_engine_type_race,
	_game_engine_type_headhunter,
	_game_engine_type_juggernaut,
	_game_engine_type_territories,
	_game_engine_type_assault,
	_game_engine_type_stub,

	k_game_engine_type_count,
	k_game_engine_playable_types = 10,
	k_game_engine_type_bits_required = bits_required_for(k_game_engine_type_count - 1)
};

enum e_game_variant_description_index : int8
{
	_game_variant_description_slayer = 0,
	_game_variant_description_oddball,
	_game_variant_description_juggernaut,
	_game_variant_description_king,
	_game_variant_description_ctf,
	_game_variant_description_invasion,
	_game_variant_description_territories,

	k_base_game_variant_description_count,

	// cartographer added variant descriptions
	_game_variant_description_zombies = k_base_game_variant_description_count,
	_game_variant_description_headhunter,

	k_game_variant_description_count,
	k_game_variant_description_invalid = NONE,

	// todo: list all the mission game variant description indexes (over 100ish missing)
	k_game_variant_description_bits_required = 7
	//k_game_variant_description_bits_required = bits_required_for(k_game_variant_description_count - 1)
};

enum e_game_engine_flags : int32
{
	_game_engine_teams_bit = 0,
	_game_engine_motion_sensor_bit,
	_game_engine_always_invisible_bit,
	_game_engine_round_switch_resets_map_bit,
	_game_engine_tie_resolution_bit,
	_game_engine_observers_bit,
	_game_engine_changing_teams_bit,
	_game_engine_friendly_fire_bit,
	_game_engine_overshields_on_map_bit,
	_game_engine_invisibility_on_map_bit,
	_game_engine_grenades_on_map_bit,
	_game_engine_starting_grenades_bit,
	_game_engine_extra_damage_bit,
	_game_engine_damage_resistant_bit,
	_game_engine_force_even_teams_bit,

	k_game_engine_flags_count,
	k_game_engine_flags_clear_unused_bits = FLAG(_game_engine_force_even_teams_bit | _game_engine_damage_resistant_bit | _game_engine_extra_damage_bit | _game_engine_starting_grenades_bit | _game_engine_grenades_on_map_bit | _game_engine_invisibility_on_map_bit | _game_engine_overshields_on_map_bit | _game_engine_friendly_fire_bit | _game_engine_changing_teams_bit | _game_engine_observers_bit | _game_engine_tie_resolution_bit | _game_engine_round_switch_resets_map_bit | _game_engine_always_invisible_bit | _game_engine_motion_sensor_bit | _game_engine_teams_bit),
	k_game_engine_flags_bits_required = bits_required_for(FLAG(k_game_engine_flags_count - 1))
};

enum e_game_engine_round_setting
{
	_game_engine_round_setting_1_round = 0,
	_game_engine_round_setting_2_rounds,
	_game_engine_round_setting_4_rounds,
	_game_engine_round_setting_6_rounds,
	_game_engine_round_setting_first_to_2_rounds,
	_game_engine_round_setting_first_to_3_rounds,
	_game_engine_round_setting_first_to_4_rounds,

	k_game_engine_round_setting_count,
	k_game_engine_round_setting_bits_required = bits_required_for(k_game_engine_round_setting_count - 1)
};

enum e_game_engine_join_in_progress
{
	_game_engine_join_in_progress_on = 0,
	_game_engine_join_in_progress_uneven,
	_game_engine_join_in_progress_off,

	k_game_engine_join_in_progress_count,
	k_game_engine_join_in_progress_bits_required = bits_required_for(k_game_engine_join_in_progress_count - 1)
};

enum e_game_engine_shield_setting
{
	_game_engine_shield_normal = 0,
	_game_engine_shield_off,
	_game_engine_shield_supershield,

	k_game_engine_shield_setting_count,
	k_game_engine_shield_setting_bits_required = bits_required_for(k_game_engine_shield_setting_count - 1)
};

enum e_game_engine_team_score
{
	_game_engine_team_score_sum = 0,
	_game_engine_team_score_min,
	_game_engine_team_score_max,

	k_game_engine_team_score_count,
	k_game_engine_team_score_bits_required = bits_required_for(k_game_engine_team_score_count - 1),
	k_game_engine_team_score_invalid = NONE
};

enum e_game_engine_team_respawn
{
	_game_engine_team_respawn_inheritance = 0,
	_game_engine_team_respawn_cycling,
	_game_engine_team_respawn_standard,

	k_game_engine_team_respawn_count,
	k_game_engine_team_respawn_bits_required = bits_required_for(k_game_engine_team_respawn_count - 1)
};

enum e_game_engine_respawn_setting : int8
{
	_game_engine_respawn_setting_on = 0,
	_game_engine_respawn_setting_off,
	_game_engine_respawn_setting_half,
	_game_engine_respawn_setting_double,

	k_game_engine_respawn_setting_count,
	k_game_engine_respawn_setting_bits_required = bits_required_for(k_game_engine_respawn_setting_count - 1)
};

enum e_game_engine_light_land_vehicle : int8
{
	_game_engine_light_land_vehicle_default = 0,
	_game_engine_light_land_vehicle_warthog,
	_game_engine_light_land_vehicle_gauss_warthog,
	_game_engine_light_land_vehicle_ghost,
	_game_engine_light_land_vehicle_mongoose,
	_game_engine_light_land_vehicle_shadow,
	_game_engine_light_land_vehicle_random,
	_game_engine_light_land_vehicle_none,

	k_game_engine_light_land_vehicle_count,
	k_game_engine_light_land_vehicle_bits_required = bits_required_for(k_game_engine_light_land_vehicle_count - 1)
};

enum e_game_engine_heavy_land_vehicle : int8
{
	_game_engine_heavy_land_vehicle_default = 0,
	_game_engine_heavy_land_vehicle_scorpion,
	_game_engine_heavy_land_vehicle_wraith,
	_game_engine_heavy_land_vehicle_random,
	_game_engine_heavy_land_vehicle_none,

	k_game_engine_heavy_land_vehicle_count,
	k_game_engine_heavy_land_vehicle_bits_required = bits_required_for(k_game_engine_heavy_land_vehicle_count - 1)
};

enum e_game_engine_flying_vehicle : int8
{
	_game_engine_flying_vehicle_default = 0,
	_game_engine_flying_vehicle_banshee,
	_game_engine_flying_vehicle_falcon,
	_game_engine_flying_vehicle_random,
	_game_engine_flying_vehicle_none,

	k_game_engine_flying_vehicle_count,
	k_game_engine_flying_vehicle_bits_required = bits_required_for(k_game_engine_flying_vehicle_count - 1)
};

enum e_game_engine_turret_vehicle : int8
{
	_game_engine_turret_vehicle_default = 0,
	_game_engine_turret_vehicle_human_ap,
	_game_engine_turret_vehicle_human_av,
	_game_engine_turret_vehicle_covenant_ap,
	_game_engine_turret_vehicle_covenant_av,
	_game_engine_turret_vehicle_random,
	_game_engine_turret_vehicle_none,

	k_game_engine_turret_vehicle_count,
	k_game_engine_turret_vehicle_bits_required = bits_required_for(k_game_engine_turret_vehicle_count - 1)
};

enum e_game_engine_weapon_set : int8
{
	_game_engine_weapon_set_default = 0,
	_game_engine_weapon_set_none,
	_game_engine_weapon_set_rockets,
	_game_engine_weapon_set_shotguns,
	_game_engine_weapon_set_swords,
	_game_engine_weapon_set_brute_shots,
	_game_engine_weapon_set_halo_classic,
	_game_engine_weapon_set_new_classic,
	_game_engine_weapon_set_heavy_weapons,
	_game_engine_weapon_set_all_duals,
	_game_engine_weapon_set_no_duals,
	_game_engine_weapon_set_rifles,
	_game_engine_weapon_set_sniping,
	_game_engine_weapon_set_no_sniping,
	_game_engine_weapon_set_pistols,
	_game_engine_weapon_set_plasma,
	_game_engine_weapon_set_human,
	_game_engine_weapon_set_covenant,
	_game_engine_weapon_set_sentinel,
	_game_engine_weapon_set_random,

	k_game_engine_weapon_set_count,
	k_game_engine_weapon_set_bits_required = bits_required_for(k_game_engine_weapon_set_count - 1)
};

enum e_game_engine_starting_weapon : int8
{
	_game_engine_starting_weapon_default = 0,
	_game_engine_starting_weapon_none,
	_game_engine_starting_weapon_random,
	_game_engine_starting_weapon_battle_rifle,
	_game_engine_starting_weapon_magnum,
	_game_engine_starting_weapon_smg,
	_game_engine_starting_weapon_plasma_pistol,
	_game_engine_starting_weapon_plasma_rifle,
	_game_engine_starting_weapon_rocket_launcher,
	_game_engine_starting_weapon_shotgun,
	_game_engine_starting_weapon_sniper_rifle,
	_game_engine_starting_weapon_brute_shot,
	_game_engine_starting_weapon_needler,
	_game_engine_starting_weapon_carbine,
	_game_engine_starting_weapon_beam_rifle,
	_game_engine_starting_weapon_disintegrator,
	_game_engine_starting_weapon_fuel_rod_gun,
	_game_engine_starting_weapon_plasma_sword,
	_game_engine_starting_weapon_brute_plasma,

	k_game_engine_starting_weapon_count,
	k_game_engine_starting_weapon_bits_required = bits_required_for(k_game_engine_starting_weapon_count - 1)
};

enum e_ctf_engine_player_speed
{
	_ctf_engine_player_speed_slow = 0,
	_ctf_engine_player_speed_normal,
	_ctf_engine_player_speed_fast,

	k_ctf_engine_player_speed_count,
	k_ctf_engine_player_speed_bits_required = bits_required_for(k_ctf_engine_player_speed_count - 1),
	k_ctf_engine_players_speed_invalid = NONE
};

enum e_oddball_player_speed : uint16
{
	_oddball_engine_player_speed_slow = 0,
	_oddball_engine_player_speed_normal,
	_oddball_engine_player_speed_fast,

	k_oddball_player_speed_count,
	k_oddball_player_speed_bits_required = bits_required_for(k_oddball_player_speed_count - 1)
};

enum e_game_engine_weapon_hit
{
	_game_engine_multiplayer_weapon_hit_extra_damage = 0,
	_game_engine_multiplayer_weapon_hit_normal_damage,

	k_game_engine_weapon_hit_count,
	k_game_engine_weapon_hit_bits_required = bits_required_for(k_game_engine_weapon_hit_count - 1)
};


enum e_slayer_engine_flags : uint32
{
	_slayer_engine_bonus_points_bit = 0,
	_slayer_engine_suicide_point_loss_bit,
	_slayer_engine_death_point_loss_bit,

	k_slayer_engine_flags_count,
	k_slayer_engine_flags_bits_required = bits_required_for(MASK(k_slayer_engine_flags_count)),

	k_slayer_engine_clear_all_bits_mask = FLAG(_slayer_engine_death_point_loss_bit | _slayer_engine_suicide_point_loss_bit | _slayer_engine_bonus_points_bit)
};

enum e_ctf_game_type
{
	_ctf_game_type_multi_flag = 0,
	_ctf_game_type_single_flag,
	_ctf_game_type_neutral_flag,

	k_ctf_game_type_count,
	k_ctf_game_type_bits_required = bits_required_for(k_ctf_game_type_count - 1)
};

enum e_ctf_engine_home_flag_waypoint_type
{
	_ctf_engine_home_flag_waypoint_neutral = 0,
	_ctf_engine_home_flag_waypoint_on,
	_ctf_engine_home_flag_waypoint_away_from_home,
	_ctf_engine_home_flag_waypoint_off,

	k_ctf_engine_home_flag_waypoint_type_count,
	k_ctf_engine_home_flag_waypoint_type_bits_required = bits_required_for(k_ctf_engine_home_flag_waypoint_type_count - 1)
};

enum e_ctf_engine_enemy_bomb_waypoint_type : uint32
{
	_ctf_engine_enemy_bomb_waypoint_always_on = 0,
	_ctf_engine_enemy_bomb_waypoint_uncontrolled,
	_ctf_engine_enemy_bomb_waypoint_armed,
	_ctf_engine_enemy_bomb_waypoint_off,

	k_ctf_engine_enemy_bomb_waypoint_type_count,
	k_ctf_engine_enemy_bomb_waypoint_type_bits_required = bits_required_for(k_ctf_engine_enemy_bomb_waypoint_type_count - 1)
};

enum e_ctf_engine_flags : int8
{
	_ctf_engine_sudden_death_bit,
	_ctf_engine_flag_can_be_returned_bit,
	_ctf_engine_flag_at_home_to_score_bit,
	_ctf_engine_carriers_can_drive_and_gun_bit,
	_ctf_engine_sticky_arming_bit,
	_ctf_engine_carriers_damage_resistant_bit,
	_ctf_engine_carriers_invisible_bit,

	k_ctf_engine_flags_count,
	k_ctf_engine_flags_bits_required = CHAR_BITS
};

enum e_oddball_engine_waypoint_type
{
	_oddball_waypoint_type_always_on = 0,
	_oddball_waypoint_type_neutral,
	_oddball_waypoint_type_team_control,
	_oddball_waypoint_type_off,

	k_oddball_waypoint_type_count,
	k_oddball_waypoint_type_bits_required = bits_required_for(k_oddball_waypoint_type_count - 1)
};

enum e_king_engine_flags : uint32
{
	_king_engine_uncontested_hill_to_score_bit = 0,
	_king_engine_team_multiplier_bit,
	_king_engine_extra_damage_on_hill_bit,
	_king_engine_damage_resistance_on_hill_bit,
	_king_engine_invis_on_hill_bit,

	k_king_engine_flags_count,
	k_king_engine_flags_bits_required = bits_required_for(FLAG(k_king_engine_flags_count - 1)),

	k_king_engine_clear_unused_bits_mask = FLAG(_king_engine_invis_on_hill_bit | _king_engine_damage_resistance_on_hill_bit | _king_engine_extra_damage_on_hill_bit | _king_engine_team_multiplier_bit | _king_engine_uncontested_hill_to_score_bit)
};

enum e_oddball_engine_flags : uint32
{
	_oddball_engine_carriers_can_drive_and_gun_bit = 0,
	_oddball_engine_carriers_always_invis_bit,
	_oddball_engine_carriers_damage_resistance_bit,

	k_oddball_engine_flags_count,
	k_oddball_engine_flags_bits_required = bits_required_for(FLAG(k_oddball_engine_flags_count - 1)),
	k_oddball_engine_clear_unused_bits_mask = FLAG(_oddball_engine_carriers_damage_resistance_bit | _oddball_engine_carriers_always_invis_bit | _oddball_engine_carriers_can_drive_and_gun_bit)
};

enum e_juggernaut_engine_flags
{
	_juggernaut_has_motion_sensor_bit = 0,
	_juggernaut_has_supershield_bit,
	_juggernaut_has_active_camo_bit,
	_juggernaut_betrayal_point_loss_bit,
	_juggernaut_does_extra_damage_bit,
	_juggernaut_has_infinite_ammo_bit,
	_juggernaut_is_damage_resistant_bit,

	k_juggernaut_engine_flags_count,
	k_juggernaut_engine_flags_bits_required = bits_required_for(FLAG(k_juggernaut_engine_flags_count - 1)),
	k_juggernaut_engine_clear_unused_bits_mask = FLAG(_juggernaut_is_damage_resistant_bit | _juggernaut_has_infinite_ammo_bit | _juggernaut_does_extra_damage_bit | _juggernaut_betrayal_point_loss_bit | _juggernaut_has_active_camo_bit | _juggernaut_has_supershield_bit | _juggernaut_has_motion_sensor_bit)
};

enum e_headhunter_max_heads_carried : int8
{
	_headhunter_max_heads_carried_none,
	_headhunter_max_heads_carried_one,
	_headhunter_max_heads_carried_five,
	_headhunter_max_heads_carried_ten,

	k_headhunter_max_heads_carried_count,
	k_headhunter_max_heads_carried_bits_required = bits_required_for(k_headhunter_max_heads_carried_count - 1),
	k_headhunter_max_heads_carried_invalid = NONE
};

enum e_headhunter_engine_flags : uint32
{
	_headhunter_engine_uncontested_hill_to_score_bit = 0,
	_headhunter_engine_team_multiplier_bit = 1,
	_headhunter_engine_extra_damage_on_hill_bit = 2,
	_headhunter_engine_damage_resistance_on_hill_bit = 3,
	_headhunter_engine_invis_on_hill_bit = 4,
	_headhunter_engine_flag_moving_bin,
	_headhunter_engine_flag_point_multiplier,
	_headhunter_engine_flag_suicide_point_loss,
	_headhunter_engine_flag_death_point_loss,

	k_headhunter_engine_flags_count
};

enum e_cartographer_variant_settings_version : uint8
{
	_cartographer_variant_settings_version_none,
	_cartographer_variant_settings_version_one,
	k_cartographer_variant_settings_version_count,

	k_cartographer_variant_settings_version_bits_required = bits_required_for(k_cartographer_variant_settings_version_count - 1)
};

enum e_cartographer_variant_flags : uint32
{
	_cartographer_variant_engine_mode,
	_cartographer_variant_infinite_ammo,
	_cartographer_variant_infinite_grenades,
	_cartographer_variant_explosion_physics,
	_cartographer_variant_force_default_fov,
	_cartographer_variant_force_default_weapon_offsets,
	_cartographer_variant_force_default_cross_hair_offset,
	_cartographer_variant_disable_dub_shot,
	_cartographer_variant_invincible_players,

	k_cartographer_variant_flags_count,
	k_cartographer_variant_flags_bits_required = bits_required_for(k_cartographer_variant_flags_count - 1),
};

enum e_game_speed_modifier : int8
{
	_game_speed_modifier_none,
	_game_speed_modifier_half,
	_game_speed_modifier_hundred_fifty,
	_game_speed_modifier_double,
	_game_speed_modifier_ludicrous,

	k_game_speed_modifier_count,
	k_game_speed_modifier_bits_required = bits_required_for(k_game_speed_modifier_count - 1),
	k_game_speed_modifier_invalid = NONE
};

enum e_game_gravity_modifier : int8
{
	_game_gravity_modifier_none,
	_game_gravity_modifier_twenty_five_percent,
	_game_gravity_modifier_fifty_percent,
	_game_gravity_modifier_seventy_five_percent,
	_game_gravity_modifier_hundred_twenty_five_percent,
	_game_gravity_modifier_hundred_fifty_percent,
	_game_gravity_modifier_hundred_seventy_five_percent,
	_game_gravity_modifier_two_hundred,

	k_game_gravity_modifier_count,
	k_game_gravity_modifier_bits_required = bits_required_for(k_game_gravity_modifier_count - 1),
	k_game_gravity_modifier_invalid = NONE
};

enum e_player_spawn_protection_timer : int8
{
	_player_spawn_protection_timer_one_second,
	_player_spawn_protection_timer_none,
	_player_spawn_protection_timer_three_seconds,
	_player_spawn_protection_timer_five_seconds,
	_player_spawn_protection_timer_ten_seconds,

	k_player_spawn_protection_timer_count,
	k_player_spawn_protection_timer_bits_required = bits_required_for(k_player_spawn_protection_timer_count-1),
	k_player_spawn_protection_timer_invalid = NONE
};

enum e_player_speed_modifier : int8
{
	_player_speed_modifier_none = 0,
	_player_speed_modifier_twenty_five_percent,
	_player_speed_modifier_fifty_percent,
	_player_speed_modifier_seventy_five_percent,
	_player_speed_modifier_hundred_twenty_five_percent,
	_player_speed_modifier_hundred_fifty_percent,
	_player_speed_modifier_hundred_seventy_five_percent,
	_player_speed_modifier_two_hundred_percent,
	_player_speed_modifier_two_hundred_twenty_five_percent,
	_player_speed_modifier_two_hundred_fifty_percent,
	_player_speed_modifier_two_hundred_seventy_five_percent,
	_player_speed_modifier_three_hundred_percent,

	k_player_speed_modifier_count,
	k_player_speed_modifier_bits_required = bits_required_for(k_player_speed_modifier_count - 1),
	k_player_speed_modifier_invalid = NONE
};

struct s_variant_description_map
{
	const char* name;
	e_game_variant_description_index index;
};

struct s_ctf_variant_settings
{
	c_flags_no_init<e_ctf_engine_flags, uint8, k_ctf_engine_flags_count> flags;
	uint32 flag_reset_time; // in seconds
	e_ctf_engine_player_speed speed_with_flag;
	e_game_engine_weapon_hit flag_hit_damage;
	e_ctf_engine_enemy_bomb_waypoint_type waypoint_type;
	e_ctf_game_type game_type;
	uint16 bomb_arming_time;
	uint16 bomb_fuse_time;
};

struct s_slayer_variant_settings
{
	c_flags_no_init<e_slayer_engine_flags, uint32, k_slayer_engine_flags_count> flags;
};

struct s_oddball_variant_settings
{
	c_flags_no_init<e_oddball_engine_flags, uint32, k_oddball_engine_flags_count> flags;
	uint16 ball_count;
	e_game_engine_weapon_hit ball_hit_damage;
	e_oddball_player_speed speed_with_ball;
	e_oddball_engine_waypoint_type waypoint_to_ball;
};

struct s_king_variant_settings
{
	c_flags_no_init<e_king_engine_flags, uint32, k_king_engine_flags_count> flags;
	uint16 hill_move_time;
	int8 pad;
};

struct s_juggernaut_variant_settings
{
	c_flags_no_init<e_juggernaut_engine_flags, uint32, k_juggernaut_engine_flags_count> flags;
	e_ctf_engine_player_speed juggernaut_movement_speed;
	int8 pad;
};

struct s_head_hunter_settings
{
	c_flags_no_init<e_headhunter_engine_flags, uint32, k_headhunter_engine_flags_count> flags;
	uint16 hill_move_time;
	e_ctf_engine_player_speed speed_with_heads;
	e_headhunter_max_heads_carried max_heads_carried;
};

struct s_territories_variant_settings
{
	uint16 territory_count;
	uint16 territory_contest_time;
	uint16 territory_capture_time;
};

union s_game_engine_variant
{
	int8 variant_storage[64];
	s_ctf_variant_settings ctf;
	s_slayer_variant_settings slayer;
	s_oddball_variant_settings oddball;
	s_king_variant_settings king;
	s_juggernaut_variant_settings juggernaut;
	s_territories_variant_settings territories;
	s_ctf_variant_settings assault;
	s_head_hunter_settings head_hunter;
};
ASSERT_STRUCT_SIZE(s_game_engine_variant, 64);

#pragma pack(push, 1)
struct s_cart_variant_settings
{
	e_cartographer_variant_settings_version version;
	c_flags_no_init<e_cartographer_variant_flags, uint32, k_cartographer_variant_flags_count> flags;
	e_game_speed_modifier game_speed;
	e_game_gravity_modifier gravity;
	e_player_spawn_protection_timer spawn_protection;
	e_player_speed_modifier player_speed;

	int8 available_storage[15];
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(s_cart_variant_settings, 24);


struct s_game_variant
{
	int16 flags;
	int8 pad;
	e_game_variant_description_index description_index;
	wchar_t variant_name[32];
	e_game_engine_type variant_game_engine_index;
	c_flags_no_init<e_game_engine_flags, uint32, k_game_engine_flags_count> game_engine_flags;

	/* match settings */
	e_game_engine_round_setting round_setting;
	int32 score_to_win_round;
	int32 round_time_limit;
	e_game_engine_join_in_progress join_in_progress_setting;
	int32 unused_match_settings[6];

	/* player settings */
	int32 max_players;
	int32 max_living_players;
	int32 lives_per_round;
	int32 respawn_time;
	int32 suicide_penalty;
	e_game_engine_shield_setting shield_setting;
	int32 unused_player_settings[6];

	/* team settings */
	e_game_engine_team_score team_score_setting;
	e_game_engine_team_respawn team_respawn_setting;
	int32 betrayal_penalty;
	int32 unk;
	int32 maximum_allowable_teams;
	int32 unused_team_settings[5];

	/* vehicle settings */
	e_game_engine_respawn_setting vehicle_respawn_setting;
	e_game_engine_light_land_vehicle primary_light_land_vehicle;
	e_game_engine_light_land_vehicle secondary_light_land_vehicle;
	e_game_engine_heavy_land_vehicle primary_heavy_land_vehicle;
	e_game_engine_flying_vehicle primary_flying_vehicle;
	e_game_engine_heavy_land_vehicle secondary_heavy_land_vehicle;
	e_game_engine_turret_vehicle primary_turret_vehicle;
	e_game_engine_turret_vehicle secondary_turret_vehicle;

	/* equipment settings */
	e_game_engine_weapon_set weapon_set;
	e_game_engine_respawn_setting weapon_respawn_setting;
	e_game_engine_starting_weapon starting_equipment_primary;
	e_game_engine_starting_weapon starting_equipment_secondary;

	/* Maybe make use of these? */

	union
	{
		int32 unused_settings[6];
		s_cart_variant_settings cartographer_settings;
	};

	s_game_engine_variant game_engine_variant;
};
ASSERT_STRUCT_SIZE(s_game_variant, 304);

/* prototypes */

s_game_variant* get_game_variant(void);

void __cdecl game_variant_build_default(s_game_variant* variant, int32 game_variant_type);

s_game_variant* __cdecl get_default_game_variant_by_name(wchar_t const* name);

bool game_variant_is_valid(s_game_variant* variant);

void game_variant_cartographer_settings_default_new(s_game_variant* variant);

bool game_variant_cartographer_settings_validate(s_game_variant* variant);