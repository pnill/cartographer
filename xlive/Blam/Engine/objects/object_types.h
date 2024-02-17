#pragma once
#include "object_type_list.h"
#include "object_placement.h"

#include "Blam/Engine/scenario/scenario.h"

typedef void (*object_initialize_t)();
typedef void (*object_dispose_t)();
typedef void (*object_initialize_for_new_map_t)();
typedef void (*object_dispose_from_old_map_t)();
typedef void (*object_initialize_for_new_structure_bsp_t)(datum);
typedef void (*object_dispose_from_old_structure_bsp_t)(datum);
typedef void (*object_adjust_placement_t)(object_placement_data*);
typedef bool (*object_new_t)(datum, object_placement_data*, bool*);
typedef void (*object_place_t)(datum, s_scenario_object*);
typedef void (*object_unplace_t)(datum);
typedef void (*object_create_children_t)(datum);
typedef void (*object_delete_t)(datum);
typedef bool (*object_update_t)(datum);
typedef void (*object_move_t)(datum);
typedef bool (*object_compute_activation_t)(datum, s_game_cluster_bit_vectors*, bool*);
typedef bool (*object_compute_function_value_t)(datum, string_id, real32*, bool*);
typedef void (*object_attach_gamestate_entity_t)(datum);
typedef void (*object_detach_gamestate_entity_t)(datum);
typedef void (*object_handle_deleted_object_t)(datum, datum);
typedef void (*object_handle_deleted_player_t)(datum, datum);
typedef void (*object_handle_region_destroyed_t)(datum, int16, uint32);
typedef bool (*object_handle_parent_destroyed_t)(datum);
typedef void (*object_fix_transform_t)(datum, real_point3d*, real_vector3d*, real_vector3d*);
typedef void (*object_preprocess_node_orientations_t)(datum, uint8*, int32, real_orientation*);
typedef void (*object_preprocess_root_node_matrix_t)(datum, real_matrix4x3*);
typedef void (*object_postprocess_node_matrices_t)(datum, int32, real_matrix4x3*);
typedef void (*object_reset_t)(datum);
typedef void (*object_notify_impulse_sound_t)(datum, int32, int32);
typedef void (*object_render_debug)(datum);


#define k_max_object_type_inheritence 16
struct object_type_definition
{
	const char* name;
	blam_tag tag_group;
	int16 datum_size;
	int16 placement_tag_block_offset;
	int16 palette_tag_block_offset;
	int16 placement_tag_block_size;
	object_initialize_t object_initialize;
	object_dispose_t object_dispose;
	object_initialize_for_new_map_t object_initialize_for_new_map;
	object_dispose_from_old_map_t object_dispose_from_old_map;
	object_initialize_for_new_structure_bsp_t initialize_for_new_structure_bsp;
	object_dispose_from_old_structure_bsp_t object_dispose_from_old_structure_bsp;
	object_adjust_placement_t object_adjust_placement;
	object_new_t object_new;
	object_place_t object_place;
	object_unplace_t object_unplace;
	object_create_children_t object_create_children;
	object_delete_t object_delete;
	object_update_t object_update;
	object_move_t object_move;
	object_compute_activation_t object_compute_activation;
	object_compute_function_value_t object_type_compute_function_value;
	object_attach_gamestate_entity_t object_attach_gamestate_entity;
	object_detach_gamestate_entity_t object_detach_gamestate_entity;
	object_handle_deleted_object_t object_handle_deleted_object;
	object_handle_deleted_player_t object_handle_deleted_player;
	object_handle_region_destroyed_t object_handle_region_destroyed;
	object_handle_parent_destroyed_t object_handle_parent_destroyed;
	object_fix_transform_t object_fix_transform;
	object_preprocess_node_orientations_t object_preprocess_node_orientations;
	object_preprocess_root_node_matrix_t object_preprocess_root_node_matrix;
	object_postprocess_node_matrices_t object_postprocess_node_matrices;
	object_reset_t object_reset;
	object_notify_impulse_sound_t object_notify_impulse_sound;
	object_render_debug object_render_debug;
	object_type_definition* base_object_types[k_max_object_type_inheritence];
	void* next;
};
CHECK_STRUCT_SIZE(object_type_definition, 0xC8);

object_type_definition** get_object_type_definitions(void);
object_type_definition* object_type_definition_get(e_object_type object_type);
object_type_definition* get_game_object_type_definition(datum object_datum);

void __cdecl object_type_adjust_placement(object_placement_data* placement_data);

bool object_type_new(datum object_index, object_placement_data* placement_data, bool* a3);

void object_type_create_children(datum object_index);

void object_type_delete(datum object_datum);

bool object_type_compute_activation(datum object_datum, s_game_cluster_bit_vectors* cluster_activation, bool* a3);

void object_type_postprocess_node_matrices(datum object_datum, int32 node_count, real_matrix4x3* node_matracies);


