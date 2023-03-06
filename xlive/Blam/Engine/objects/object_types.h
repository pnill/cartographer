#pragma once
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Engine/objects/object_placement.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Blam/Engine/math/real_math.h"

typedef void (*type_initialize_gamestate_data_t)();
typedef bool (*type_object_new_t)(datum, object_placement_data*, bool*);
typedef void (*type_object_adjust_placement_t)(object_placement_data*);
typedef void (*type_dispose_from_old_structure_bsp_t)(datum);
typedef void (*type_object_place_t)(datum, s_scenario_object*);
typedef void (*type_object_unplace_t)(datum);
typedef void (*type_object_create_children_t)(datum);
typedef void (*type_object_delete_t)(datum);
typedef void (*type_attach_gamestate_entity_t)(datum);
typedef void (*type_detach_gamestate_entity_t)(datum);
typedef void (*type_handle_deleted_object_t)(datum, int);
typedef void (*type_object_move_t)(datum);
typedef bool (*type_object_compute_activation_t)(datum, DWORD*, bool*);
typedef bool (*type_compute_function_value_t)(datum, string_id, float*, bool*);
typedef bool (*type_handle_parent_destroyed_t)(datum);
typedef void (*type_fix_transform_t)(datum, real_point3d*, real_vector3d*, real_vector3d*);
typedef void (*type_preprocess_node_orientations_t)(datum, byte*, int, real_quaternion*);
typedef void (*type_preprocess_root_node_matrix_t)(datum, real_matrix4x3*);
typedef void (*type_process_node_matricies_t)(datum, int, real_matrix4x3*);
typedef void (*type_object_reset_t)(datum);
typedef void (*type_notify_impulse_sound_t)(datum, int, int);

enum e_object_type : signed char
{
	biped = 0,
	vehicle,
	weapon,
	equipment,
	garbage,
	projectile,
	scenery,
	machine,
	control,
	light_fixture,
	sound_scenery,
	crate,
	creature,
};

#define G_MAX_OBJECT_TYPE_IHERITENCE 3
struct object_type_definition
{
	const char* name;
	blam_tag::tag_group_type group_tag;
	__int16 datum_size;
	__int16 placement_tag_block_offset;
	__int16 palette_tag_block_offset;
	__int16 placement_tag_block_size;
	type_initialize_gamestate_data_t initialize_gamestate_data;//();
	void* dispose_gamestate_data;//();
	void* initialize_gamestate_for_new_map;// ();
	void* dispose_gamestate_from_old_map;// ();
	void* new_object_placement_data;// (datum onject_datum, object_placement_data* placement_data);
	type_dispose_from_old_structure_bsp_t dispose_from_old_structure_bsp;// (datum object_datum);
	type_object_adjust_placement_t adjust_placement;// (datum object_datum);
	type_object_new_t object_new;
	type_object_place_t object_place;
	type_object_unplace_t object_unplace;
	type_object_create_children_t object_create_children;
	type_object_delete_t object_delete;
	void* object_update;
	type_object_move_t object_move;
	type_object_compute_activation_t object_compute_activation;
	type_compute_function_value_t object_type_compute_function_value;
	type_attach_gamestate_entity_t attach_gamestate_entity;
	type_detach_gamestate_entity_t detach_gamestate_entity;
	type_handle_deleted_object_t handle_deleted_object;
	void* unknown5C;
	void* unknown60;
	type_handle_parent_destroyed_t handle_parent_destroyed;
	type_fix_transform_t fix_transform;
	type_preprocess_node_orientations_t preprocess_node_orientations;
	type_preprocess_root_node_matrix_t preprocess_root_node_matrix;
	type_process_node_matricies_t process_node_matricies;
	type_object_reset_t object_reset;
	type_notify_impulse_sound_t notify_impulse_sound;
	void* unknown80;
	object_type_definition* base_object_types[G_MAX_OBJECT_TYPE_IHERITENCE]; //Last index always reference's itself for function calling
	int unused[13];
	void* next;
};
CHECK_STRUCT_SIZE(object_type_definition, 0xC8);

object_type_definition** get_object_type_definitions();
object_type_definition* object_type_definition_get(e_object_type object_type);
object_type_definition* get_game_object_type_definition(datum object_datum);
int object_type_from_group_tag(datum tag_datum);
bool object_type_new(datum object_datum, object_placement_data* placement_data, bool* some_bool);
void object_type_adjust_placement(object_placement_data* placement_data);
void object_type_dispose_from_old_structure_bsp(datum object_datum);
void object_type_place(datum object_datum, s_scenario_object* object);
void object_type_create_children(datum object_datum);
void object_type_notify_impulse_sound(datum object_datum, int a2, int a3);
void object_type_delete(datum object_datum);
void object_type_move(datum object_datum);
bool object_type_compute_activation(datum object_datum, DWORD* a2, bool* a3);
void object_type_attach_gamestate_entity(datum object_datum);
void object_type_detach_gamestate_entity(datum object_datum);
void object_type_postprocess_node_matrices(datum object_datum, int node_count, real_matrix4x3* node_matracies);