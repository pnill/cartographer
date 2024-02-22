#include "stdafx.h"
#include "simulation_type_collection.h"

c_simulation_entity_definition* c_simulation_type_collection::get_entity_definition(e_simulation_entity_type entity_type) const
{
	return m_entity_definitions[entity_type];
}

c_simulation_event_definition* c_simulation_type_collection::get_event_definition(e_simulation_event_type event_type) const
{
	return m_event_definitions[event_type];
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
