#include "stdafx.h"
#include "particle_model_definitions.h"

c_particle_system_definition* c_particle_model_definition_interface::get_attached_particle_system(int32 particle_system_index)
{
	return this->particle_model_definition.attached_particle_systems[particle_system_index];
}
