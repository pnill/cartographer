#pragma once

#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_events.h"

#define k_simulation_entity_type_maximum_count 32
#define k_simulation_event_type_maximum_count 32

class c_simulation_type_collection
{
public:
	c_simulation_entity_definition* get_entity_definition(e_simulation_entity_type type) const;
	c_simulation_event_definition* get_event_definition(e_simulation_event_type type) const;

	void register_entity_definition(e_simulation_entity_type type, void** event_definition);
	void register_event_definition(e_simulation_event_type type, void** event_definition);

	static c_simulation_type_collection* get()
	{
		return *Memory::GetAddress<c_simulation_type_collection**>(0x5178E4, 0x520B74);
	}

private:
	int32 m_entity_type_count;
	c_simulation_entity_definition* m_entity_definitions[k_simulation_entity_type_maximum_count];
	int32 m_event_type_count;
	c_simulation_event_definition* m_event_definitions[k_simulation_event_type_maximum_count];
};
CHECK_STRUCT_SIZE(c_simulation_type_collection, 264);
