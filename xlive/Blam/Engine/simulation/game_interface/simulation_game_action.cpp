#include "stdafx.h"
#include "simulation_game_action.h"

#include "game/game.h"
#include "game/game_engine_simulation.h"
#include "simulation/simulation.h"

//Pass datum from new object into object to sync
void __cdecl simulation_action_object_create(datum object_index)
{
	INVOKE(0x1B8D14, 0x1B2C44, simulation_action_object_create, object_index);
	return;
}

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

void __cdecl simulation_action_game_engine_player_update(datum player_index, uint32 update_mask)
{
	INVOKE(0x1B6662, 0x1B0592, simulation_action_game_engine_player_update, player_index, update_mask);
	return;
}

void __cdecl simulation_action_game_engine_player_create(int16 player_index)
{
	if (game_is_server() && game_is_distributed())
	{
		c_simulation_world* world = simulation_get_world();
		int32 entity_index = simulation_entity_create(6, NONE);
		if (entity_index != NONE)
		{
			game_engine_globals_set_player_entity_index(player_index, entity_index);
			world->get_distributed_world()->m_entity_database.entity_capture_creation_data(entity_index);
		}
	}
}

void simulation_game_action_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x6FF5B, 0x6EB53), simulation_action_game_engine_player_create);
	PatchCall(Memory::GetAddress(0x75148, 0x72240), simulation_action_game_engine_player_create);
	return;
}