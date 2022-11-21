#pragma once

#include "Blam/Common/Common.h"
#include "Blam/Engine/Game/game/game.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"

#include "H2MOD/Variants/Variants.h"

enum e_sound_type
{
	_sound_type_team_change,
	_sound_type_gained_the_lead,
	_sound_type_lost_the_lead,
	_sound_type_tied_leader,
	_sound_type_slayer,
};
#define ALL_SOUNDS_NO_SLAYER (FLAG(_sound_type_team_change) | FLAG(_sound_type_gained_the_lead) | FLAG(_sound_type_lost_the_lead) | FLAG(_sound_type_tied_leader))

enum e_static_lod : DWORD
{
	disable = 0,
	super_low,
	low,
	medium,
	high,
	super_high,
	cinematic
};

class H2MOD
{
public:
	static void Initialize();
	static void Deinitialize();
	static void ApplyHooks();
	static void ApplyFirefightHooks();
	static void RegisterEvents();

	void team_player_indicator_visibility(bool toggle);
	void disable_sounds(int sound);
	void custom_sound_play(const wchar_t* soundName, int delay);
	void disable_weapon_pickup(bool b_Enable);

	void toggle_ai_multiplayer(bool toggle);
	void toggle_xbox_tickrate(s_game_options* game_options, bool toggle);

	bool drawTeamIndicators = true;
};

extern std::unique_ptr<H2MOD> h2mod;