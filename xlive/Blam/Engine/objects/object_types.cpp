#include "stdafx.h"
#include "object_types.h"
#include "objects.h"

#include "H2MOD/Tags/TagInterface.h"

object_type_definition** get_object_type_definitions()
{
	return Memory::GetAddress<object_type_definition**>(0x41F560, 0x0);
}

object_type_definition* object_type_definition_get(e_object_type object_type)
{
	return get_object_type_definitions()[object_type];
}

object_type_definition* get_game_object_type_definition(datum object_datum)
{
	return object_type_definition_get((e_object_type)object_get_fast_unsafe<s_object_data_definition>(object_datum)->placement_info.object_type);
}

int object_type_from_group_tag(datum tag_index)
{
	int result;

	result = 0;
	while (get_object_type_definitions()[result]->group_tag != tags::get_tag_group_fast(tag_index))
	{
		if (++result >= 13)
			return -1;
	}
	return result;
}

bool object_type_new(datum object_datum, object_placement_data* placement_data, bool* some_bool)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);
	bool object_created = true;

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->object_new)
		{
			object_created = object_created && object_type->base_object_types[i]->object_new(object_datum, placement_data, some_bool);
		}
	}
	return object_created;
}

void object_type_adjust_placement(object_placement_data* placement_data)
{
	object_type_definition* object_type = get_object_type_definitions()[object_type_from_group_tag(placement_data->tag_index)];

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->adjust_placement)
		{
			object_type->base_object_types[i]->adjust_placement(placement_data);
		}
	}
}

void object_type_dispose_from_old_structure_bsp(datum object_datum)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->dispose_from_old_structure_bsp)
		{
			object_type->base_object_types[i]->dispose_from_old_structure_bsp(object_datum);
		}
	}
}

void object_type_place(datum object_datum, s_scenario_object* object)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->object_place)
		{
			object_type->base_object_types[i]->object_place(object_datum, object);
		}
	}
}

void object_type_create_children(datum object_datum)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; ++i)
	{
		if (object_type->base_object_types[i]->object_create_children)
		{
			object_type->base_object_types[i]->object_create_children(object_datum);
		}
	}
}

void  object_type_notify_impulse_sound(datum object_datum, int a2, int a3)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->notify_impulse_sound)
		{
			object_type->base_object_types[i]->notify_impulse_sound(object_datum, a2, a3);
		}
	}
}

void object_type_delete(datum object_datum)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->object_delete)
		{
			object_type->base_object_types[i]->object_delete(object_datum);
		}
	}
}

typedef void(* object_type_object_move_t)(datum object_datum);
object_type_object_move_t p_object_type_object_move;

void  object_type_move(datum object_datum)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->object_move)
		{
			object_type->base_object_types[i]->object_move(object_datum);
		}
	}
}

bool  object_type_compute_activation(datum object_datum, DWORD* a2, bool* a3)
{
	bool result = false;
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i] && !result; i++) 
	{
		if (object_type->base_object_types[i]->object_compute_activation)
		{
			result = object_type->base_object_types[i]->object_compute_activation(object_datum, a2, a3);
		}
	}

	return result;
}


void  object_type_attach_gamestate_entity(datum object_datum)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->attach_gamestate_entity)
		{
			object_type->base_object_types[i]->attach_gamestate_entity(object_datum);
		}
	}
}

void  object_type_detach_gamestate_entity(datum object_datum)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->detach_gamestate_entity)
		{
			object_type->base_object_types[i]->detach_gamestate_entity(object_datum);
		}
	}
}

void  object_type_handle_deleted_object(datum object_datum, int a2)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->handle_deleted_object)
		{
			object_type->base_object_types[i]->handle_deleted_object(object_datum, a2);
		}
	}
}

void  object_type_postprocess_node_matrices(datum object_datum, int node_count, real_matrix4x3* node_matracies)
{
	object_type_definition* object_type = get_game_object_type_definition(object_datum);

	for (byte i = 0; object_type->base_object_types[i]; i++)
	{
		if (object_type->base_object_types[i]->process_node_matricies)
		{
			object_type->base_object_types[i]->process_node_matricies(object_datum, node_count, node_matracies);
		}
	}
}
