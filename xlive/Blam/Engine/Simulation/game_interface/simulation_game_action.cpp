#include "stdafx.h"
#include "simulation_game_action.h"

//Pass datum from new object into object to sync
void __cdecl simulation_action_object_create(datum object_index)
{
	INVOKE(0x1B8D14, 0x1B2C44, simulation_action_object_create, object_index);
	return;
}