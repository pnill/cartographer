#pragma once

#include "memory/bitstream.h"

void simulation_gamestate_index_encode(c_bitstream* stream, datum object_index);
void simulation_gamestate_index_decode(c_bitstream* stream, datum* object_index);
