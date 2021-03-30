#pragma once

#include "Blam/Engine/DataArray/DataArray.h"

struct s_game_state_objects
{
    static s_datum_array* getArray();
    static bool indexValid(datum objectDatum);
    static void* getObject(datum objectDatum);
};