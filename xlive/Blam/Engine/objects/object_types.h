#pragma once

/*
* THESE ARE ONLY USED FOR REFERENCE DO NOT UNCOMMENT
typedef void (*object_initialize_t)();
typedef void (*object_dispose_t)();
typedef void (*object_initialize_for_new_map_t)();
typedef void (*object_dispose_from_old_map_t)();
typedef void (*object_initialize_for_new_structure_bsp_t)(datum);
typedef void (*object_dispose_from_old_structure_bsp_t)(datum);
typedef void (*object_adjust_placement_t)(object_placement_data*);
typedef bool (*object_new_t)(datum, object_placement_data*);
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
*/

#define k_max_object_type_inheritence 3
struct object_type_definition
{
	const char* name;
	int32 group_tag;
	int16 datum_size;
	int16 placement_tag_block_offset;
	int16 palette_tag_block_offset;
	int16 placement_tag_block_size;
	void* object_initialize;
	void* object_dispose;
	void* object_initialize_for_new_map;
	void* object_dispose_from_old_map;
	void* initialize_for_new_structure_bsp;
	void* object_dispose_from_old_structure_bsp;
	void* object_adjust_placement;
	void* object_new;
	void* object_place;
	void* object_unplace;
	void* object_create_children;
	void* object_delete;
	void* object_update;
	void* object_move;
	void* object_compute_activation;
	void* object_type_compute_function_value;
	void* object_attach_gamestate_entity;
	void* object_detach_gamestate_entity;
	void* object_handle_deleted_object;
	void* object_handle_deleted_player;
	void* object_handle_region_destroyed;
	void* object_handle_parent_destroyed;
	void* object_fix_transform;
	void* object_preprocess_node_orientations;
	void* object_preprocess_root_node_matrix;
	void* object_postprocess_node_matrices;
	void* object_reset;
	void* object_notify_impulse_sound;
	void* object_render_debug;
	object_type_definition* base_object_types[k_max_object_type_inheritence];
	int32 unused[13];
	void* next;
};
CHECK_STRUCT_SIZE(object_type_definition, 0xC8);

enum e_object_type : int8
{
	_object_type_biped = 0,
	_object_type_vehicle = 1,
	_object_type_weapon = 2,
	_object_type_equipment = 3,
	_object_type_garbage = 4,
	_object_type_projectile = 5,
	_object_type_scenery = 6,
	_object_type_machine = 7,
	_object_type_control = 8,
	_object_type_light_fixture = 9,
	_object_type_sound_scenery = 10,
	_object_type_crate = 11,
	_object_type_creature = 12,
	k_object_types_count,
};

object_type_definition** get_object_type_definitions();
object_type_definition* object_type_definition_get(e_object_type object_type);
