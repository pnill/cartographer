#include "stdafx.h"
#include "simulation.h"

#include "game/game.h"
#include "objects/objects.h"
#include "simulation_world.h"

c_simulation_world* simulation_get_world()
{
	// ### TODO offset
	return *Memory::GetAddress<c_simulation_world**>(0x5178DC, 0x0);
}

bool simulation_query_object_is_predicted(datum object_datum)
{
    return game_is_predicted() && object_get_fast_unsafe(object_datum)->simulation_entity_index != NONE;
}

void __cdecl simulation_process_input(uint32 player_action_mask, const player_action* player_actions)
{
    INVOKE(0x1ADDA9, 0x0, simulation_process_input, player_action_mask, player_actions);
    return;
}

c_simulation_type_collection* simulation_get_type_collection()
{
    return c_simulation_type_collection::get();
}