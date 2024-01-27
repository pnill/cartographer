#pragma once

#include "memory/bitstream.h"

void simulation_gamestate_index_encode(c_bitstream* stream, datum object_index);
void simulation_gamestate_index_decode(c_bitstream* stream, datum* object_index);

void simulation_entity_index_encode(c_bitstream* stream, int32 entity_index);
void simulation_entity_index_decode(c_bitstream* stream, int32* entity_index);
