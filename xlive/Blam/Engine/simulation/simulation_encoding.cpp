#include "stdafx.h"
#include "simulation_encoding.h"

void __cdecl simulation_player_update_encode(c_bitstream* stream, const simulation_player_update* player_update)
{
	INVOKE(0x1E06AB, 0x1C7B6B, simulation_player_update_encode, stream, player_update);
	return;
}

bool __cdecl simulation_player_update_decode(c_bitstream* stream, simulation_player_update* player_update)
{
	return INVOKE(0x1E078A, 0x1C7C4A, simulation_player_update_decode, stream, player_update);
}