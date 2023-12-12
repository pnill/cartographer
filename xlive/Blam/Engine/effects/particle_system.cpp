#include "stdafx.h"
#include "particle_system.h"

#include "effects_definitions.h"
#include "particle_system_definition.h"
#include "H2MOD/Tags/TagInterface.h"

c_particle_system_definition* c_particle_system::get_particle_system_definition()
{
	blam_tag tag_type = tags::datum_to_instance(this->tag_index)->type;

	if (tag_type.tag_type == blam_tag::tag_group_type::effect)
	{
		effect_definition* effect = (effect_definition*)tags::get_tag_fast(this->tag_index);
		return effect->events[this->effect_event_index]->particle_systems[this->event_particle_system_index];
	}
	if (tag_type.tag_type == blam_tag::tag_group_type::particlemodel)
	{
		c_particle_system_definition* parent_system_definition = this->parent_system->get_particle_system_definition();
		c_particle_definition_interface* system_interface = parent_system_definition->get_particle_system_interface();
		return system_interface->get_attached_particle_system(this->event_particle_system_index);
	}
	if (tag_type.tag_type == blam_tag::tag_group_type::breakablesurface)
	{

	}
	return nullptr;
}
