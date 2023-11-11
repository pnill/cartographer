#pragma once
#include "stdafx.h"
#include "simulation.h"

#include "Blam/Engine/game/game.h"
#include "Blam/Engine/objects/objects.h"

bool simulation_query_object_is_predicted(datum object_datum)
{
    return game_is_predicted() && object_get_fast_unsafe(object_datum)->simulation_entity_index != NONE;
}