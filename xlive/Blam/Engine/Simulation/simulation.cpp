#pragma once
#include "stdafx.h"
#include "simulation.h"

#include "Blam/Engine/game/game.h"
#include "Blam/Engine/objects/objects.h"

bool simulation_query_object_is_predicted(datum object_datum)
{
    return game_is_predicted() && object_get_fast_unsafe(object_datum)->simulation_entity_index != NONE;
}

void __cdecl simulation_process_input(uint32 player_action_mask, const player_action* player_actions)
{
    INVOKE(0x1ADDA9, 0x0, simulation_process_input, player_action_mask, player_actions);
    return;
}