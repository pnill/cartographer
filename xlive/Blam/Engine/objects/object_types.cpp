#include "stdafx.h"
#include "object_types.h"

#include "objects.h"

#include "structures/cluster_partitions.h"

/* public code */

object_type_definition** get_object_type_definitions(void)
{
	return Memory::GetAddress<object_type_definition**>(0x41F560, 0x3C2A70);
}

object_type_definition* object_type_definition_get(e_object_type object_type)
{
	object_type_definition** object_type_definitions = get_object_type_definitions();

	vassert(
		VALID_INDEX(object_type, k_object_types_count),
		"#%d isn't a valid object type in [#0,#%d)",
		object_type,
		k_object_types_count
	);
	ASSERT(object_type_definitions[object_type]);
	ASSERT(object_type_definitions[object_type]->group_tag);

	return object_type_definitions[object_type];
}

void __cdecl object_types_initialize_for_new_map(void)
{
	INVOKE(0x185A9C, 0x16FB38, object_types_initialize_for_new_map);
	return;
}

void __cdecl object_type_adjust_placement(object_placement_data* placement_data)
{
	INVOKE(0x1863EA, 0x170486, object_type_adjust_placement, placement_data);
	return;
}

bool object_type_new(datum object_index, object_placement_data* placement_data, bool* a3)
{
	bool object_created = true;

	const object_datum* object = object_get(object_index);
	const object_type_definition* object_type = object_type_definition_get(object->object.object_identifier.get_type());

	for (uint8 i = 0; object_type->part_definitions[i]; i++)
	{
		object_new_t p_object_new = object_type->part_definitions[i]->object_new;
		if (p_object_new)
		{
			object_created = object_created && p_object_new(object_index, placement_data, a3);
		}
	}
	return object_created;
}

void object_type_create_children(datum object_index)
{
	const object_datum* object = object_get(object_index);
	const object_type_definition* object_type = object_type_definition_get(object->object.object_identifier.get_type());

	for (uint8 i = 0; object_type->part_definitions[i]; ++i)
	{
		object_create_children_t object_create_children = object_type->part_definitions[i]->object_create_children;
		if (object_create_children)
		{
			object_create_children(object_index);
		}
	}
	return;
}

void object_type_delete(datum object_index)
{
	const object_datum* object = object_get(object_index);
	const object_type_definition* object_type = object_type_definition_get(object->object.object_identifier.get_type());

	for (uint8 i = 0; object_type->part_definitions[i]; i++)
	{
		object_delete_t object_delete = object_type->part_definitions[i]->object_delete;
		if (object_delete)
		{
			object_delete(object_index);
		}
	}

	return;
}

bool object_type_compute_activation(datum object_index, s_game_cluster_bit_vectors* cluster_activation, bool* a3)
{
	bool result = false;

	const object_datum* object = object_get(object_index);
	const object_type_definition* object_type = object_type_definition_get(object->object.object_identifier.get_type());

	for (uint8 i = 0; object_type->part_definitions[i] && !result; i++)
	{
		object_compute_activation_t object_compute_activation = object_type->part_definitions[i]->object_compute_activation;
		if (object_compute_activation)
		{
			result = object_compute_activation(object_index, cluster_activation, a3);
		}
	}

	return result;
}

void object_type_postprocess_node_matrices(datum object_index, int32 node_count, real_matrix4x3* node_matracies)
{
	const object_datum* object = object_get(object_index);
	const object_type_definition* object_type = object_type_definition_get(object->object.object_identifier.get_type());

	for (uint8 i = 0; object_type->part_definitions[i]; i++)
	{
		object_postprocess_node_matrices_t object_postprocess_node_matrices = object_type->part_definitions[i]->object_postprocess_node_matrices;
		if (object_postprocess_node_matrices)
		{
			object_postprocess_node_matrices(object_index, node_count, node_matracies);
		}
	}
	return;
}

const char* object_type_get_name(e_object_type object_type)
{
	vassert(VALID_INDEX(object_type, k_object_types_count), "#%d isn't a valid object type in [#0,#%d)", object_type, k_object_types_count);

	object_type_definition** object_type_definitions = get_object_type_definitions();
	ASSERT(object_type_definitions[object_type]);

	return object_type_definitions[object_type]->name;
}
