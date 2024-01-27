#include "stdafx.h"
#include "simulation_gamestate_entities.h"

void simulation_gamestate_index_encode(c_bitstream* stream, datum object_index)
{
	stream->write_integer("gamestate-index-id", DATUM_INDEX_TO_IDENTIFIER(object_index), 16);
	stream->write_integer("gamestate-index-absolute", DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index), 11);
}

void simulation_gamestate_index_decode(c_bitstream* stream, datum* object_index)
{
	int32 object_index_id = stream->read_integer("gamestate-index-id", 16);
	int32 object_absolute_index = stream->read_integer("gamestate-index-absolute", 11);

	*object_index = DATUM_INDEX_NEW(object_absolute_index, object_index_id);
}