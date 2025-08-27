#include "stdafx.h"
#include "simulation_type_collection.h"

#include "game_interface/simulation_game_engine_headhunter.h"

/* typedefs */

typedef void(__cdecl* t_simulation_game_register_types)(c_simulation_type_collection*, uint32*, uint32*);
t_simulation_game_register_types p_simulation_game_register_types;

void __cdecl simulation_game_register_types(c_simulation_type_collection* collection, uint32* out_entity_definition_count, uint32* out_event_definition_count);

/* public code */

c_simulation_entity_definition* c_simulation_type_collection::get_entity_definition(e_simulation_entity_type entity_type) const
{
	return m_entity_definitions[entity_type];
}

c_simulation_event_definition* c_simulation_type_collection::get_event_definition(e_simulation_event_type event_type) const
{
	return m_event_definitions[event_type];
}

int32 c_simulation_type_collection::get_event_definition_count(void) const
{
	ASSERT(m_event_type_count >= 0);
	return m_event_type_count;
}

void c_simulation_type_collection::register_entity_definition(e_simulation_entity_type entity_type, c_simulation_entity_definition* definition)
{
	m_entity_definitions[entity_type] = definition;
	return;
}

void c_simulation_type_collection::register_event_definition(e_simulation_event_type type, c_simulation_event_definition* definition)
{
	m_event_definitions[type] = definition;
	return;
}

void simulation_type_collection_apply_patches()
{
	DETOUR_ATTACH(p_simulation_game_register_types, Memory::GetAddress<t_simulation_game_register_types>(0x1DAF44, 0x1C0653), simulation_game_register_types);
}

/* private code */

void __cdecl simulation_game_register_types(c_simulation_type_collection* collection, uint32* out_entity_definition_count, uint32* out_event_definition_count)
{
	p_simulation_game_register_types(collection, out_entity_definition_count, out_event_definition_count);

	collection->register_entity_definition(_simulation_entity_type_headhunter_engine_globals, get_global_headhunter_engine_globals_definition());

	*out_event_definition_count++;
}