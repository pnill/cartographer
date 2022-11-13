#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"

#define SKULL_COUNT 16

enum class e_skulls : unsigned int
{
	_envy = 0,
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


namespace cheats
{
	bool __cdecl ice_cream_flavor_available(unsigned int skull);
	void __cdecl ice_cream_activation(unsigned int skull);

	// New Carto Functions
	bool* ice_cream_flavor_state();

	void ApplyHooks();
}