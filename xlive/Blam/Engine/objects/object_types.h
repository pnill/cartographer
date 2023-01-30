#pragma once
#include "object_placement.h"
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"

typedef void (*initialize_gamestate_data_t)();
typedef bool (*object_new_t)(datum, object_placement_data*, bool*);
typedef void (*object_adjust_placement_t)(object_placement_data*);
typedef void (*disconnect_from_structure_bsp_t)(datum);
typedef void (*object_delete_t)(datum, int);
typedef void (*unknown38_t)(datum);
typedef void (*unknown3C_t)(datum);
typedef void (*assign_new_entity_t)(datum);
typedef void (*detach_entity_t)(datum);
typedef void (*unknown7C_t)(datum, int, int);
typedef void (*object_move_t)(datum);
typedef void (*object_can_activate_t)(datum, int, int);
typedef void (*object_process_node_t)(datum, int, int);

#define G_MAX_OBJECT_TYPE_IHERITENCE 3
struct __declspec(align(4)) object_type_definition
{
	const char* name;
	blam_tag::tag_group_type group_tag;
	__int16 datum_size;
	__int16 placement_tag_block_offset;
	__int16 palette_tag_block_offset;
	__int16 placement_tag_block_size;
	initialize_gamestate_data_t initialize_gamestate_data;//();
	void* dispose_gamestate_data;//();
	void* initialize_gamestate_for_new_map;// ();
	void* dispose_gamestate_from_old_map;// ();
	void* new_object_placenment_data;// (datum onject_datum, object_placement_data* placement_data);
	disconnect_from_structure_bsp_t disconnect_from_structure_bsp;// (datum object_datum);
	object_adjust_placement_t adjust_placement;// (datum object_datum);
	object_new_t object_new;
	object_delete_t object_delete;
	void* unknown34;
	unknown38_t unknown38;
	unknown3C_t unknown3C;
	void* object_update;
	object_move_t object_move;
	object_can_activate_t object_can_activate;
	void* export_function_values;
	assign_new_entity_t assign_new_entity;
	detach_entity_t detach_entity;
	void* handle_deleted_object;
	void* unknown5C;
	void* unknown60;
	void* unknown64;
	void* unknown68;
	void* unknown6C;
	void* unknown70;
	object_process_node_t process_node_matricies;
	void* unknown78;
	unknown7C_t unknown7C;
	void* unknown80;
	object_type_definition* base_object_types[G_MAX_OBJECT_TYPE_IHERITENCE]; //Last index always reference's itself for function calling
	int unused[13];
	void* next;
};
CHECK_STRUCT_SIZE(object_type_definition, 0xC8);

object_type_definition** get_object_type_definitions();
object_type_definition* get_game_object_type_definition(datum object_datum);
int object_type_from_group_tag(datum tag_datum);

