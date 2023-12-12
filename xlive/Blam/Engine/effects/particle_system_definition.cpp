#include "stdafx.h"
#include "particle_system_definition.h"

c_particle_definition_interface* c_particle_system_definition::get_particle_system_interface() const
{
	return get_particle_system_interface_from_tag_index(this->particle.TagIndex);
}

c_particle_system_definition* c_particle_sprite_definition_interface::get_attached_particle_system(int32 particle_system_index)
{
	return this->particle_definition->attached_particle_systems[particle_system_index];
}