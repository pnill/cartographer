#include "stdafx.h"
#include "game_variant.h"

#include "saved_game_files.h"
#include "game/game.h"
#include "game/game_engine_territories.h"
#include "game/game_time.h"
#include "networking/logic/network_session_interface.h"
#include "physics/physics_constants.h"
#include "text/text_group.h"
#include "text/unicode.h"

#define variant_setting_pin(v, min, max)((v) = PIN(v, min, max))

/* typedef */

typedef void(__cdecl* t_game_variant_create_default_new)(s_game_variant*, e_game_variant_description_index);
t_game_variant_create_default_new p_game_variant_create_default_new;

typedef bool(__cdecl* t_game_variant_validate)(s_game_variant*);
t_game_variant_validate p_game_variant_validate;

void game_variant_apply_patches()
{
	DETOUR_ATTACH(p_game_variant_create_default_new, Memory::GetAddress<t_game_variant_create_default_new>(0x5B33D, 0x3CF9D), game_variant_create_default_new);
	DETOUR_ATTACH(p_game_variant_validate, Memory::GetAddress<t_game_variant_validate>(0x5B720, 0x3D380), game_variant_cleanup);
}

s_game_variant* get_game_variant(void)
{
	return &game_options_get()->game_variant;
}

s_game_variant* __cdecl get_default_game_variant_by_name(wchar_t* name)
{
	// Server only function?
	if (Memory::g_memory_is_dedicated_server)
	{
		return INVOKE(0, 0x678E, get_default_game_variant_by_name, name);
	}
	else
		return nullptr;
}

void __cdecl game_variant_create_default_new(s_game_variant* variant, e_game_variant_description_index game_variant_type)
{
	if (game_variant_type < k_base_game_variant_description_count)
	{
		p_game_variant_create_default_new(variant, game_variant_type);
		//INVOKE(0x5B33D, 0x3CF9D, game_variant_create_default_new, variant, game_variant_type);
	}
	else
	{
		variant->description_index = game_variant_type;

		switch (game_variant_type)
		{
		case _game_variant_description_zombies:
		{
			break;
		}
		case _game_variant_description_headhunter:
		{

			variant->variant_game_engine_index = _game_engine_type_headhunter;
			variant->game_engine_flags.set_unsafe(variant->game_engine_flags.get_unsafe() & ~0xFFEu | 0xFFA);
			variant->round_setting = _game_engine_round_setting_1_round;
			variant->round_time_limit = 0;
			variant->join_in_progress_setting = _game_engine_join_in_progress_on;

			memset((int8*)&variant->max_players, 0, 12);

			variant->max_players = k_maximum_players;
			variant->max_living_players = k_maximum_players;
			variant->lives_per_round = 0;
			variant->maximum_allowable_teams = k_game_multiplayer_team_count;
			variant->game_engine_variant.head_hunter.hill_move_time = 0;
			variant->respawn_time = 5;
			variant->suicide_penalty = 5;
			variant->shield_setting = _game_engine_shield_normal;
			variant->team_score_setting = _game_engine_team_score_sum;
			variant->team_respawn_setting = _game_engine_team_respawn_inheritance;
			variant->betrayal_penalty = 10;
			variant->unk = 0;
			variant->game_engine_variant.head_hunter.speed_with_heads = _ctf_engine_player_speed_normal;
			variant->game_engine_variant.head_hunter.max_heads_carried = _headhunter_max_heads_carried_none;

			csmemset((int8*)&variant->vehicle_respawn_setting, 0, 12);

			break;
		}
		}
	}

	game_variant_cartographer_settings_default_new(variant);
}
bool __cdecl game_variant_cleanup(s_game_variant* variant)
{
	// todo: fix this, so it can just be the rewritten function its 1-1 rewrite but for some reason doesn't work.
	// all default saves just end up going under slayer??

	if (variant->variant_game_engine_index != _game_engine_type_headhunter)
	{
		bool result = p_game_variant_validate(variant);
		game_variant_cartographer_settings_validate(variant);
		return result;
	}

	s_game_variant base_variant{};

	memcpy(&base_variant, variant, sizeof(s_game_variant));

	variant->flags &= 1u;

	int32 raw_flags = variant->game_engine_flags.get_unsafe();

	raw_flags &= SHRT_MAX;

	variant->game_engine_flags.set_unsafe(raw_flags);

	variant_setting_pin(variant->round_setting, _game_engine_round_setting_1_round, _game_engine_round_setting_first_to_4_rounds);
	variant_setting_pin(variant->score_to_win_round, 0, SHRT_MAX);
	variant_setting_pin(variant->round_time_limit, 0, SHRT_MAX);
	variant_setting_pin(variant->join_in_progress_setting, _game_engine_join_in_progress_on, _game_engine_join_in_progress_off);
	variant_setting_pin(variant->max_players, 0, k_maximum_players);
	variant_setting_pin(variant->max_living_players, 0, k_maximum_players);
	variant_setting_pin(variant->lives_per_round, 0, SHRT_MAX);
	variant_setting_pin(variant->respawn_time, 0, SHRT_MAX);
	variant_setting_pin(variant->suicide_penalty, 0, SHRT_MAX);
	variant_setting_pin(variant->shield_setting, _game_engine_shield_normal, _game_engine_shield_supershield);
	variant_setting_pin(variant->team_score_setting, _game_engine_team_score_sum, _game_engine_team_score_max);
	variant_setting_pin(variant->team_respawn_setting, _game_engine_team_respawn_inheritance, _game_engine_team_respawn_standard);
	variant_setting_pin(variant->betrayal_penalty, 0, SHRT_MAX);
	variant_setting_pin(variant->maximum_allowable_teams, 0, k_game_multiplayer_team_count);
	variant_setting_pin(variant->vehicle_respawn_setting, _game_engine_respawn_setting_on, _game_engine_respawn_setting_double);
	variant_setting_pin(variant->primary_light_land_vehicle, _game_engine_light_land_vehicle_default, _game_engine_light_land_vehicle_none);
	variant_setting_pin(variant->secondary_light_land_vehicle, _game_engine_light_land_vehicle_default, _game_engine_light_land_vehicle_none);
	variant_setting_pin(variant->primary_heavy_land_vehicle, _game_engine_heavy_land_vehicle_default, _game_engine_heavy_land_vehicle_none);
	variant_setting_pin(variant->primary_flying_vehicle, _game_engine_flying_vehicle_default, _game_engine_flying_vehicle_none);
	variant_setting_pin(variant->secondary_heavy_land_vehicle, _game_engine_heavy_land_vehicle_default, _game_engine_heavy_land_vehicle_none);
	variant_setting_pin(variant->primary_turret_vehicle, _game_engine_turret_vehicle_default, _game_engine_turret_vehicle_none);
	variant_setting_pin(variant->secondary_turret_vehicle, _game_engine_turret_vehicle_default, _game_engine_turret_vehicle_none);
	variant_setting_pin(variant->weapon_set, _game_engine_weapon_set_default, _game_engine_weapon_set_random);
	variant_setting_pin(variant->weapon_respawn_setting, _game_engine_respawn_setting_on, _game_engine_respawn_setting_double);
	variant_setting_pin(variant->starting_equipment_primary, _game_engine_starting_weapon_default, _game_engine_starting_weapon_brute_plasma);
	variant_setting_pin(variant->starting_equipment_secondary, _game_engine_starting_weapon_default, _game_engine_starting_weapon_brute_plasma);

	e_game_engine_type previous_type = variant->variant_game_engine_index;
	variant_setting_pin(variant->variant_game_engine_index, _game_engine_type_ctf, k_game_engine_playable_types);

	if (previous_type == variant->variant_game_engine_index)
	{
		switch (variant->variant_game_engine_index)
		{
		case _game_engine_type_slayer:
		{
			variant->game_engine_variant.slayer.flags.set_unsafe(variant->game_engine_variant.slayer.flags.get_unsafe() & k_slayer_engine_clear_all_bits_mask);
			break;
		}
		case _game_engine_type_oddball:
		{
			variant->game_engine_variant.oddball.flags.set_unsafe(variant->game_engine_variant.oddball.flags.get_unsafe() & k_oddball_engine_clear_unused_bits_mask);
			variant_setting_pin(variant->game_engine_variant.oddball.ball_count, 0, k_game_engine_oddball_maximum_balls);
			variant_setting_pin(variant->game_engine_variant.oddball.ball_hit_damage, _game_engine_multiplayer_weapon_hit_extra_damage, _game_engine_multiplayer_weapon_hit_normal_damage);
			variant_setting_pin(variant->game_engine_variant.oddball.speed_with_ball, _oddball_engine_player_speed_slow, _oddball_engine_player_speed_fast);
			variant_setting_pin(variant->game_engine_variant.oddball.waypoint_to_ball, _oddball_waypoint_type_always_on, _oddball_waypoint_type_off);
			break;
		}
		case _game_engine_type_koth:
		{
			variant->game_engine_variant.king.flags.set_unsafe(variant->game_engine_variant.king.flags.get_unsafe() & k_king_engine_clear_unused_bits_mask);
			variant_setting_pin(variant->game_engine_variant.king.hill_move_time, 0, SHRT_MAX);
			break;
		}
		case _game_engine_type_juggernaut:
		{
			variant->game_engine_flags.set_unsafe(variant->game_engine_flags.get_unsafe() & ~1u);
			variant->game_engine_variant.juggernaut.flags.set_unsafe(variant->game_engine_variant.juggernaut.flags.get_unsafe() & k_juggernaut_engine_clear_unused_bits_mask);
			variant_setting_pin(variant->game_engine_variant.juggernaut.juggernaut_movement_speed, _ctf_engine_player_speed_slow, _ctf_engine_player_speed_fast);
			break;
		}
		case _game_engine_type_territories:
		{
			variant_setting_pin(variant->game_engine_variant.territories.territory_count, 1, k_maximum_territories_flags);
			variant_setting_pin(variant->game_engine_variant.territories.territory_contest_time, 0, 1);
			variant_setting_pin(variant->game_engine_variant.territories.territory_capture_time, 0, 2);
			break;
		}
		case _game_engine_type_assault:
		case _game_engine_type_ctf:
		{
			if (variant->variant_game_engine_index == _game_engine_type_assault)
			{
				variant_setting_pin(variant->game_engine_variant.assault.bomb_arming_time, 0, 15);
				variant_setting_pin(variant->game_engine_variant.assault.bomb_fuse_time, 0, 15);
			}

			variant->game_engine_variant.ctf.flags.set_unsafe(variant->game_engine_variant.ctf.flags.get_unsafe() | 1u);

			variant_setting_pin(variant->game_engine_variant.ctf.flag_reset_time, 0, SHRT_MAX);
			variant_setting_pin(variant->game_engine_variant.ctf.speed_with_flag, _ctf_engine_player_speed_slow, _ctf_engine_player_speed_fast);
			variant_setting_pin(variant->game_engine_variant.ctf.flag_hit_damage, _game_engine_multiplayer_weapon_hit_extra_damage, _game_engine_multiplayer_weapon_hit_normal_damage);
			variant_setting_pin(variant->game_engine_variant.ctf.waypoint_type, _ctf_engine_enemy_bomb_waypoint_always_on, _ctf_engine_enemy_bomb_waypoint_off);
			variant_setting_pin(variant->game_engine_variant.ctf.game_type, _ctf_game_type_multi_flag, _ctf_game_type_single_flag);
			break;
		}
		case _game_engine_type_headhunter:
		{
			break;
		}
		default:
		{
			variant->variant_game_engine_index = _game_engine_type_stub;
			break;
		}
		}
	}

	variant->unk = 0;

	bool k = validate_wchar_characters(variant->variant_name);
	if (!memcmp(&base_variant, variant, sizeof(s_game_variant)))
	{
		game_variant_cartographer_settings_validate(variant);
		return k;
	}

	memset(&base_variant, 0, sizeof(s_game_variant));

	const static wchar_t* label = L"FailedVariant";

	ustrncpy(base_variant.variant_name, label, NUMBEROF(base_variant.variant_name));

	base_variant.game_engine_flags.set_unsafe(base_variant.game_engine_flags.get_unsafe() & 0xFFFF8000 | 0xF9A);
	base_variant.max_players = 16;
	base_variant.max_living_players = 16;
	base_variant.respawn_time = 5;
	base_variant.suicide_penalty = 5;
	base_variant.variant_game_engine_index = _game_engine_type_slayer;
	base_variant.round_setting = _game_engine_round_setting_1_round;
	base_variant.join_in_progress_setting = _game_engine_join_in_progress_on;
	base_variant.lives_per_round = 0;
	base_variant.shield_setting = _game_engine_shield_normal;
	base_variant.team_score_setting = _game_engine_team_score_sum;
	base_variant.team_respawn_setting = _game_engine_team_respawn_standard;
	base_variant.unk = 0;
	base_variant.game_engine_variant.slayer.flags.set_unsafe(base_variant.game_engine_variant.slayer.flags.get_unsafe() & 0xFFFFFFF8 | 2);
	base_variant.description_index = k_game_variant_description_invalid;
	base_variant.score_to_win_round = 25;
	base_variant.round_time_limit = 480;
	base_variant.betrayal_penalty = 10;
	memset(&base_variant.vehicle_respawn_setting, 0, 12);


	memcpy(variant, &base_variant, sizeof(s_game_variant));
	return false;

	//return INVOKE(0x5B720, 0x3D380, game_variant_cleanup, variant);
}

bool game_variant_is_valid(s_game_variant* variant)
{
	s_game_variant temporary_variant{};
	csmemcpy(&temporary_variant, variant, sizeof(s_game_variant));

	return game_variant_cleanup(variant);
}

void game_variant_cartographer_settings_default_new(s_game_variant* variant)
{
	variant->cartographer_settings.version = _cartographer_variant_settings_version_one;
	variant->cartographer_settings.flags.set_unsafe(0);
	variant->cartographer_settings.gravity = _game_gravity_modifier_none;
	variant->cartographer_settings.game_speed = _game_speed_modifier_none;
	variant->cartographer_settings.spawn_protection = _player_spawn_protection_timer_one_second;


	variant->cartographer_settings.flags.set(_cartographer_variant_explosion_physics, true);
}

bool game_variant_cartographer_settings_validate(s_game_variant* variant)
{
	if (variant->cartographer_settings.version == _cartographer_variant_settings_version_none)
	{
		game_variant_cartographer_settings_default_new(variant);
	}

	// in the future this functionality needs to be expanded for upgrading between versions and verification.
	return true;
}

#undef variant_setting_pin