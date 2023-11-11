#include "stdafx.h"
#include "object_types.h"

#include "object_placement.h"


object_type_definition** get_object_type_definitions(void)
{
	return Memory::GetAddress<object_type_definition**>(0x41F560, 0x3C2A70);
}

object_type_definition* object_type_definition_get(e_object_type object_type)
{
	return get_object_type_definitions()[object_type];
}

object_type_definition* get_game_object_type_definition(datum object_datum)
{
	return object_type_definition_get(object_get_fast_unsafe(object_datum)->object_identifier.get_type());
}

void __cdecl object_type_adjust_placement(object_placement_data* placement_data)
{
	INVOKE(0x1863EA, 0x170486, object_type_adjust_placement, placement_data);
	return;
}

bool object_type_new(datum object_index, object_placement_data* placement_data, bool* some_bool)
{
	const object_type_definition* object_type = get_game_object_type_definition(object_index);
	bool object_created = true;

	for (uint8 i = 0; object_type->base_object_types[i]; i++)
	{
		object_new_t object_new = object_type->base_object_types[i]->object_new;
		if (object_new)
		{
			object_created &= object_new(object_index, placement_data, some_bool);
		}
	}
	return object_created;
}

void object_type_create_children(datum object_index)
{
	const object_type_definition* object_type = get_game_object_type_definition(object_index);

	for (uint8 i = 0; object_type->base_object_types[i]; ++i)
	{
		object_create_children_t object_create_children = object_type->base_object_types[i]->object_create_children;
		if (object_create_children)
		{
			object_create_children(object_index);
		}
	}
	return;
}

void object_type_delete(datum object_datum)
{
	const object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (uint8 i = 0; object_type->base_object_types[i]; i++)
	{
		object_delete_t object_delete = object_type->base_object_types[i]->object_delete;
		if (object_delete)
		{
			object_delete(object_datum);
		}
	}

	return;
}

bool object_type_compute_activation(datum object_datum, s_game_cluster_bit_vectors* cluster_activation, bool* a3)
{
	bool result = false;
	const object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (uint8 i = 0; object_type->base_object_types[i] && !result; i++)
	{
		object_compute_activation_t object_compute_activation = object_type->base_object_types[i]->object_compute_activation;
		if (object_compute_activation)
		{
			result = object_compute_activation(object_datum, cluster_activation, a3);
		}
	}

	return result;
}

void object_type_postprocess_node_matrices(datum object_datum, int32 node_count, real_matrix4x3* node_matracies)
{
	const object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (uint8 i = 0; object_type->base_object_types[i]; i++)
	{
		object_postprocess_node_matrices_t object_postprocess_node_matrices = object_type->base_object_types[i]->object_postprocess_node_matrices;
		if (object_postprocess_node_matrices)
		{
			object_postprocess_node_matrices(object_datum, node_count, node_matracies);
		}
	}
	return;
}
