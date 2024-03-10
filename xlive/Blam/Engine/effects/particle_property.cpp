#include "stdafx.h"
#include "particle_property.h"
#include "particle_state.h"

real32 c_particle_property::get_result(s_particle_state* particle_state)
{
	return INVOKE_TYPE(0xC32DE, 0xCBDB6, real32(__thiscall*)(c_particle_property*, s_particle_state*), this, particle_state);
}
