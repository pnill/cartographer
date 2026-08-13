#include "stdafx.h"
#include "particle_update.h"

#include "particle_system.h"

void particle_update(real32 delta)
{
	data_iterator particle_system_it;
	iterator_new(&particle_system_it, get_particle_system_table());
	c_particle_system* particle_system = (c_particle_system*)iterator_next(&particle_system_it);
	while (particle_system)
	{
		if (particle_system->first_particle_index == NONE && !c_particle_system::frame_advance(particle_system, delta))
		{
			if (particle_system->parent_effect_index != NONE)
			{
				particle_system_remove_from_effects_cache(particle_system->parent_effect_index, particle_system_it.index);
			}
			c_particle_system::destroy(particle_system_it.index);
		}

		particle_system = (c_particle_system*)iterator_next(&particle_system_it);
	}
}

void particle_update_apply_patches()
{
	apply_particle_system_patches();

	//PatchCall(Memory::GetAddress(0xAA636, 0xB1C5D), particles_update);
}
