#include "stdafx.h"
#include "particle_model_definitions.h"
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "H2MOD/Tags/TagInterface.h"

c_particle_system_definition* c_particle_model_definition_interface::get_attached_particle_system(int32 particle_system_index)
{
	return this->particle_model_definition->attached_particle_systems[particle_system_index];
}

effect_location_definition* c_particle_model_definition_interface::get_particle_definition_locations()
{
	return this->particle_model_definition->locations.begin();
}

size_t c_particle_model_definition_interface::get_particle_definition_locations_size()
{
	return this->particle_model_definition->locations.count;
}

bool c_particle_model_definition_interface::particle_is_v_mirrored_or_one_shot()
{
	return (this->particle_model_definition->flags >> 12) & 1;
}

bool c_particle_model_definition_interface::particle_is_one_shot()
{
	return (this->particle_model_definition->flags >> 8) & 1;
}
