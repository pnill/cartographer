#pragma once
#include "Blam\Common\Common.h"

struct c_simulation_globals_type_collection
{
	DWORD simulation_entity_count;
	void* simulation_entity_definition_array[32];
	DWORD simulation_event_count;
	void* simulation_event_definition_array[32];
};
CHECK_STRUCT_SIZE(c_simulation_globals_type_collection, 0x108);
