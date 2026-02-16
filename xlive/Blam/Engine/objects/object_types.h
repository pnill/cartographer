#pragma once

/* constants */

enum
{
	k_max_object_type_inheritence = 16
};

/* typedefs */

typedef void (*object_initialize_t)(void);
typedef void (*object_dispose_t)(void);
typedef void (*object_initialize_for_new_map_t)(void);
typedef void (*object_dispose_from_old_map_t)(void);
typedef void (*object_initialize_for_new_structure_bsp_t)(datum);
typedef void (*object_dispose_from_old_structure_bsp_t)(datum);
typedef void (*object_adjust_placement_t)(struct object_placement_data*);
typedef bool (*object_new_t)(datum, struct object_placement_data*, bool*);
typedef void (*object_place_t)(datum, void*);
typedef void (*object_unplace_t)(datum);
typedef void (*object_create_children_t)(datum);
typedef void (*object_delete_t)(datum);
typedef bool (*object_update_t)(datum);
typedef void (*object_move_t)(datum);
typedef bool (*object_compute_activation_t)(datum, struct s_game_cluster_bit_vectors*, bool*);
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

/* enums */

enum e_object_type : int32
{
	_object_type_none = NONE,
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
	NUMBER_OF_OBJECT_TYPES = k_object_types_count,	// Duplicate constant? Both names appear in halo 2

	_object_mask_all = NONE,
	_object_mask_unit = FLAG(_object_type_biped) | FLAG(_object_type_vehicle),
	_object_mask_biped = FLAG(_object_type_biped),
	_object_mask_vehicle = FLAG(_object_type_vehicle),

	_object_mask_item = FLAG(_object_type_weapon) | FLAG(_object_type_equipment) | FLAG(_object_type_garbage),

	_object_mask_weapon = FLAG(_object_type_weapon),
	_object_mask_equipment = FLAG(_object_type_equipment),
	_object_mask_garbage = FLAG(_object_type_garbage),
	_object_mask_projectile = FLAG(_object_type_projectile),
	_object_mask_scenery = FLAG(_object_type_scenery),
	_object_mask_sound_scenery = FLAG(_object_type_sound_scenery),

	_object_mask_device = FLAG(_object_type_machine) | FLAG(_object_type_control) | FLAG(_object_type_light_fixture),
	_object_mask_machine = FLAG(_object_type_machine),
	_object_mask_control = FLAG(_object_type_control),
	_object_mask_light_fixture = FLAG(_object_type_light_fixture),

	_object_mask_crate = FLAG(_object_type_crate),

	_object_mask_creature = FLAG(_object_type_creature),

	_object_mask_sightblocking = FLAG(_object_type_vehicle) | FLAG(_object_type_scenery) | FLAG(_object_type_machine),
	_object_mask_cannot_interpolate = FLAG(_object_type_sound_scenery) | FLAG(_object_type_light_fixture) | FLAG(_object_type_control) | FLAG(_object_type_machine) | FLAG(_object_type_scenery) | FLAG(_object_type_projectile),
	_object_mask_havok = FLAG(_object_type_creature) | FLAG(_object_type_crate) | FLAG(_object_type_machine) | FLAG(_object_type_vehicle) | FLAG(_object_type_biped)
};

/* structures */

struct object_type_definition
{
	const char* name;
	tag_group group_tag;
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
	object_type_definition* part_definitions[k_max_object_type_inheritence];
	object_type_definition* next;
};
ASSERT_STRUCT_SIZE(object_type_definition, 0xC8);

/* prototypes */

object_type_definition** get_object_type_definitions(void);

object_type_definition* object_type_definition_get(e_object_type object_type);

void __cdecl object_types_initialize_for_new_map(void);

void __cdecl object_type_adjust_placement(struct object_placement_data* placement_data);

bool object_type_new(datum object_index, struct object_placement_data* placement_data, bool* a3);

void object_type_create_children(datum object_index);

void object_type_delete(datum object_datum);

bool object_type_compute_activation(datum object_datum, struct s_game_cluster_bit_vectors* cluster_activation, bool* a3);

void object_type_postprocess_node_matrices(datum object_datum, int32 node_count, real_matrix4x3* node_matracies);

const char* object_type_get_name(e_object_type object_type);

/* public code */

inline e_object_type operator++(e_object_type& type)
{
	return (e_object_type)++((int32&)type);
};
