#include "stdafx.h"
#include "cheats.h"
#include "Blam/Enums/HaloStrings.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

typedef bool(__cdecl* ice_cream_flavor_available_t)(unsigned int skull);
ice_cream_flavor_available_t p_ice_cream_flavor_available;

typedef void(__cdecl* ice_cream_activation_t)(unsigned int skull);
ice_cream_activation_t p_ice_cream_activation;

bool skull_enabled[SKULL_COUNT] = { 0 };
const string_id skull_string_ids[SKULL_COUNT - 1] =
{
	HS_UNLOCKED_EXTRA_1,
	HS_UNLOCKED_EXTRA_2,
	HS_UNLOCKED_EXTRA_3,
	HS_UNLOCKED_EXTRA_4,
	HS_UNLOCKED_EXTRA_5,
	HS_UNLOCKED_EXTRA_6,
	HS_UNLOCKED_EXTRA_7,
	HS_UNLOCKED_EXTRA_8,
	HS_UNLOCKED_EXTRA_9,
	HS_UNLOCKED_EXTRA_10,
	HS_UNLOCKED_EXTRA_11,
	HS_UNLOCKED_EXTRA_12,
	HS_UNLOCKED_EXTRA_13,
	HS_UNLOCKED_EXTRA_14,
	HS_UNLOCKED_EXTRA_15
};


namespace cheats
{
	bool __cdecl ice_cream_flavor_available(unsigned int skull)
	{
#ifdef GAMEREWRITE
		return skull < (unsigned int)e_skulls::_skulls_end && s_game_globals::game_is_campaign() && skull_enabled[skull];
#else
		return skull < (unsigned int)e_skulls::_skulls_end && s_game_globals::game_is_campaign() && Memory::GetAddress<bool*>(0x4D8320)[skull];
#endif
	}

	void __cdecl ice_cream_activation(unsigned int skull)
	{
		typedef void(__cdecl *hud_clear_messages_t)();	
		hud_clear_messages_t p_hud_clear_messages = Memory::GetAddress<hud_clear_messages_t>(0x22CE83, 0x206863);
		typedef int(__cdecl* local_user_index_get_first_valid_t)();
		local_user_index_get_first_valid_t p_local_user_index_get_first_valid = Memory::GetAddress<local_user_index_get_first_valid_t>(0x5343F, 0x5B973);
		typedef int(__cdecl* display_generic_hud_string_t)(int controller_index, string_id string_id);
		display_generic_hud_string_t p_display_generic_hud_string = Memory::GetAddress<display_generic_hud_string_t>(0x22DEA4, 0x206BB7);
		typedef int(__cdecl* fade_in_flash_t)(float r, float g, float b, __int16 ticks);
		fade_in_flash_t p_fade_in_flash = Memory::GetAddress<fade_in_flash_t>(0xA402C, 0x9628C);
		typedef int(__cdecl* unspatialized_impulse_sound_new_t)(datum sound_datum, float scale);
		unspatialized_impulse_sound_new_t p_unspatialized_impulse_sound_new = Memory::GetAddress<unspatialized_impulse_sound_new_t>(0x8836C, 0x7F173);

		int user_index;
		s_globals_group_definition* global_globals_tag = tags::get_matg_globals_ptr();
		const s_globals_group_definition::s_player_information_block* player_information_tagblock;
		datum sound_datum;
		string_id skull_string_id;

		if (skull <= 14 && !skull_enabled[skull])
		{
			skull_enabled[skull] = true;
			p_hud_clear_messages();
			skull_string_id = skull_string_ids[skull];
			user_index = p_local_user_index_get_first_valid();
			p_display_generic_hud_string(user_index, skull_string_id);
			p_fade_in_flash(1.0, 1.0, 1.0, 20);
			player_information_tagblock = global_globals_tag->player_information[0];
			sound_datum = player_information_tagblock->ice_cream.TagIndex;
			if (sound_datum != -1)
				p_unspatialized_impulse_sound_new(sound_datum, 1.0);
		}
	}

	// New Carto Functions
	bool* ice_cream_flavor_state()
	{
#ifdef GAMEREWRITE
		return skull_enabled;
#else
		return Memory::GetAddress<bool*>(0x4D8320);
#endif
	}

	void ApplyHooks()
	{
		DETOUR_ATTACH(p_ice_cream_flavor_available, Memory::GetAddress<ice_cream_flavor_available_t>(0xBD114, 0xAC2A2), ice_cream_flavor_available);
		DETOUR_ATTACH(p_ice_cream_activation, Memory::GetAddress<ice_cream_activation_t>(0xBD137, 0xAC2C5), ice_cream_activation);
	}
}