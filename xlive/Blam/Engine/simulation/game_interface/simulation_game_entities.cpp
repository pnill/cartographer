#include "stdafx.h"
#include "simulation_game_entities.h"

#include "simulation_game_objects.h"

#include "objects/objects.h"

bool simulation_object_index_valid(datum object_index)
{
	bool result = object_try_and_get_and_verify_type(object_index, NONE);
	return result;
}

void simulation_entity_indices_to_object_index(int32* entity_references, int32 entity_reference_count, datum* out_object_index, int32 out_object_index_count)
{
	int32 i;
	for (i = 0; i < entity_reference_count && i < out_object_index_count; i++)
	{
		out_object_index[i] = simulation_object_get_replicated_object_from_entity(entity_references[i]);

		if (!simulation_object_index_valid(out_object_index[i]))
		{
			// DEBUG
		}
	}

	// set the rest to none, in case we have less entity references
	for (; i < out_object_index_count; i++)
	{
		out_object_index[i] = DATUM_INDEX_NONE;
	}
}
