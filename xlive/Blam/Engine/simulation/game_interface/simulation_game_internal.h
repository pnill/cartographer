#pragma once
#include "Blam/Engine/memory/bitstream.h"

void __cdecl simulation_write_definition_index(char* name, bitstream* packet, datum index);

datum __cdecl simulation_read_definition_index(char* name, bitstream* packet);
