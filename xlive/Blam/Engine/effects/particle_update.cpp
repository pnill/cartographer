#include "stdafx.h"
#include "particle_update.h"

#include "particle.h"
#include "particle_system.h"
#include "particle_emitter.h"


void particles_update(real32 delta)
{
	s_data_iterator<c_particle_system> particle_system_it(get_particle_system_table());
	while (particle_system_it.get_next_datum())
	{
		c_particle_system* particle_system = particle_system_it.get_current_datum();
		if (particle_system->first_particle_index == NONE && !c_particle_system::frame_advance(particle_system, delta))
		{
			if (particle_system->parent_effect_index != NONE)
			{
				particle_system_remove_from_effects_cache(particle_system->parent_effect_index, particle_system_it.get_current_datum_index());
			}
			c_particle_system::destroy(particle_system_it.get_current_datum_index());
		}
	}
}

void apply_particle_update_patches()
{
	apply_particle_system_patches();

	//PatchCall(Memory::GetAddress(0xAA636, 0xB1C5D), particles_update);
}
