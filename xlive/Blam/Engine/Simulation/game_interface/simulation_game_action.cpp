#include "stdafx.h"
#include "simulation_game_action.h"

void __cdecl simulation_action_object_update(datum unit_index, uint32 update_mask)
{
	INVOKE(0x1B6685, 0x1B05B5, simulation_action_object_update, unit_index, update_mask);
	return;
}

void __cdecl simulation_action_pickup_equipment(datum unit_datum_index, datum grenade_tag_index)
{
	INVOKE(0x1B6F12, 0x1B0E42, simulation_action_pickup_equipment, unit_datum_index, grenade_tag_index);
	return;
}