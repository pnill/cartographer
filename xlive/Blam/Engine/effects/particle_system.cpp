#include "stdafx.h"
#include "particle_system.h"

#include "effects_definitions.h"
#include "particle_model_definitions.h"
#include "particle_system_definition.h"
#include "Blam/Engine/physics/breakable_surface_definitions.h"
#include "H2MOD/Tags/TagInterface.h"

c_particle_system_definition* c_particle_system::get_particle_system_definition() const
{
	blam_tag tag_type = tags::datum_to_instance(this->tag_index)->type;

	//if (this->tag_index == (datum)0xE7952D30 || this->tag_index == (datum)0xE79A2D35 || this->tag_index == (datum)0xE7982D33
	//	|| this->tag_index == (datum)0xEA482FE3)
	//{
	//	__debugbreak();
	//}
	if (tag_type.tag_type == blam_tag::tag_group_type::effect)
	{
		effect_definition* effect = (effect_definition*)tags::get_tag_fast(this->tag_index);
		return effect->events[this->effect_event_index]->particle_systems[this->event_particle_system_index];
	}
	if (tag_type.tag_type == blam_tag::tag_group_type::particlemodel)
	{
		c_particle_system_definition* parent_system_definition = this->parent_system->get_particle_system_definition();
		c_particle_model_definition_interface* system_interface = dynamic_cast<c_particle_model_definition_interface*>(parent_system_definition->get_particle_system_interface());
		return system_interface->get_attached_particle_system(this->event_particle_system_index);
	}
	if(tag_type.tag_type == blam_tag::tag_group_type::particle)
	{
		c_particle_system_definition* parent_system_definition = this->parent_system->get_particle_system_definition();
		c_particle_sprite_definition_interface* system_interface = dynamic_cast<c_particle_sprite_definition_interface*>(parent_system_definition->get_particle_system_interface());
		return system_interface->get_attached_particle_system(this->event_particle_system_index);
	}
	if (tag_type.tag_type == blam_tag::tag_group_type::breakablesurface)
	{
		s_breakable_surface_definition* breakable_surface = (s_breakable_surface_definition*)tags::get_tag_fast(this->tag_index);
		effect_definition* effect = (effect_definition*)tags::get_tag_fast(breakable_surface->effect.TagIndex);
		return effect->events[this->effect_event_index]->particle_systems[this->event_particle_system_index];
	}
	return nullptr;
}

bool c_particle_system::update(real32 delta_time)
{
	typedef bool(__thiscall* c_particle_system_update_t)(c_particle_system*, real32);
	c_particle_system_update_t function = Memory::GetAddress<c_particle_system_update_t>(0xC43F9, 0xB12E4);
	return function(this, delta_time);
}

void __cdecl c_particle_system::destroy(datum particle_system_index)
{
	INVOKE(0xC3C49, 0xB0B34, c_particle_system::destroy, particle_system_index);
}

s_data_array* get_particle_system_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4D83D4, 0x4FFEA8);
}

void __cdecl particle_syste_remove_from_effects_cache(datum effect_index, datum particle_system_index)
{
	INVOKE(0xA69EB, 0x98A6B, particle_syste_remove_from_effects_cache, effect_index, particle_system_index);
}

