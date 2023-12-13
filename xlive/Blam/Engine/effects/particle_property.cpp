#include "stdafx.h"
#include "particle_property.h"
#include "particle_state.h"

real32 c_particle_property::get_result(s_particle_state* particle_state)
{
	typedef real32(__thiscall* get_particle_property_result_t)(c_particle_property*, s_particle_state*);
	auto function = Memory::GetAddress<get_particle_property_result_t>(0xC32DE, 0xCBDB6);
	return function(this, particle_state);
}
