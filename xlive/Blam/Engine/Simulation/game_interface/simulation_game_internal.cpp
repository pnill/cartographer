#include "stdafx.h"
#include "simulation_game_internal.h"

void __cdecl simulation_write_definition_index(char* name, bitstream* packet, datum index)
{
    INVOKE(0x1F4CC0, 0x1DFD59, simulation_write_definition_index, name, packet, index);
    return;
}

datum __cdecl simulation_read_definition_index(char* name, bitstream* packet)
{
    return INVOKE(0x1F4CE9, 0x1DFD82, simulation_read_definition_index, name, packet);
}
