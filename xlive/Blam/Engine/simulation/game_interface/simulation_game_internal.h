#pragma once
#include "memory/bitstream.h"

void __cdecl simulation_write_definition_index(char* name, c_bitstream* packet, datum index);

datum __cdecl simulation_read_definition_index(char* name, c_bitstream* packet);
