#include "stdafx.h"
#include "simulation_gamestate_entities.h"

void simulation_gamestate_index_decode(c_bitstream* bitstream, int32* gamestate_index_out)
{
	uint32 entity_abs_index = bitstream->read_integer("entity-absolute-index", 10);
	uint8 seed = bitstream->read_integer("entity-seed", 4);

	*gamestate_index_out = entity_abs_index | (seed << 28);
	return;
}

void simulation_gamestate_index_decode_get_abs_entity_index(c_bitstream* bitstream, int32* gamestate_index_out, uint32* entity_abs_index)
{
	*entity_abs_index = bitstream->read_integer("entity-absolute-index", 10);
	uint8 seed = bitstream->read_integer("entity-seed", 4);

	*gamestate_index_out = *entity_abs_index | (seed << 28);
	return;
}

