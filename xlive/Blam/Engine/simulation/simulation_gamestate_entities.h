#pragma once
#include "memory/bitstream.h"

// Get the gamestate index
void simulation_gamestate_index_decode(c_bitstream* bitstream, int32* gamestate_index_out);

// Get the gamestate index as well as the entity abs index
void simulation_gamestate_index_decode_get_abs_entity_index(c_bitstream* bitstream, int32* gamestate_index_out, uint32* entity_abs_index);
