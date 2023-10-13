#include "stdafx.h"
#include "simulation_game_objects.h"

#include "Blam/Engine/objects/objects.h"

void c_simulation_object_entity_definition__object_build_creation_data(datum object_index, s_simulation_object_creation_data* a2)
{
	object_datum* object = object_get_fast_unsafe<object_datum>(object_index);
	a2->object_definition_index = object->tag_definition_index;
	a2->object_scenario_datum_index = object->placement_index;
	a2->multiplayer_spawn_monitor_index = object->netgame_equipment_index;
	*(uint32*)&a2->emblem_info_foreground_index = object->foreground_emblem;
}