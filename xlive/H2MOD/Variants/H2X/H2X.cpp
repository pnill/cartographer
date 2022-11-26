#include "stdafx.h"

#include "H2X.h"

#include "Blam/Cache/TagGroups/sound_classes_definition.hpp"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"
#include "Blam/Engine/Game/game/game.h"
#include "Blam/Engine/Game/game/game_time.h"
#include "Blam/Engine/Game/math/real_math.h"
#include "H2MOD/Tags/TagInterface.h"

#include "Util/Hooks/Hook.h"

const std::vector<H2X::h2x_mod_info> weapons =
{
	{ "objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle", 0.535f, 0.5f, 0, false },
	{ "objects\\weapons\\rifle\\battle_rifle\\battle_rifle", 0.295f, 0.26f, 0, false },
	{ "objects\\weapons\\pistol\\magnum\\magnum", 0.13f, 0.1f, 0, false },
	{ "objects\\weapons\\rifle\\shotgun\\shotgun", 1.035f, 1.0f, 0, false },
	{ "objects\\weapons\\rifle\\beam_rifle\\beam_rifle", 0.25875f, 0.25f, 0, false },
	{ "objects\\weapons\\support_low\\brute_shot\\brute_shot", 0.39f, 0.3f, 0, false },
	{ "objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol", 0.11f, 0.05f, 0, false },
	{ "objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine", 0.20f, 0.14f, 1, false },
	{ "objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher", 0.85f, 0.8f, 0, false },
	{ "objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle", 8.5f, 9.0f, 0, true },
	{ "objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle", 10.0f, 11.0f, 0, true }
};

float __cdecl game_seconds_to_ticks_real_weapon_adjust(float s)
{
	// if the recovery time set in the tags isn't at least game's seconds per tick (i.e. 1/tickrate seconds) or a multiple of it,
	// it'll result in fractional ticks that cannot be handled that easily in a fixed time step, where a game tick advances a certain amount of seconds/milliseconds per game tick
	// e.g 0.14 seconds of carbine fire recovery time will result in 30 ticks * 0.14 recovery = 4.2 ticks of recovery time, with .2 fractional tick at 30hz 
	// and 8.4 with .4 fractional ticks at 60hz by using the same logic
	// causing differences between tickrates (in this case between 30 and 60)

	float tick_difference = 0.0f;
	if (trunc(s) != s)
		tick_difference = real_math::blam_max(time_globals::get_ticks_difference_real() - 1.0f, 0.0f);
	float seconds_to_ticks_adjusted = (float)time_globals::get()->tickrate * s + tick_difference;
	return seconds_to_ticks_adjusted;
}

void H2X::ApplyMapLoadPatches(bool enable)
{
	for (auto& weapon : weapons)
	{
		datum weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, weapon.tag_string);
		if (weapon_datum != DATUM_INDEX_NONE)
		{
			s_weapon_group_definition* weapon_tag = tags::get_tag_fast<s_weapon_group_definition>(weapon_datum);
			weapon.rounds_per_second_based ? 
				weapon_tag->barrels[weapon.barrel_data_block_index]->rounds_per_second_upper = (enable ? weapon.h2x_rate_of_fire : weapon.original_rate_of_fire)
			: weapon_tag->barrels[weapon.barrel_data_block_index]->fire_recovery_time = (enable ? weapon.h2x_rate_of_fire : weapon.original_rate_of_fire);
		}
	}

	if (enable && !Memory::IsDedicatedServer() && s_game_globals::game_is_multiplayer())
	{
		// Change sound_classes data to equivalents in original halo 2
		datum sound_classes_datum = tags::find_tag(blam_tag::tag_group_type::soundclasses, "sound\\sound_classes");
		if (sound_classes_datum != DATUM_INDEX_NONE)
		{
			s_sound_classes_block* sound_classes = tags::get_tag_fast<s_sound_classes_block>(sound_classes_datum);
			sound_classes->soundClasses[0]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[1]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[2]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[3]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[4]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[5]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[6]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[7]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[8]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[9]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[10]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[11]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[12]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[13]->gainBoundsDB = { -64.0f, -4.0f };
			sound_classes->soundClasses[14]->gainBoundsDB = { -12.0f, -4.0f };
			sound_classes->soundClasses[18]->gainBoundsDB = { -32.0f, -9.0f };
			sound_classes->soundClasses[20]->gainBoundsDB = { -0.0f, -2.0f };
			sound_classes->soundClasses[22]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[23]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[24]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[28]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[29]->gainBoundsDB = { -0.0f, -4.0f };
			sound_classes->soundClasses[34]->gainBoundsDB = { -0.0f, -4.0f };
		}				  				  
	}
}


void H2X::ApplyPatches()
{
	// PatchCall(Memory::GetAddress(0x15C4C3), game_seconds_to_ticks_real_weapon_adjust);
	// PatchCall(Memory::GetAddress(0x15C5B3), game_seconds_to_ticks_real_weapon_adjust);
}
