#pragma once
#include "emblems.h"
#include "object_placement.h"
#include "object_type_list.h"

#include "animations/animation_manager.h"
#include "memory/data.h"
#include "math/matrix_math.h"
#include "models/render_model_definitions.h"
#include "memory/memory_pool.h"
#include "memory/static_arrays.h"

#define k_maximum_objects_per_map 2048

enum e_object_data_flags : int32
{
	_object_hidden_bit = 0,
	_object_always_active_bit = 1,
	_object_being_created_bit = 2,
	_object_underwater_bit = 3,
	_object_has_attached_lights_bit = 4,
	_object_has_attached_looping_sounds_bit = 5,
	_object_has_unattached_lights_bit = 6,
	_object_in_limbo_bit = 7,
	_object_connected_to_map_bit = 8,
	_object_uses_collidable_list_bit = 9,
	_object_mirrored_bit = 10,
	_object_data_bit_11 = 11,
	_object_data_bit_12 = 12,
	_object_data_bit_13 = 13,
	_object_garbage_bit = 14,
	_object_data_bit_15 = 15,
	_object_shadowless_bit = 16,
	_object_deleted_when_deactivated_bit = 17,
	_object_outside_of_map_bit = 18,
	_object_cinematic_lod_bit = 19,
	_object_cinematic_collision_bit = 20,
	_object_cinematic_visibility_bit = 21,
	_object_static_pathfinding_bit = 22,
	_object_dynamic_pathfinding_disabled_bit = 23,
	_object_uses_cinematic_lighting_bit = 24,
	_object_has_override_bit = 25,
	_object_created_with_parent_bit = 26,
	_object_reconnect_to_map_bit = 27,
	_object_ever_referenced_by_hs_bit = 28,
	_object_orientations_frozen_bit = 29,
	_object_render_only_orientations_needed_bit = 30,
	_object_has_prt_or_lighting_info_bit = 31,
	k_object_data_flags_count
};

enum e_object_physics_flags : uint16
{
	_object_allocated_havok_component_bit = 0,
	_object_physics_bit_1 = 1,
	_object_physics_bit_2 = 2,
	_object_is_early_mover_bit = 3,
	_object_is_early_mover_child_bit = 4,
	_object_physics_bit_5 = 5,
	_object_connected_to_physics_bit = 6,
	_object_physics_bit_8 = 8,
	_object_physics_bit_9 = 9,
	_object_physics_bit_10 = 10,
	_object_physics_bit_11 = 11,
	k_object_physics_flags_count
};

enum e_object_damage_flags : uint16
{
	_object_is_dead_bit = 2,
};

enum e_object_header_flags : uint8
{
	_object_header_active_bit = 0,
	_object_header_awake_bit = 1,
	_object_header_requires_motion_bit = 2,
	_object_header_post_update_bit = 3,
	_object_header_being_deleted_bit = 4,
	_object_header_do_not_update_bit = 5,
	_object_header_connected_to_map_bit = 6,
	_object_header_child_bit = 7,
	k_object_header_flags
};

struct object_header_block_reference
{
	int16 size;
	int16 offset;
};
ASSERT_STRUCT_SIZE(object_header_block_reference, 4);

struct s_object_header
{
	int16 datum_salt;
	c_flags<e_object_header_flags, uint8, k_object_header_flags> flags;
	e_object_type object_type;
	int16 cluster_index;
	int16 object_data_size;
	void* object;
};
ASSERT_STRUCT_SIZE(s_object_header, 12);

struct s_object_payload
{
	e_object_type object_type;
	int8 pad;
	uint16 object_collision_cull_flags;
	real_point3d origin_point;
	real32 bounding_sphere_radius;
};
ASSERT_STRUCT_SIZE(s_object_payload, 20);

// Used in object_attachments_block
struct object_attachment
{
	int8 field_0;
	int8 pad[3];
	int32 field_4;
};
ASSERT_STRUCT_SIZE(object_attachment, 8);

struct object_datum
{
	datum tag_definition_index;
	c_flags_no_init<e_object_data_flags, uint32, k_object_data_flags_count> flags;
	void* object_header_block;
	datum next_index;
	datum current_weapon_datum;
	datum parent_object_index;
	uint8 matrix_index;
	uint8 inhibited_flags;
	int16 placement_index;
	uint8 gap_1C[8];
	s_emblem_info emblem_info;
	uint8 pad;
	s_location location;
	real_point3d center;
	real32 radius;
	real_point3d object_origin_point;
	real32 shadow_sphere_radius;
	real_point3d dynamic_light_sphere_offset;
	real32 dynamic_light_sphere_radius;
	int32 first_cluster_reference;
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real_vector3d translational_velocity;
	real_vector3d angular_velocity;
	real32 scale;
	c_object_identifier object_identifier;
	int16 name_list_index;
	uint8 structure_bsp_index;
	int8 netgame_equipment_index;
	e_bsp_policy placement_policy;
	int8 field_b1;
	int8 field_b2;
	int8 field_b3;
	datum havok_datum;
	datum early_mover_index;
	uint32 unkBC;
	c_flags<e_object_physics_flags, uint16, k_object_physics_flags_count> physics_flags;
	uint16 damage_owner_target_model_abs_index;
	datum damage_owner_owner_index;
	datum damage_owner_object_index;
	datum cached_object_render_state_index;
	int16 field_D0;
	int8 model_variant_id;					// hlmt variant tag_block index
	int8 gap_D3;
	int32 simulation_entity_index;
	bool attached_to_simulation;
	int8 gap_D9[3];
	datum object_projectile_datum;
	uint16 destroyed_constraints_flag;
	uint16 loosened_constraints_flag;
	real32 body_max_vitality;
	real32 shield_max_vitality;
	real32 body_current_vitality;
	real32 shield_current_vitality;
	uint32 gap_F4[3];
	real32 field_100;
	uint16 shield_stun_ticks;
	uint16 body_stun_ticks;
	int8 byte_108;
	int8 byte_109;
	e_object_damage_flags object_damage_flags;
	object_header_block_reference original_orientation_block;
	object_header_block_reference node_orientation_block;
	object_header_block_reference nodes_block;
	object_header_block_reference collision_regions_block;
	object_header_block_reference object_attachments_block;
	object_header_block_reference damage_sections_block;
	object_header_block_reference change_color_block;
	object_header_block_reference animation_manager_block;
};
ASSERT_STRUCT_SIZE(object_datum, 300);

struct object_marker
{
	int16 node_index;
	int16 region_index;
	real_matrix4x3 node_matrix;
	real_matrix4x3 matrix;
	int32 field_6C;
};
ASSERT_STRUCT_SIZE(object_marker, 112);

s_data_array* object_header_data_get(void);

static s_object_header* object_get_header(datum object_idx)
{
	return (s_object_header*)datum_get(object_header_data_get(), object_idx);
}

// Get the object fast, with no validation from datum index
template<typename T = object_datum>
static T* object_get_fast_unsafe(datum object_idx)
{
	s_object_header* header = object_get_header(object_idx);
	return (T*)header->object;
}

s_memory_pool* get_object_table(void);

// Gets the object and verifies the type, returns NULL if object doesn't match object type flags
void* __cdecl object_try_and_get_and_verify_type(datum object_index, int32 object_type_flags);

void* object_header_block_get(const datum object_datum, const object_header_block_reference* reference);
void* object_header_block_get_with_count(const datum object_datum, const object_header_block_reference* reference, uint32 element_size, int32* element_count);

void __cdecl object_placement_data_new(object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, s_damage_owner* damage_owner);

void object_initialize_for_interpolation(datum object_index);

datum __cdecl object_new(object_placement_data* placement_data);
void __cdecl object_delete(datum object_idx);

real_point3d* __cdecl object_get_center_of_mass(datum object_index, real_point3d* point);

datum object_get_damage_owner(datum damaged_unit_index);

int32 object_get_entity_index(datum object_idx);

void object_get_origin_interpolated(datum object_index, real_point3d* point_out);

real_matrix4x3* object_get_node_matrix(datum object_datum, int16 node_index);

int32 __cdecl object_get_skinning_matrices(datum object_index, int32 skinning_matrix_count, real_matrix4x3* object_skinning_matrices, real_matrix4x3* out_object_skinning_matrices);

real_matrix4x3* object_try_get_node_matrix_interpolated(datum object_index, int16 node_index, real_matrix4x3* out_mat);

real_matrix4x3* object_get_node_matrices(datum object_datum, int32* out_node_count);

void __cdecl object_apply_function_overlay_node_orientations(datum object_index, 
	render_model_definition* render_model, 
	c_animation_manager* animation_manager, 
	int32 a4, 
	int32 orientation_count, 
	real_orientation* orientations);

void __cdecl object_get_origin(datum object_index, real_point3d* point_out, bool interpolated);
real_point3d* object_get_center_of_mass_interpolated(datum object_datum, real_point3d* center_of_mass);

datum __cdecl object_get_parent_recursive(datum parent_index);

int16 __cdecl object_get_markers_by_string_id(datum object_index, string_id marker, object_marker* marker_object, int16 count);

void __cdecl objects_purge_deleted_objects(void);

void __cdecl object_get_velocities(datum object_index, real_vector3d* translational_velocity, real_vector3d* angular_velocity);

void objects_apply_patches(void);
