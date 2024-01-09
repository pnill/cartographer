#include "stdafx.h"
#include "simulation_type_collection.h"

c_simulation_entity_definition* c_simulation_type_collection::get_entity_definition(e_simulation_entity_type entity_type)
{
	return m_entity_definitions[entity_type];
}

c_simulation_event_definition* c_simulation_type_collection::get_event_definition(e_simulation_event_type event_type)
{
	return m_event_definitions[event_type];
}