#include "stdafx.h"
#include "particle_state.h"

void __cdecl particle_state_update(c_flags_no_init<e_particle_state_flags, uint32, k_particle_state_flags> flags, c_particle_system* particle_system, c_particle_location* particle_location, c_particle* particle, s_particle_state* particle_state, int32 state_values_count)
{
	return INVOKE(0x4FDD4, 0x56FD4, particle_state_update, flags, particle_system, particle_location, particle, particle_state, state_values_count);
}
