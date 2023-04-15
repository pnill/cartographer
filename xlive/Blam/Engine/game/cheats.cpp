#include "stdafx.h"
#include "cheats.h"

#include "Blam/Enums/HaloStrings.h"
#include "Blam/Engine/game/game_globals.h"

#include "H2MOD/Tags/TagInterface.h"

#include "Util/Hooks/Hook.h"


typedef bool(__cdecl* ice_cream_flavor_available_t)(unsigned int skull);
ice_cream_flavor_available_t p_ice_cream_flavor_available;

typedef void(__cdecl* ice_cream_activation_t)(unsigned int skull);
ice_cream_activation_t p_ice_cream_activation;

bool skull_enabled[SKULL_COUNT] = { false };
const string_id skull_string_ids[SKULL_COUNT] =
{
	HS_UNLOCKED_EXTRA_1,	// Envy
	HS_UNLOCKED_EXTRA_2,	// Grunt Birthday Party
	HS_UNLOCKED_EXTRA_3,	// Assassins
	HS_UNLOCKED_EXTRA_4,	// Thunderstorm
	HS_UNLOCKED_EXTRA_5,	// Famine
	HS_UNLOCKED_EXTRA_6,	// I Would Have Been Your Daddy
	HS_UNLOCKED_EXTRA_7,	// Blind
	HS_UNLOCKED_EXTRA_8,	// Ghost
	HS_UNLOCKED_EXTRA_9,	// Black Eye
	HS_UNLOCKED_EXTRA_10,	// Catch
	HS_UNLOCKED_EXTRA_11,	// Sputnik
	HS_UNLOCKED_EXTRA_12,	// Iron
	HS_UNLOCKED_EXTRA_13,	// Mythic
	HS_UNLOCKED_EXTRA_14,	// Angry
	HS_UNLOCKED_EXTRA_16,	// That's Just... Wrong
	HS_UNLOCKED_EXTRA_15,	// Cow Bell
};



bool __cdecl ice_cream_flavor_available(const e_skull_type skull)
{
	return skull < SKULL_COUNT && s_game_globals::game_is_campaign() && skull_enabled[skull];
}

void __cdecl ice_cream_flavor_stock(const e_skull_type skull)
{
	typedef void(__cdecl* hud_clear_messages_t)();
	hud_clear_messages_t p_hud_clear_messages = Memory::GetAddress<hud_clear_messages_t>(0x22CE83, 0x206863);
	typedef int(__cdecl* players_first_active_user_t)();
	players_first_active_user_t p_players_first_active_user = Memory::GetAddress<players_first_active_user_t>(0x5343F, 0x5B973);
	typedef int(__cdecl* display_generic_hud_string_t)(int controller_index, string_id string_id);
	display_generic_hud_string_t p_display_generic_hud_string = Memory::GetAddress<display_generic_hud_string_t>(0x22DEA4, 0x206BB7);
	typedef int(__cdecl* scripted_player_effect_screen_fade_in_t)(float r, float g, float b, __int16 ticks);
	scripted_player_effect_screen_fade_in_t p_scripted_player_effect_screen_fade_in = Memory::GetAddress<scripted_player_effect_screen_fade_in_t>(0xA402C, 0x9628C);
	typedef int(__cdecl* unspatialized_impulse_sound_new_t)(datum sound_datum, float scale);
	unspatialized_impulse_sound_new_t p_unspatialized_impulse_sound_new = Memory::GetAddress<unspatialized_impulse_sound_new_t>(0x8836C, 0x7F173);

	int user_index;
	s_globals_group_definition* g_globals_tag = tags::get_matg_globals_ptr();
	const s_globals_group_definition::s_player_information_block* player_information_tagblock;
	datum sound_datum;
	string_id skull_string_id;

	if (skull < SKULL_COUNT && !skull_enabled[skull])
	{
		skull_enabled[skull] = true;
		p_hud_clear_messages();
		skull_string_id = skull_string_ids[skull];
		user_index = p_players_first_active_user();
		p_display_generic_hud_string(user_index, skull_string_id);
		p_scripted_player_effect_screen_fade_in(1.0, 1.0, 1.0, 20);

		// The below should never return true, if that's the case we've got bigger issues
		if (g_globals_tag->player_information.size == 0) 
		{
			LOG_CRITICAL_GAME("g_globals_tag->player_information.size == 0, something is wrong with the globals tag on this map");
			return; 
		}

		sound_datum = g_globals_tag->player_information[0]->ice_cream.TagIndex;
		if (sound_datum != DATUM_INDEX_NONE)
		{
			p_unspatialized_impulse_sound_new(sound_datum, 1.0);
		}
	}
}

bool* get_ice_cream_activation()
{
	return skull_enabled;
}

void apply_cheat_hooks()
{
	DETOUR_ATTACH(p_ice_cream_flavor_available, Memory::GetAddress<ice_cream_flavor_available_t>(0xBD114, 0xAC2A2), ice_cream_flavor_available);
	DETOUR_ATTACH(p_ice_cream_activation, Memory::GetAddress<ice_cream_activation_t>(0xBD137, 0xAC2C5), ice_cream_flavor_stock);
}
