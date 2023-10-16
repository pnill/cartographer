#include "stdafx.h"
#include "objects.h"

#include "object_definition.h"
#include "object_types.h"

#include "Blam/Engine/cache/cache_files.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/memory/bitstream.h"
#include "Blam/Engine/Simulation/game_interface/simulation_game_units.h"
#include "Blam/Engine/units/bipeds.h"

#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "Util/Hooks/Hook.h"

void* object_header_block_get(const datum object_datum, const object_header_block_reference* reference)
{
	return (void*)((char*)object_get_fast_unsafe(object_datum) + reference->offset);
}

void* object_header_block_get_with_count(const datum object_datum, const object_header_block_reference* reference, DWORD element_size, DWORD* element_count)
{
	void* block;
	if (reference->offset == -1)
	{
		block = 0;
		*element_count = 0;
	}
	else
	{
		block = (void*)object_header_block_get(object_datum, reference);
		*element_count = reference->size / element_size;
	}

	return block;
}

void __cdecl object_placement_data_new(object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, s_damage_owner* damage_owner)
{
	INVOKE(0x132163, 0x121033, object_placement_data_new, object_placement_data, object_definition_idx, object_owner_idx, damage_owner);
	return;
}

//Pass new placement data into Create_object_new
datum __cdecl object_new(object_placement_data* placement_data)
{
	/*
	datum result = NONE;

	if (!TEST_FLAG(placement_data->object_placement_flags, 0x10))
	{
		if (placement_data->tag_index == NONE) 
		{
			return result;
		}

		object_type_adjust_placement(placement_data);
	}
	*/
	return INVOKE(0x136CA7, 0x125B77, object_new, placement_data);
}

//Pass datum from new object into object to sync
void __cdecl simulation_action_object_create(datum object_idx)
{
	INVOKE(0x1B8D14, 0x1B2C44, simulation_action_object_create, object_idx);
	return;
}

void __cdecl object_delete(datum object_idx)
{
	INVOKE(0x136005, 0x124ED5, object_delete, object_idx);
	return;
}

real_matrix4x3* object_get_node_matrices(datum object_datum, DWORD* out_node_count)
{
	return (real_matrix4x3*)object_header_block_get_with_count(object_datum, &object_get_fast_unsafe(object_datum)->nodes_block, sizeof(real_matrix4x3), out_node_count);
}

int object_get_count()
{
	s_data_iterator object_it(get_objects_header());
	return object_it.get_data_count();
}

int object_count_from_iter()
{
	s_data_iterator object_it(get_objects_header());
	int count = 0;
	while (object_it.get_next_datum())
	{
		count++;
	}
	return count;
}

datum object_get_damage_owner(datum object_index)
{
	object_datum* object = object_try_and_get_and_verify_type(object_index, -1);
	return object->damage_owner_object_datum;
}
