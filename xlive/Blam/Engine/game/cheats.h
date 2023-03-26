#pragma once

#include "Blam/Engine/game/game_globals.h"

enum e_skulls
{
	_envy,
	_grunt_birthday_party,
	_assassians,
	_thunderstorm,
	_famine,
	_IWHBYD,
	_blind,
	_ghost,
	_black_eye,
	_catch,
	_sputnik,
	_iron,
	_mythic,
	_anger,
	_whuppopotamus,

	_skulls_end
};

static bool ice_cream_flavor_available(e_skulls skull)
{
	if (skull <= _skulls_end && s_game_globals::game_is_campaign())
		return Memory::GetAddress<bool*>(0x4D8320)[skull];

	return false;
}

static bool* ice_cream_flavor_state()
{
	return Memory::GetAddress<bool*>(0x4D8320);
}
