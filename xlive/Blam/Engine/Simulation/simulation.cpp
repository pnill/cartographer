#include "stdafx.h"
#include "simulation.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "Blam/Engine/objects/objects.h"

bool simulation_query_object_is_predicted(const datum object_datum)
{
    return s_game_globals::game_is_predicted() && object_get_fast_unsafe(object_datum)->simulation_entity_index != DATUM_INDEX_NONE;
}
