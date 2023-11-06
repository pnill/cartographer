#pragma once

enum e_simulation_action_update : uint32
{
	_simulation_action_update_grenade_count_bit = 22
};

void __cdecl simulation_action_object_update(datum unit_index, uint32 update_mask);

void __cdecl simulation_action_pickup_equipment(datum unit_datum_index, datum grenade_tag_index);
