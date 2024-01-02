#include "stdafx.h"
#include "simulation_game_entities.h"

#include "objects/objects.h"

datum simulation_entity_index_to_object_index(int32 entity_index)
{
	// ### TODO OFFSET
	return INVOKE(0x1F2211, 0x0, simulation_entity_index_to_object_index, entity_index);
}

bool simulation_object_index_valid(datum object_index)
{
	bool result = object_try_and_get_and_verify_type(object_index, -1);
	return result;
}

void simulation_entity_indices_to_object_index(int32* entity_references, int32 entity_reference_count, datum* out_object_index, int32 out_object_index_count)
{
	for (int32 i = 0; i < entity_reference_count && i < out_object_index_count; i++)
	{
		out_object_index[i] = simulation_entity_index_to_object_index(entity_references[i]);

		if (!simulation_object_index_valid(out_object_index[i]))
		{
			// DEBUG
		}
	}
}
