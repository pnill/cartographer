#include "stdafx.h"
#include "multiplayer_variant_settings_interface_definition.h"

#include "multiplayer_variant_interface_cartographer_strings.h"
#include "user_interface_widget_group.h"
#include "cache/cache_files.h"
#include "game/game_engine.h"
#include "game/game_globals.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "text/text_group.h"
#include "text/unicode.h"

/* private code */

static void multiplayer_variant_settings_interface_parse_cartographer_engine_mode(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
	*out_offset = offsetof(s_game_variant, cartographer_settings.flags);
	*(e_cartographer_variant_flags*)out_variable = _cartographer_variant_engine_mode;
}

static void multiplayer_variant_settings_interface_parse_cartographer_infinite_ammo(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
	*out_offset = offsetof(s_game_variant, cartographer_settings.flags);
	*(e_cartographer_variant_flags*)out_variable = _cartographer_variant_infinite_ammo;
}

static void multiplayer_variant_settings_interface_parse_cartographer_infinite_grenades(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
	*out_offset = offsetof(s_game_variant, cartographer_settings.flags);
	*(e_cartographer_variant_flags*)out_variable = _cartographer_variant_infinite_grenades;
}

static void multiplayer_variant_settings_interface_parse_cartographer_explosion_physics(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
	*out_offset = offsetof(s_game_variant, cartographer_settings.flags);
	*(e_cartographer_variant_flags*)out_variable = _cartographer_variant_explosion_physics;
}

static void multiplayer_variant_settings_interface_parse_cartographer_force_default_fov(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
	*out_offset = offsetof(s_game_variant, cartographer_settings.flags);
	*(e_cartographer_variant_flags*)out_variable = _cartographer_variant_force_default_fov;
}

static void multiplayer_variant_settings_interface_parse_cartographer_game_speed(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_int8;
	*out_offset = offsetof(s_game_variant, cartographer_settings.game_speed);
	*(real32*)out_variable = 1.f;
}

static void multiplayer_variant_settings_interface_parse_cartographer_gravity(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_int8;
	*out_offset = offsetof(s_game_variant, cartographer_settings.gravity);
	*(real32*)out_variable = 1.f;
}

static void multiplayer_variant_settings_interface_parse_cartographer_spawn_protection(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_int8;
	*out_offset = offsetof(s_game_variant, cartographer_settings.spawn_protection);
	*(real32*)out_variable = 1.f;
}

static void multiplayer_variant_settings_interface_parse_cartographer_dub_shot(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
	*out_offset = offsetof(s_game_variant, cartographer_settings.flags);
	*(e_cartographer_variant_flags*)out_variable = _cartographer_variant_disable_dub_shot;
}

static void multiplayer_variant_settings_interface_parse_cartographer_invincible_players(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
{
	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
	*out_offset = offsetof(s_game_variant, cartographer_settings.flags);
	*(e_cartographer_variant_flags*)out_variable = _cartographer_variant_invincible_players;
}
//static void multiplayer_variant_settings_interface_parse_headhunter_moving_bin(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
//{
//	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_int16;
//	*out_offset = offsetof(s_game_variant, game_engine_variant.head_hunter.hill_move_time);
//	*(real32*)out_variable = 1.f;
//}
//
//static void multiplayer_variant_settings_interface_parse_headhunter_point_multiplier(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
//{
//	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
//	*out_offset = offsetof(s_game_variant, game_engine_variant.head_hunter.flags);
//	*(uint32*)out_variable = _headhunter_engine_flag_point_multiplier;
//}
//
//static void multiplayer_variant_settings_interface_parse_headhunter_suicide_point_loss(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
//{
//	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
//	*out_offset = offsetof(s_game_variant, game_engine_variant.head_hunter.flags);
//	*(uint32*)out_variable = _headhunter_engine_flag_suicide_point_loss;
//}
//
//static void multiplayer_variant_settings_interface_parse_headhunter_death_point_loss(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
//{
//	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
//	*out_offset = offsetof(s_game_variant, game_engine_variant.head_hunter.flags);
//	*(uint32*)out_variable = _headhunter_engine_flag_death_point_loss;
//}
//
//static void multiplayer_variant_settings_interface_parse_headhunter_uncontested_bin(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
//{
//	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_flags_32;
//	*out_offset = offsetof(s_game_variant, game_engine_variant.head_hunter.flags);
//	*(uint32*)out_variable = _king_engine_uncontested_hill_to_score_bit;
//}
//
//static void multiplayer_variant_settings_interface_parse_headhunter_speed_with_heads(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
//{
//	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_int32;
//	*out_offset = offsetof(s_game_variant, game_engine_variant.head_hunter.speed_with_heads);
//	*(real32*)out_variable = 1.0f;
//}
//
//static void multiplayer_variant_settings_interface_parse_headhunter_max_heads_carried(e_multiplayer_variant_setting_interface_conversion_type* out_conversion_type, uint32* out_offset, void* out_variable)
//{
//	*out_conversion_type = _multiplayer_variant_setting_interface_conversion_type_int8;
//	*out_offset = offsetof(s_game_variant, game_engine_variant.head_hunter.max_heads_carried);
//	*(real32*)out_variable = 1.0f;
//}
//
//static void multiplayer_variant_settings_interface_get_variant_parameter_label_headhunter(int32 value, e_variant_setting_parameter_type type, wchar_t* out_string)
//{
//	const wchar_t* string = L"";
//
//	switch (type)
//	{
//	case _variant_setting_parameter_type_headhunter_moving_head_bin:
//	case _variant_setting_parameter_type_headhunter_point_multiplier:
//	case _variant_setting_parameter_type_headhunter_suicide_point_loss:
//	case _variant_setting_parameter_type_headhunter_death_point_loss:
//	case _variant_setting_parameter_type_headhunter_uncontested_bin:
//	{
//		string = g_multiplayer_variant_interface_bool_value_strings[get_current_language()][value];
//		break;
//	}
//	case _variant_setting_parameter_type_headhunter_speed_with_heads:
//	{
//		switch ((e_ctf_engine_player_speed)value)
//		{
//		case _ctf_engine_player_speed_normal:
//		case _ctf_engine_player_speed_fast:
//		case _ctf_engine_player_speed_slow:
//			string = g_multiplayer_variant_interface_headhunter_speed_with_head_strings[get_current_language()][value];
//			break;
//		}
//		break;
//	}
//	case _variant_setting_parameter_type_headhunter_max_heads_carried:
//	{
//		switch ((e_headhunter_max_heads_carried)value)
//		{
//		case _headhunter_max_heads_carried_none:
//		case _headhunter_max_heads_carried_one:
//		case _headhunter_max_heads_carried_five:
//		case _headhunter_max_heads_carried_ten:
//			string = g_multiplayer_variant_interface_headhunter_max_heads_carried_strings[get_current_language()][value];
//			break;
//		}
//		break;
//	}
//	}
//	usnprintf(out_string, 512, string);
//}

static int32 multiplayer_variant_settings_interface_get_variant_parameter_value_count_headhnter(e_variant_setting_parameter_type type)
{
	switch (type)
	{
	case _variant_setting_parameter_type_headhunter_moving_head_bin:
	case _variant_setting_parameter_type_headhunter_point_multiplier:
	case _variant_setting_parameter_type_headhunter_suicide_point_loss:
	case _variant_setting_parameter_type_headhunter_death_point_loss:
	case _variant_setting_parameter_type_headhunter_uncontested_bin:
	{
		return 2;
	}
	case _variant_setting_parameter_type_headhunter_speed_with_heads:
	{
		return k_ctf_engine_player_speed_count;
	}
	case _variant_setting_parameter_type_headhunter_max_heads_carried:
	{
		return k_headhunter_max_heads_carried_count;
	}
	}
	return NONE;
}

static void multiplayer_variant_settings_interface_get_variant_parameter_label_cartographer(int32 value, e_variant_setting_parameter_type type, wchar_t* out_string)
{
	const wchar_t* string = L"";

	switch (type)
	{
	case _variant_setting_parameter_type_cartographer_infinite_ammo:
	case _variant_setting_parameter_type_cartographer_infinite_grenades:
	case _variant_setting_parameter_type_cartographer_explosion_physics:
	case _variant_setting_parameter_type_cartographer_force_default_fov:
	case _variant_setting_parameter_type_cartographer_force_default_cross_hair_offset:
	case _variant_setting_parameter_type_cartographer_force_default_weapon_offsets:
	case _variant_setting_parameter_type_cartographer_disable_dub_shot:
	case _variant_setting_parameter_type_cartographer_invincible_players:
		string = g_multiplayer_variant_interface_bool_value_strings[get_current_language()][value];
		break;

	case _variant_setting_parameter_type_cartographer_engine_mode:
	{
		string = g_multiplayer_variant_interface_cartographer_engine_mode[get_current_language()][value];
		break;
	}


	case _variant_setting_parameter_type_cartographer_game_speed:
	{
		switch ((e_game_speed_modifier)value)
		{
		case _game_speed_modifier_none:
		case _game_speed_modifier_half:
		case _game_speed_modifier_hundred_fifty:
		case _game_speed_modifier_double:
		case _game_speed_modifier_ludicrous:
		{
			string = g_multiplayer_variant_interface_cartographer_game_speed_strings[get_current_language()][value];
			break;
		}
		}
		break;
	}

	case _variant_setting_parameter_type_cartographer_gravity:
	{
		switch ((e_game_gravity_modifier)value)
		{
		case _game_gravity_modifier_none:
		case _game_gravity_modifier_twenty_five_percent:
		case _game_gravity_modifier_fifty_percent:
		case _game_gravity_modifier_seventy_five_percent:
		case _game_gravity_modifier_hundred_twenty_five_percent:
		case _game_gravity_modifier_hundred_fifty_percent:
		case _game_gravity_modifier_hundred_seventy_five_percent:
		case _game_gravity_modifier_two_hundred:
		{
			string = g_multiplayer_variant_interface_cartographer_gravity_strings[get_current_language()][value];
			break;
		}
		}
		break;
	}

	case _variant_setting_parameter_type_cartographer_spawn_protection:
	{
		switch ((e_player_spawn_protection_timer)value)
		{
		case _player_spawn_protection_timer_none:
		case _player_spawn_protection_timer_one_second:
		case _player_spawn_protection_timer_three_seconds:
		case _player_spawn_protection_timer_five_seconds:
		case _player_spawn_protection_timer_ten_seconds:
		{
			string = g_multiplayer_variant_interface_cartographer_spawn_protection_strings[get_current_language()][value];
			break;
		}
		}
		break;
	}
	}

	usnprintf(out_string, 512, string);
}

static int32 multiplayer_variant_settings_interface_get_variant_parameter_value_count_cartographer(e_variant_setting_parameter_type type)
{
	switch (type)
	{
	case _variant_setting_parameter_type_cartographer_infinite_ammo:
	case _variant_setting_parameter_type_cartographer_infinite_grenades:
	case _variant_setting_parameter_type_cartographer_explosion_physics:
	case _variant_setting_parameter_type_cartographer_force_default_fov:
	case _variant_setting_parameter_type_cartographer_engine_mode:
	case _variant_setting_parameter_type_cartographer_force_default_cross_hair_offset:
	case _variant_setting_parameter_type_cartographer_force_default_weapon_offsets:
	case _variant_setting_parameter_type_cartographer_disable_dub_shot:
	case _variant_setting_parameter_type_cartographer_invincible_players:
		return 2;

	case _variant_setting_parameter_type_cartographer_game_speed:
		return k_game_speed_modifier_count;

	case _variant_setting_parameter_type_cartographer_gravity:
		return k_game_gravity_modifier_count;

	case _variant_setting_parameter_type_cartographer_spawn_protection:
		return k_player_spawn_protection_timer_count;
	}

	return NONE;
}


/* public code */

void multiplayer_variant_settings_interface_apply_patches()
{
	//WritePointer(Memory::GetAddress(0x464E04), &multiplayer_variant_settings_interface_parse_headhunter_moving_bin);
	//WritePointer(Memory::GetAddress(0x464E08), &multiplayer_variant_settings_interface_parse_headhunter_point_multiplier);
	//WritePointer(Memory::GetAddress(0x464E0C), &multiplayer_variant_settings_interface_parse_headhunter_suicide_point_loss);
	//WritePointer(Memory::GetAddress(0x464E10), &multiplayer_variant_settings_interface_parse_headhunter_death_point_loss);
	//WritePointer(Memory::GetAddress(0x464E14), &multiplayer_variant_settings_interface_parse_headhunter_uncontested_bin);
	//WritePointer(Memory::GetAddress(0x464E18), &multiplayer_variant_settings_interface_parse_headhunter_speed_with_heads);
	//WritePointer(Memory::GetAddress(0x464E1C), &multiplayer_variant_settings_interface_parse_headhunter_max_heads_carried);

	PatchCall(Memory::GetAddress(0x21138B), multiplayer_variant_settings_interface_get_variant_setting_string);
}

void multiplayer_variant_settings_interface_on_map_load()
{
	tag_injection_set_active_map(L"mainmenu");
	if (tag_injection_active_map_verified())
	{
		datum goof_datum = tag_injection_load(_tag_group_multiplayer_variant_settings_interface_definition, "multiplayer\\game_variant_settings\\multiplayer_game_settings", true);

		if (goof_datum != NONE)
		{
			tag_injection_inject();

			s_game_globals const* globals = scenario_get_game_globals();

			if (globals)
			{
				s_game_globals_interface_tag_references* interface_globals = ((s_game_globals_interface_tag_references*)tag_block_get_element_with_size(&globals->interface_tags, 0, sizeof(s_game_globals_interface_tag_references)));

				s_user_interface_tag_globals* interface_tag_globals = user_interface_tag_globals_get(interface_globals->multiplayer_ui_globals.index);

				interface_tag_globals->mp_variant_settings_ui.index = goof_datum;
				interface_tag_globals->mp_variant_settings_ui.group = _tag_group_multiplayer_variant_settings_interface_definition;
			}
		}
	}
}

s_variant_setting_edit_reference* __cdecl multiplayer_variant_settings_interface_get_category_reference(e_variant_setting_category_type category_type)
{
	switch (category_type)
	{
		// return the setting reference for king on headhunter game modes
	case _variant_setting_category_type_game_head_hunter:
		return INVOKE(0x23ACDC, 0, multiplayer_variant_settings_interface_get_category_reference, _variant_setting_category_type_game_king);

	case _variant_setting_category_type_quick_options_head_hunter:
		return INVOKE(0x23ACDC, 0, multiplayer_variant_settings_interface_get_category_reference, _variant_setting_category_type_quick_options_king);

	default:
		return INVOKE(0x23ACDC, 0, multiplayer_variant_settings_interface_get_category_reference, category_type);
	}
}

s_text_value_pair_definition* multiplayer_variant_settings_interface_get_text_value_pair_for_parameter(e_variant_setting_parameter_type parameter_type)
{
	return INVOKE(0x23ABCC, 0, multiplayer_variant_settings_interface_get_text_value_pair_for_parameter, parameter_type);
}

int32 __cdecl multiplayer_variant_settings_interface_get_variant_parameter_value(s_game_variant* variant, e_variant_setting_parameter_type type)
{
	if (IN_RANGE(type, 0, k_variant_setting_parameter_type_base_count))
	{
		return INVOKE(0x23AA54, 0, multiplayer_variant_settings_interface_get_variant_parameter_value, variant, type);
	}
	else
	{
		switch (type)
		{
		case _variant_setting_parameter_type_cartographer_engine_mode:
			return variant->cartographer_settings.flags.test(_cartographer_variant_engine_mode);
		case _variant_setting_parameter_type_cartographer_infinite_ammo:
			return variant->cartographer_settings.flags.test(_cartographer_variant_infinite_ammo);
		case _variant_setting_parameter_type_cartographer_infinite_grenades:
			return variant->cartographer_settings.flags.test(_cartographer_variant_infinite_grenades);
		case _variant_setting_parameter_type_cartographer_explosion_physics:
			return variant->cartographer_settings.flags.test(_cartographer_variant_explosion_physics);
		case _variant_setting_parameter_type_cartographer_force_default_fov:
			return variant->cartographer_settings.flags.test(_cartographer_variant_force_default_fov);
		case _variant_setting_parameter_type_cartographer_force_default_weapon_offsets:
			return variant->cartographer_settings.flags.test(_cartographer_variant_force_default_weapon_offsets);
		case _variant_setting_parameter_type_cartographer_force_default_cross_hair_offset:
			return variant->cartographer_settings.flags.test(_cartographer_variant_force_default_cross_hair_offset);
		case _variant_setting_parameter_type_cartographer_disable_dub_shot:
			return variant->cartographer_settings.flags.test(_cartographer_variant_disable_dub_shot);
		case _variant_setting_parameter_type_cartographer_invincible_players:
			return variant->cartographer_settings.flags.test(_cartographer_variant_invincible_players);
		case _variant_setting_parameter_type_cartographer_game_speed:
			return variant->cartographer_settings.game_speed;
		case _variant_setting_parameter_type_cartographer_gravity:
			return variant->cartographer_settings.gravity;
		case _variant_setting_parameter_type_cartographer_spawn_protection:
			return variant->cartographer_settings.spawn_protection;
		}
	}

	return 0;
}

void __cdecl multiplayer_variant_settings_interface_set_variant_parameter_value(s_game_variant* variant, e_variant_setting_parameter_type parameter_type, int32 value)
{
	if (IN_RANGE(parameter_type, 0, k_variant_setting_parameter_type_base_count))
	{
		INVOKE(0x23A8C2, 0, multiplayer_variant_settings_interface_set_variant_parameter_value, variant, parameter_type, value);
	}
	else
	{
		switch (parameter_type)
		{
		case _variant_setting_parameter_type_cartographer_engine_mode:
			variant->cartographer_settings.flags.set(_cartographer_variant_engine_mode, value);
			break;
		case _variant_setting_parameter_type_cartographer_infinite_ammo:
			variant->cartographer_settings.flags.set(_cartographer_variant_infinite_ammo, value);
			break;
		case _variant_setting_parameter_type_cartographer_infinite_grenades:
			variant->cartographer_settings.flags.set(_cartographer_variant_infinite_grenades, value);
			break;
		case _variant_setting_parameter_type_cartographer_explosion_physics:
			variant->cartographer_settings.flags.set(_cartographer_variant_explosion_physics, value);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_fov:
			variant->cartographer_settings.flags.set(_cartographer_variant_force_default_fov, value);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_weapon_offsets:
			variant->cartographer_settings.flags.set(_cartographer_variant_force_default_weapon_offsets, value);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_cross_hair_offset:
			variant->cartographer_settings.flags.set(_cartographer_variant_force_default_cross_hair_offset, value);
			break;
		case _variant_setting_parameter_type_cartographer_disable_dub_shot:
			variant->cartographer_settings.flags.set(_cartographer_variant_disable_dub_shot, value);
			break;
		case _variant_setting_parameter_type_cartographer_invincible_players:
			variant->cartographer_settings.flags.set(_cartographer_variant_invincible_players, value);
			break;
		case _variant_setting_parameter_type_cartographer_game_speed:
			variant->cartographer_settings.game_speed = (e_game_speed_modifier)value;
			break;
		case _variant_setting_parameter_type_cartographer_gravity:
			variant->cartographer_settings.gravity = (e_game_gravity_modifier)value;
			break;
		case _variant_setting_parameter_type_cartographer_spawn_protection:
			variant->cartographer_settings.spawn_protection = (e_player_spawn_protection_timer)value;
			break;
		}
	}
}

s_text_value_pair_reference_new* __cdecl multiplayer_variant_settings_interface_get_variant_parameter_label(s_text_value_pair_definition* text_value_pair, int32 value)
{
	return INVOKE(0x23AD57, 0, multiplayer_variant_settings_interface_get_variant_parameter_label, text_value_pair, value);
}

void multiplayer_variant_settings_interface_get_variant_setting_string(s_game_variant* variant, c_text_widget* widget, int32 setting_index, wchar_t* separator, wchar_t* buffer)
{
	e_variant_setting_category_type category = _variant_setting_category_type_none;
	switch (variant->variant_game_engine_index)
	{
	case _game_engine_type_ctf:
		category = _variant_setting_category_type_quick_options_ctf;
		break;
	case _game_engine_type_slayer:
		category = _variant_setting_category_type_quick_options_slayer;
		break;
	case _game_engine_type_oddball:
		category = _variant_setting_category_type_quick_options_oddball;
		break;
	case _game_engine_type_koth:
		category = _variant_setting_category_type_quick_options_king;
		break;
	case _game_engine_type_headhunter:
		category = _variant_setting_category_type_quick_options_head_hunter;
		break;
	case _game_engine_type_juggernaut:
		category = _variant_setting_category_type_quick_options_juggernaut;
		break;
	case _game_engine_type_territories:
		category = _variant_setting_category_type_quick_options_territories;
		break;
	case _game_engine_type_assault:
		category = _variant_setting_category_type_quick_options_assault;
		break;
	case _game_engine_type_race:
	default:
		DISPLAY_ASSERT("invalid game engine index provided");
		break;
	}

	switch (category)
	{
	case _variant_setting_category_type_quick_options_head_hunter:
	{
		s_variant_setting_edit_reference* slayer_reference = multiplayer_variant_settings_interface_get_category_reference(_variant_setting_category_type_quick_options_slayer);
		s_variant_setting_edit_reference* king_reference = multiplayer_variant_settings_interface_get_category_reference(_variant_setting_category_type_quick_options_king);

		if (slayer_reference && king_reference)
		{
			s_text_value_pair_definition* option;
			c_maximum_interface_text label_buffer;
			c_maximum_interface_text value_buffer;

			switch (setting_index)
			{
			case 0: // Score to Win Round
			{
				// use slayer reference parsing because the scoring for head hunter is not time based.
				option = text_value_pair_definition_get(slayer_reference->options[setting_index]->index);
				break;
			}
			case 4: // Max Heads Carried
			{
				option = nullptr;
				multiplayer_variant_settings_interface_get_custom_variant_parameter_label(variant, _variant_setting_parameter_type_headhunter_max_heads_carried, variant->game_engine_variant.head_hunter.max_heads_carried, value_buffer.get_buffer());
				multiplayer_variant_settings_interface_get_custom_variant_parameter_title(variant, _variant_setting_parameter_type_headhunter_max_heads_carried, label_buffer.get_buffer());
				break;
			}
			default:
			{
				option = text_value_pair_definition_get(king_reference->options[setting_index]->index);
				break;
			}
			}

			if (option)
			{
				int32 setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, option->parameter);
				s_text_value_pair_reference_new* setting_label = multiplayer_variant_settings_interface_get_variant_parameter_label(option, setting_value);

				string_list_get_normal_string(option->string_list.index, option->title_text, &label_buffer);

				if (setting_label)
					string_list_get_normal_string(option->string_list.index, setting_label->label_string, &value_buffer);
			}

			if (widget)
			{
				usnprintf(buffer, 512, L"%s%s%s", label_buffer.get_buffer(), separator, value_buffer.get_buffer());
				widget->set_text(buffer);
				widget->set_visible(true);
			}
		}
		break;
	}
	default:
	{
		s_variant_setting_edit_reference* reference = multiplayer_variant_settings_interface_get_category_reference(category);

		if (reference && setting_index >= 0 && setting_index < reference->options.count)
		{
			s_text_value_pair_definition* option = text_value_pair_definition_get(reference->options[setting_index]->index);

			c_maximum_interface_text label_buffer;
			c_maximum_interface_text value_buffer;

			if (option)
			{
				int32 setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, option->parameter);
				s_text_value_pair_reference_new* setting_label = multiplayer_variant_settings_interface_get_variant_parameter_label(option, setting_value);

				string_list_get_normal_string(option->string_list.index, option->title_text, &label_buffer);

				if (setting_label)
					string_list_get_normal_string(option->string_list.index, setting_label->label_string, &value_buffer);
			}
			if (widget)
			{
				if (ustrnlen(value_buffer.get_buffer(), 511) > 0)
				{
					usnprintf(buffer, 512, L"%s%s%s", label_buffer.get_buffer(), separator, value_buffer.get_buffer());
					widget->set_text(buffer);
					widget->set_visible(true);
				}
			}
		}
		break;
	}
	}
}

void multiplayer_variant_settings_interface_get_custom_variant_parameter_label(s_game_variant* variant, e_variant_setting_parameter_type type, int32 value, wchar_t* out_string)
{
	//if (variant)
	//{
	//	switch (variant->variant_game_engine_index)
	//	{
	//	case _game_engine_type_headhunter:
	//	{
	//		multiplayer_variant_settings_interface_get_variant_parameter_label_headhunter(value, type, out_string);
	//		return;
	//	}
	//	}
	//}

	if (IN_RANGE(type, k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
	{
		multiplayer_variant_settings_interface_get_variant_parameter_label_cartographer(value, type, out_string);
		return;
	}

	usnprintf(out_string, 512, L"");
}
void multiplayer_variant_settings_interface_get_custom_variant_parameter_title(s_game_variant* variant, int32 index, wchar_t* out_string)
{

	//switch (variant->variant_game_engine_index)
	//{
	//case _game_engine_type_headhunter:
	//{
	//	usnprintf(out_string, 512, g_multiplayer_variant_interface_headhunter_parameter_title_strings[get_current_language()][index]);
	//	return;
	//}
	//}


	usnprintf(out_string, 512, L"");
}

void multiplayer_variant_settings_interface_get_custom_variant_parameter_title(s_game_variant* variant, e_variant_setting_parameter_type type, wchar_t* out_string)
{
	//if (variant)
	//{
	//	switch (variant->variant_game_engine_index)
	//	{
	//	case _game_engine_type_headhunter:
	//	{
	//		for (int32 i = 0; i < k_multiplayer_variant_headhunter_parameter_count; ++i)
	//		{
	//			if (g_multiplayer_variant_interface_headhunter_parameter_types[i] == type)
	//			{
	//				usnprintf(out_string, 512, g_multiplayer_variant_interface_headhunter_parameter_title_strings[get_current_language()][i]);
	//				return;
	//			}
	//		}
	//	}
	//	}
	//}


	if (IN_RANGE(type, k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
	{
		switch (type)
		{
		case _variant_setting_parameter_type_cartographer_engine_mode:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][0]);
			break;
		case _variant_setting_parameter_type_cartographer_infinite_ammo:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][1]);
			break;
		case _variant_setting_parameter_type_cartographer_infinite_grenades:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][2]);
			break;
		case _variant_setting_parameter_type_cartographer_explosion_physics:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][3]);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_fov:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][4]);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_weapon_offsets:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][5]);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_cross_hair_offset:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][6]);
			break;
		case _variant_setting_parameter_type_cartographer_game_speed:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][7]);
			break;
		case _variant_setting_parameter_type_cartographer_gravity:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][8]);
			break;
		case _variant_setting_parameter_type_cartographer_spawn_protection:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][9]);
			break;
		case _variant_setting_parameter_type_cartographer_disable_dub_shot:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][10]);
			break;
		case _variant_setting_parameter_type_cartographer_invincible_players:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_title_strings[get_current_language()][11]);
			break;
		}

		return;
	}

	usnprintf(out_string, 512, L"");
}

void multiplayer_variant_settings_interface_get_custom_variant_parameter_description(s_game_variant* variant, e_variant_setting_parameter_type type, wchar_t* out_string)
{
	//if (variant)
	//{
	//	switch (variant->variant_game_engine_index)
	//	{
	//	case _game_engine_type_headhunter:
	//	{
	//		for (int32 i = 0; i < k_multiplayer_variant_headhunter_parameter_count; ++i)
	//		{
	//			if (g_multiplayer_variant_interface_headhunter_parameter_types[i] == type)
	//			{
	//				usnprintf(out_string, 512, g_multiplayer_variant_interface_headhunter_parameter_description_strings[get_current_language()][i]);
	//				return;
	//			}
	//		}
	//	}
	//	}
	//}

	if (IN_RANGE(type, k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
	{
		switch (type)
		{
		case _variant_setting_parameter_type_cartographer_engine_mode:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][0]);
			break;
		case _variant_setting_parameter_type_cartographer_infinite_ammo:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][1]);
			break;
		case _variant_setting_parameter_type_cartographer_infinite_grenades:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][2]);
			break;
		case _variant_setting_parameter_type_cartographer_explosion_physics:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][3]);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_fov:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][4]);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_weapon_offsets:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][5]);
			break;
		case _variant_setting_parameter_type_cartographer_force_default_cross_hair_offset:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][6]);
			break;
		case _variant_setting_parameter_type_cartographer_game_speed:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][7]);
			break;
		case _variant_setting_parameter_type_cartographer_gravity:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][8]);
			break;
		case _variant_setting_parameter_type_cartographer_spawn_protection:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][9]);
			break;
		case _variant_setting_parameter_type_cartographer_disable_dub_shot:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][10]);
			break;
		case _variant_setting_parameter_type_cartographer_invincible_players:
			usnprintf(out_string, 512, g_multiplayer_variant_interface_cartographer_parameter_description_strings[get_current_language()][10]);
			break;
		}

		return;
	}

	usnprintf(out_string, 512, L"");
}

int32 multiplayer_variant_settings_interface_get_custom_variant_parameter_value_count(s_game_variant* variant, e_variant_setting_parameter_type type)
{
	if (variant)
	{
		switch (variant->variant_game_engine_index)
		{
		case _game_engine_type_headhunter:
			return multiplayer_variant_settings_interface_get_variant_parameter_value_count_headhnter(type);
		}
	}

	if (IN_RANGE(type, k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
	{
		return multiplayer_variant_settings_interface_get_variant_parameter_value_count_cartographer(type);
	}

	return 1;
}

bool multiplayer_variant_settings_interface_parameter_is_custom(s_game_variant* variant, e_variant_setting_parameter_type type)
{
	switch (variant->variant_game_engine_index)
	{
	case _game_engine_type_headhunter:
	{
		for (const auto headhunter_parameter_type : g_multiplayer_variant_interface_headhunter_parameter_types)
			if (headhunter_parameter_type == type)
			{
				// todo: construct a way to do this that doesn't require manually checking for these two types
				if (headhunter_parameter_type != _variant_setting_parameter_type_king_moving_hill && headhunter_parameter_type != _variant_setting_parameter_type_king_uncontested_hill)
					return true;
			}
	}
	}

	if (IN_RANGE(type, k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
	{
		return true;
	}

	return false;
}