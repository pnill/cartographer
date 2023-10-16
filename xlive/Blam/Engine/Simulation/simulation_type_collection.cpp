#include "stdafx.h"
#include "simulation_type_collection.h"

c_simulation_entity_definition* c_simulation_type_collection::get_entity_definition(e_simulation_entity_type entity_type) const
{
	return this->m_entity_definitions[entity_type];
}
