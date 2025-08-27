#include "stdafx.h"
#include "network_game_definitions.h"

#include "game/game_engine_territories.h"
#include "game/players.h"

// TODO: rewrite network_game_definitions_encode_game_variant

/* typedef */
typedef bool(__cdecl* t_network_game_definitions_game_variant)(c_bitstream* packet, s_game_variant* variant);
t_network_game_definitions_game_variant p_network_game_definitions_decode_game_variant;
t_network_game_definitions_game_variant p_network_game_definitions_encode_game_variant;

/* public */

void network_game_definitions_apply_patches()
{
	DETOUR_ATTACH(p_network_game_definitions_decode_game_variant, Memory::GetAddress<t_network_game_definitions_game_variant>(0x1E2F74, 0x1B45BC), network_game_definitions_decode_game_variant);
	DETOUR_ATTACH(p_network_game_definitions_encode_game_variant, Memory::GetAddress<t_network_game_definitions_game_variant>(0x1E2865, 0x1B3EAC), network_game_definitions_encode_game_variant);
}

void network_game_definitions_encode_game_variant(c_bitstream* packet, s_game_variant* variant)
{
	if (!variant->variant_game_engine_index)
		return;

	packet->write_integer("variant-game-engine-index", variant->variant_game_engine_index, k_game_engine_type_bits_required);

	packet->write_integer("variant-flags", variant->flags, 1);

	packet->write_string_wchar("variant->desc", variant->variant_name, NUMBEROF(variant->variant_name));

	packet->write_integer("description-index", variant->description_index, k_game_variant_description_bits_required);

	packet->write_integer("flags", variant->game_engine_flags.get_unsafe(), k_game_engine_flags_bits_required);

	packet->write_integer("round-setting", variant->round_setting, k_game_engine_round_setting_bits_required);

	packet->write_integer("score-to-win-round", variant->score_to_win_round, SHORT_BITS);

	packet->write_integer("round-time-limit", variant->round_time_limit, SHORT_BITS);

	packet->write_integer("join-in-progress", variant->join_in_progress_setting, k_game_engine_join_in_progress_bits_required);

	packet->write_integer("max-players", variant->max_players, bits_required_for(k_maximum_players));

	packet->write_integer("max-living-players", variant->max_living_players, bits_required_for(k_maximum_players));

	packet->write_integer("lives-per-round", variant->lives_per_round, SHORT_BITS);

	packet->write_integer("respawn-time", variant->respawn_time, SHORT_BITS);

	packet->write_integer("suicide-penalty", variant->suicide_penalty, SHORT_BITS);

	packet->write_integer("shield-setting", variant->shield_setting, k_game_engine_shield_setting_bits_required);

	packet->write_integer("team-score-setting", variant->team_score_setting, k_game_engine_team_score_bits_required);

	packet->write_integer("team-respawn-setting", variant->team_respawn_setting, k_game_engine_team_respawn_bits_required);

	packet->write_integer("betrayal-penalty", variant->betrayal_penalty, SHORT_BITS);

	packet->write_integer("max-allowable-teams", variant->maximum_allowable_teams, bits_required_for(k_game_multiplayer_team_count));

	packet->write_integer("vehicle-respawn-setting", variant->vehicle_respawn_setting, k_game_engine_respawn_setting_bits_required);

	packet->write_integer("player-light-land-vehicle", variant->primary_light_land_vehicle, k_game_engine_light_land_vehicle_bits_required);

	packet->write_integer("secondary-light-land-vehicle", variant->secondary_light_land_vehicle, k_game_engine_light_land_vehicle_bits_required);

	packet->write_integer("primary-heavy-land-vehicle", variant->primary_heavy_land_vehicle, k_game_engine_heavy_land_vehicle_bits_required);

	packet->write_integer("primary-flying-vehicle", variant->primary_flying_vehicle, k_game_engine_flying_vehicle_bits_required);

	packet->write_integer("secondary-heavy-land-vehicle", variant->secondary_heavy_land_vehicle, k_game_engine_heavy_land_vehicle_bits_required);

	packet->write_integer("primary-turret-vehicle", variant->primary_turret_vehicle, k_game_engine_turret_vehicle_bits_required);

	packet->write_integer("secondary-turret-vehicle", variant->secondary_turret_vehicle, k_game_engine_turret_vehicle_bits_required);

	packet->write_integer("weapon-set", variant->weapon_set, k_game_engine_weapon_set_bits_required);

	packet->write_integer("weapon-respawn-setting", variant->weapon_respawn_setting, k_game_engine_respawn_setting_bits_required);

	packet->write_integer("starting-equipment-primary", variant->starting_equipment_primary, k_game_engine_starting_weapon_bits_required);

	packet->write_integer("starting-equipment-secondary", variant->starting_equipment_secondary, k_game_engine_starting_weapon_bits_required);

	switch (variant->variant_game_engine_index)
	{
	case _game_engine_type_ctf:
	{
		packet->write_integer("flags", variant->game_engine_variant.ctf.flags.get_unsafe(), k_ctf_engine_flags_bits_required);
		packet->write_integer("flag-reset-time", variant->game_engine_variant.ctf.flag_reset_time, SHORT_BITS);
		packet->write_integer("speed-with-flag", variant->game_engine_variant.ctf.speed_with_flag, k_ctf_engine_player_speed_bits_required);
		packet->write_integer("flag-hit-damage", variant->game_engine_variant.ctf.flag_hit_damage, k_game_engine_weapon_hit_bits_required);
		packet->write_integer("waypoint-to-home-flag-type", variant->game_engine_variant.ctf.waypoint_type, k_ctf_engine_enemy_bomb_waypoint_type_bits_required);
		packet->write_integer("game-type", variant->game_engine_variant.ctf.game_type, k_ctf_game_type_bits_required);
		break;
	}
	case _game_engine_type_slayer:
	{
		packet->write_integer("flags", variant->game_engine_variant.slayer.flags.get_unsafe(), k_slayer_engine_flags_bits_required);
		break;
	}
	case _game_engine_type_oddball:
	{
		packet->write_integer("flags", variant->game_engine_variant.oddball.flags.get_unsafe(), k_oddball_engine_flags_bits_required);
		packet->write_integer("ball-count", variant->game_engine_variant.oddball.ball_count, bits_required_for(k_game_engine_oddball_maximum_balls));
		packet->write_integer("ball-hit-damage", variant->game_engine_variant.oddball.ball_hit_damage, k_game_engine_weapon_hit_bits_required);
		packet->write_integer("speed-with-ball", variant->game_engine_variant.oddball.speed_with_ball, k_oddball_player_speed_bits_required);
		packet->write_integer("waypoint-to-ball", variant->game_engine_variant.oddball.waypoint_to_ball, k_oddball_waypoint_type_bits_required);
		break;
	}
	case _game_engine_type_koth:
	{
		packet->write_integer("flags", variant->game_engine_variant.king.flags.get_unsafe(), k_king_engine_flags_bits_required);
		packet->write_integer("hill-move-time", variant->game_engine_variant.king.hill_move_time, SHORT_BITS);
		break;
	}
	case _game_engine_type_race:
	{
		// No additional parameters for race mode.
		break;
	}
	case _game_engine_type_headhunter:
	{
		packet->write_integer("flags", variant->game_engine_variant.head_hunter.flags.get_unsafe(), k_king_engine_flags_bits_required);
		packet->write_integer("hill-move-time", variant->game_engine_variant.head_hunter.hill_move_time, SHORT_BITS);
		packet->write_integer("speed-with-heads", variant->game_engine_variant.head_hunter.speed_with_heads, k_ctf_engine_player_speed_bits_required);
		packet->write_integer("max-heads-carried", variant->game_engine_variant.head_hunter.max_heads_carried, k_headhunter_max_heads_carried_bits_required);
		break;
	}
	case _game_engine_type_juggernaut:
	{
		packet->write_integer("flags", variant->game_engine_variant.juggernaut.flags.get_unsafe(), k_juggernaut_engine_flags_bits_required);
		packet->write_integer("juggernaut-movement-speed", variant->game_engine_variant.juggernaut.juggernaut_movement_speed, k_ctf_engine_player_speed_bits_required);
		break;
	}
	case _game_engine_type_territories:
	{
		packet->write_integer("territory-count", variant->game_engine_variant.territories.territory_count, bits_required_for(k_maximum_territories_flags));
		packet->write_integer("territory-contest-time", variant->game_engine_variant.territories.territory_contest_time, SHORT_BITS);
		packet->write_integer("territory-capture-time", variant->game_engine_variant.territories.territory_capture_time, SHORT_BITS);
		break;
	}
	case _game_engine_type_assault:
	{
		packet->write_integer("bomb-arming-time", variant->game_engine_variant.assault.bomb_arming_time, SHORT_BITS);
		packet->write_integer("bomb-fuse-time", variant->game_engine_variant.assault.bomb_fuse_time, SHORT_BITS);
		packet->write_integer("flags", variant->game_engine_variant.assault.flags.get_unsafe(), k_ctf_engine_flags_bits_required);
		packet->write_integer("flag-reset-time", variant->game_engine_variant.assault.flag_reset_time, SHORT_BITS);
		packet->write_integer("speed-with-flag", variant->game_engine_variant.assault.speed_with_flag, k_ctf_engine_player_speed_bits_required);
		packet->write_integer("flag-hit-damage", variant->game_engine_variant.assault.flag_hit_damage, k_game_engine_weapon_hit_bits_required);
		packet->write_integer("waypoint-to-home-flag-type", variant->game_engine_variant.assault.waypoint_type, k_ctf_engine_enemy_bomb_waypoint_type_bits_required);
		packet->write_integer("game-type", variant->game_engine_variant.assault.game_type, k_ctf_game_type_bits_required);
		break;
	}
	case _game_engine_type_none:
	{
		// Native behavior (or coop mode) requires no additional engine-specific encoding.
		break;
	}
	default:
		// Optionally handle an unknown game engine type.
		break;
	}

	// cartographer game variant settings encoding

	packet->write_integer("cart-version", variant->cartographer_settings.version, 32);
	packet->write_integer("cart-flags", variant->cartographer_settings.flags.get_unsafe(), k_cartographer_variant_flags_bits_required);
	packet->write_integer("cart-game-speed", variant->cartographer_settings.game_speed, k_game_speed_modifier_bits_required);
	packet->write_integer("cart-gravity", variant->cartographer_settings.gravity, k_game_gravity_modifier_bits_required);
	packet->write_integer("cart-spawn-protection", variant->cartographer_settings.spawn_protection, k_player_spawn_protection_timer_bits_required);
}

bool network_game_definitions_decode_game_variant(c_bitstream* packet, s_game_variant* variant)
{
	variant->variant_game_engine_index = (e_game_engine_type)packet->read_integer("variant-game-engine-index", k_game_engine_type_bits_required);

	if (!variant->variant_game_engine_index)
	{
		csmemset(variant, 0, sizeof(s_game_variant));
		return false;
	}

	variant->flags = (int16)packet->read_integer("variant-flags", 1);

	packet->read_string_wchar("variant-desc", variant->variant_name, NUMBEROF(variant->variant_name));

	variant->description_index = (e_game_variant_description_index)packet->read_integer("description-index", k_game_variant_description_bits_required);

	variant->game_engine_flags.set_unsafe(packet->read_integer("flags", k_game_engine_flags_bits_required));

	variant->round_setting = (e_game_engine_round_setting)packet->read_integer("round-setting", k_game_engine_round_setting_bits_required);

	variant->score_to_win_round = packet->read_integer("score-to-win-round", SHORT_BITS);

	variant->round_time_limit = packet->read_integer("round-time-limit", SHORT_BITS);

	variant->join_in_progress_setting = (e_game_engine_join_in_progress)packet->read_integer("join-in-progress", k_game_engine_join_in_progress_bits_required);

	variant->max_players = packet->read_integer("max-players", bits_required_for(k_maximum_players));

	variant->max_living_players = packet->read_integer("max-living-players", bits_required_for(k_maximum_players));

	variant->lives_per_round = packet->read_integer("lives-per-round", SHORT_BITS);

	variant->respawn_time = packet->read_integer("respawn-time", SHORT_BITS);

	variant->suicide_penalty = packet->read_integer("suicide-penalty", SHORT_BITS);

	variant->shield_setting = (e_game_engine_shield_setting)packet->read_integer("shield-setting", k_game_engine_shield_setting_bits_required);

	variant->team_score_setting = (e_game_engine_team_score)packet->read_integer("team-score-setting", k_game_engine_team_score_bits_required);

	variant->team_respawn_setting = (e_game_engine_team_respawn)packet->read_integer("team-respawn-setting", k_game_engine_team_respawn_bits_required);

	variant->betrayal_penalty = packet->read_integer("betrayal-penalty", SHORT_BITS);

	variant->maximum_allowable_teams = packet->read_integer("max-allowable-teams", bits_required_for(k_game_multiplayer_team_count));

	variant->vehicle_respawn_setting = (e_game_engine_respawn_setting)packet->read_integer("vehicle-respawn-setting", k_game_engine_respawn_setting_bits_required);

	variant->primary_light_land_vehicle = (e_game_engine_light_land_vehicle)packet->read_integer("primary-light-land-vehicle", k_game_engine_light_land_vehicle_bits_required);

	variant->secondary_light_land_vehicle = (e_game_engine_light_land_vehicle)packet->read_integer("secondary-light-land-vehicle", k_game_engine_light_land_vehicle_bits_required);

	variant->primary_heavy_land_vehicle = (e_game_engine_heavy_land_vehicle)packet->read_integer("primary-heavy-land-vehicle", k_game_engine_heavy_land_vehicle_bits_required);

	variant->primary_flying_vehicle = (e_game_engine_flying_vehicle)packet->read_integer("primary-flying-vehicle", k_game_engine_flying_vehicle_bits_required);

	variant->secondary_heavy_land_vehicle = (e_game_engine_heavy_land_vehicle)packet->read_integer("secondary-heavy-land-vehicle", k_game_engine_heavy_land_vehicle_bits_required);

	variant->primary_turret_vehicle = (e_game_engine_turret_vehicle)packet->read_integer("primary-turret-vehicle", k_game_engine_turret_vehicle_bits_required);

	variant->secondary_turret_vehicle = (e_game_engine_turret_vehicle)packet->read_integer("secondary-turret-vehicle", k_game_engine_turret_vehicle_bits_required);

	variant->weapon_set = (e_game_engine_weapon_set)packet->read_integer("weapon-set", k_game_engine_weapon_set_bits_required);

	variant->weapon_respawn_setting = (e_game_engine_respawn_setting)packet->read_integer("weapon-respawn-setting", k_game_engine_respawn_setting_bits_required);

	variant->starting_equipment_primary = (e_game_engine_starting_weapon)packet->read_integer("starting-equipment-primary", k_game_engine_starting_weapon_bits_required);

	variant->starting_equipment_secondary = (e_game_engine_starting_weapon)packet->read_integer("starting-equipment-secondary", k_game_engine_starting_weapon_bits_required);

	switch (variant->variant_game_engine_index)
	{
	case _game_engine_type_ctf:
	{
		variant->game_engine_variant.ctf.flags.set_unsafe((uint8)packet->read_integer("flags", k_ctf_engine_flags_bits_required));
		variant->game_engine_variant.ctf.flag_reset_time = packet->read_integer("flag-reset-time", SHORT_BITS);
		variant->game_engine_variant.ctf.speed_with_flag = (e_ctf_engine_player_speed)packet->read_integer("speed-with-flag", k_ctf_engine_player_speed_bits_required);
		variant->game_engine_variant.ctf.flag_hit_damage = (e_game_engine_weapon_hit)packet->read_integer("flag-hit-damage", k_game_engine_weapon_hit_bits_required);
		variant->game_engine_variant.ctf.waypoint_type = (e_ctf_engine_enemy_bomb_waypoint_type)packet->read_integer("waypoint-to-home-flag-type", k_ctf_engine_enemy_bomb_waypoint_type_bits_required);
		variant->game_engine_variant.ctf.game_type = (e_ctf_game_type)packet->read_integer("game-type", k_ctf_game_type_bits_required);
		break;
	}
	case _game_engine_type_slayer:
	{
		variant->game_engine_variant.slayer.flags.set_unsafe(packet->read_integer("flags", k_slayer_engine_flags_bits_required));
		break;
	}
	case _game_engine_type_oddball:
	{
		variant->game_engine_variant.oddball.flags.set_unsafe(packet->read_integer("flags", k_oddball_engine_flags_bits_required));
		variant->game_engine_variant.oddball.ball_count = (uint16)packet->read_integer("ball-count", bits_required_for(k_game_engine_oddball_maximum_balls));
		variant->game_engine_variant.oddball.ball_hit_damage = (e_game_engine_weapon_hit)packet->read_integer("ball-hit-damage", k_game_engine_weapon_hit_bits_required);
		variant->game_engine_variant.oddball.speed_with_ball = (e_oddball_player_speed)packet->read_integer("speed-with-ball", k_oddball_player_speed_bits_required);
		variant->game_engine_variant.oddball.waypoint_to_ball = (e_oddball_engine_waypoint_type)packet->read_integer("waypoint-to-ball", k_oddball_waypoint_type_bits_required);
		break;
	}
	case _game_engine_type_koth:
	{
		variant->game_engine_variant.king.flags.set_unsafe(packet->read_integer("flags", k_king_engine_flags_bits_required));
		variant->game_engine_variant.king.hill_move_time = (uint16)packet->read_integer("hill-move-time", SHORT_BITS);
		break;
	}
	case _game_engine_type_race:
	{
		break;
	}
	case _game_engine_type_headhunter:
	{
		variant->game_engine_variant.head_hunter.flags.set_unsafe(packet->read_integer("flags", k_king_engine_flags_bits_required));
		variant->game_engine_variant.head_hunter.hill_move_time = (uint16)packet->read_integer("hill-move-time", SHORT_BITS);
		variant->game_engine_variant.head_hunter.speed_with_heads = (e_ctf_engine_player_speed)packet->read_integer("speed-with-heads", k_ctf_engine_player_speed_bits_required);
		variant->game_engine_variant.head_hunter.max_heads_carried = (e_headhunter_max_heads_carried)packet->read_integer("max-heads-carried", k_headhunter_max_heads_carried_bits_required);
		break;
	}
	case _game_engine_type_juggernaut:
	{
		variant->game_engine_variant.juggernaut.flags.set_unsafe(packet->read_integer("flags", k_juggernaut_engine_flags_bits_required));
		variant->game_engine_variant.juggernaut.juggernaut_movement_speed = (e_ctf_engine_player_speed)packet->read_integer("juggernaut-movement-speed", k_ctf_engine_player_speed_bits_required);
		break;
	}
	case _game_engine_type_territories:
	{
		variant->game_engine_variant.territories.territory_count = (uint16)packet->read_integer("territory-count", bits_required_for(k_maximum_territories_flags));
		variant->game_engine_variant.territories.territory_contest_time = (uint16)packet->read_integer("territory-contest-time", SHORT_BITS);
		variant->game_engine_variant.territories.territory_capture_time = (uint16)packet->read_integer("territory-capture-time", SHORT_BITS);
		break;
	}
	case _game_engine_type_assault:
	{
		variant->game_engine_variant.assault.bomb_arming_time = (uint16)packet->read_integer("bomb-arming-time", SHORT_BITS);
		variant->game_engine_variant.assault.bomb_fuse_time = (uint16)packet->read_integer("bomb-fuse-time", SHORT_BITS);
		variant->game_engine_variant.assault.flags.set_unsafe((uint8)packet->read_integer("flags", k_ctf_engine_flags_bits_required));
		variant->game_engine_variant.assault.flag_reset_time = packet->read_integer("flag-reset-time", SHORT_BITS);
		variant->game_engine_variant.assault.speed_with_flag = (e_ctf_engine_player_speed)packet->read_integer("speed-with-flag", k_ctf_engine_player_speed_bits_required);
		variant->game_engine_variant.assault.flag_hit_damage = (e_game_engine_weapon_hit)packet->read_integer("flag-hit-damage", k_game_engine_weapon_hit_bits_required);
		variant->game_engine_variant.assault.waypoint_type = (e_ctf_engine_enemy_bomb_waypoint_type)packet->read_integer("waypoint-to-home-flag-type", k_ctf_engine_enemy_bomb_waypoint_type_bits_required);
		variant->game_engine_variant.assault.game_type = (e_ctf_game_type)packet->read_integer("game-type", k_ctf_game_type_bits_required);
		break;
	}
	case _game_engine_type_none:
		return true; // native behavior??? why coop?

	default:
		return false;
	}

	// cartographer variant settings decoding

	variant->cartographer_settings.version = (e_cartographer_variant_settings_version)packet->read_integer("cart-version", 32);
	variant->cartographer_settings.flags.set_unsafe(packet->read_integer("cart-flags", k_cartographer_variant_flags_bits_required));
	variant->cartographer_settings.game_speed = (e_game_speed_modifier)packet->read_integer("cart-game-speed", k_game_speed_modifier_bits_required);
	variant->cartographer_settings.gravity = (e_game_gravity_modifier)packet->read_integer("cart-gravity", k_game_gravity_modifier_bits_required);
	variant->cartographer_settings.spawn_protection = (e_player_spawn_protection_timer)packet->read_integer("cart-spawn-protection", k_player_spawn_protection_timer_bits_required);

	bool result = game_variant_is_valid(variant);
	return result;
}