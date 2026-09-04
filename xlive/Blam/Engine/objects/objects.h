#pragma once
#include "emblems.h"
#include "object_placement.h"

#include "math/real_math.h"

/* constants */

/* macros */

#define object_header_get(index)			((struct object_header_datum*)datum_get(object_header_data_get(), index))
#define object_header_try_and_get(index)	((struct object_header_datum*)datum_try_and_get(object_header_data, (index)))

#define object_get(index)			((struct object_datum*)(object_get_and_verify_type((index), _object_mask_all)))
#define object_try_and_get(index)	((struct object_datum*)(object_try_and_get_and_verify_type((index), _object_mask_all)))

/* enums */

enum e_object_data_flags : uint32
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
	_object_dynamic_lighting_recompute_bit = 11,
	_object_static_lighting_recompute_bit = 12,
	_object_static_lighting_raycast_sideways_bit = 13,
	_object_garbage_bit = 14,
	_object_cannot_be_garbage_bit = 15,
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
	_object_was_in_local_physics_bit = 1,
	_object_in_local_physics_bit = 2,
	_object_is_early_mover_bit = 3,
	_object_is_early_mover_child_bit = 4,
	_object_physics_bit_5 = 5,
	_object_connected_to_physics_bit = 6,
	_object_physics_bit_8 = 8,
	_object_physics_bit_9 = 9,
	_object_physics_bit_10 = 10,
	_object_physics_bit_11 = 11,
	_object_physics_bit_12 = 12,
	k_object_physics_flags_count
};

enum e_object_damage_flags : uint16
{
	_object_damage_bit_0,
	_object_damage_bit_1,
	_object_is_dead_bit,
	_object_damage_bit_3,
	_object_created_with_overshield,
	_object_damage_bit_5,
	_object_damage_bit_6,
	_object_is_immune_to_damage,
	_object_damage_bit_8,
	_object_damage_bit_9,
	_object_damage_bit_10,
	_object_damage_bit_11,
	_object_damage_bit_12,
	_object_damage_bit_13,
	_object_damage_bit_14,
	k_object_damage_flags_count,
};

enum e_object_simulation_flags : uint8
{
	_object_simulation_attached_bit = 0,
	k_object_simulation_flags_count
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

enum
{
	_pvs_activation_normal = 0,
	_pvs_activation_object,
	_pvs_activation_cluster,
	NUMBER_OF_OBJECT_PVS_ACTIVATION_TYPES,
};

/* structures */

struct object_iterator
{
	uint32 type_flags;
	uint8 flags;
	int8 pad;
	int16 absolute_index;
	datum index;
	uint32 signature;
};
ASSERT_STRUCT_SIZE(object_iterator, 16);

struct object_header_block_reference
{
	int16 size;
	int16 offset;
};
ASSERT_STRUCT_SIZE(object_header_block_reference, 4);

struct s_object_payload
{
	int16/*e_object_type*/ object_type;
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

struct _object_datum
{
	c_flags_no_init<e_object_data_flags, uint32, k_object_data_flags_count> flags;
	void* object_header_block;
	datum next_object_index;
	datum first_child_object_index;
	datum parent_object_index;
	uint8 matrix_index;
	uint8 inhibited_flags;
	int16 placement_index;
	int32 next_recycling_object_index;
	int32 recycling_time;
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
	uint32 last_motion_time;
	c_flags_no_init<e_object_physics_flags, uint16, k_object_physics_flags_count> physics_flags;
	uint16 damage_owner_target_model_abs_index;
	datum damage_owner_owner_index;
	datum damage_owner_object_index;
	datum cached_render_state_index;
	int16 structure_bsp_fake_lightprobe_index;
	int8 model_variant_id;					// hlmt variant tag_block index
	int8 gap_D3;
	int32 simulation_entity_index;
	c_flags_no_init<e_object_simulation_flags, uint8, k_object_simulation_flags_count> simulation_flags;
	int8 gap_D9[3];
	datum object_projectile_datum;
	uint16 destroyed_constraints_flag;
	uint16 loosened_constraints_flag;
	real32 maximum_body_vitality;
	real32 maximum_shield_vitality;
	real32 body_vitality;
	real32 shield_vitality;
	real32 current_shield_damage;
	real32 current_body_damage;
	real32 recent_shield_damage;
	real32 recent_body_damage;
	int16 shield_stun_ticks;
	int16 body_stun_ticks;
	int8 shield_damage_decay_timer;
	int8 body_damage_decay_timer;
	c_flags_no_init<e_object_damage_flags, uint16, k_object_damage_flags_count> object_damage_flags;
	object_header_block_reference original_orientation_block;
	object_header_block_reference node_orientation_block;
	object_header_block_reference nodes_block;
	object_header_block_reference collision_regions_block;
	object_header_block_reference object_attachments_block;
	object_header_block_reference damage_sections_block;
	object_header_block_reference change_color_block;
	object_header_block_reference animation_manager_block;
};
ASSERT_STRUCT_SIZE(_object_datum, 296);

struct object_datum
{
	datum definition_index;
	_object_datum object;
};
ASSERT_STRUCT_SIZE(object_datum, 300);

struct object_header_datum
{
	int16 identifier;
	c_flags_no_init<e_object_header_flags, uint8, k_object_header_flags> flags;
	int8/*e_object_type*/ type;
	int16 cluster_index;
	int16 data_size;
	object_datum* datum;
};
ASSERT_STRUCT_SIZE(object_header_datum, 12);

struct object_marker
{
	int16 node_index;
	int16 region_index;
	real_matrix4x3 node_matrix;
	real_matrix4x3 matrix;
	real32 radius;
};
ASSERT_STRUCT_SIZE(object_marker, 112);

struct object_damage_section
{
	int8 gap0[8];
};
ASSERT_STRUCT_SIZE(object_damage_section, 8);

struct object_region_information
{
	int8 variant_permutation_index;
	int8 region_state;
	uint8 region_state_properties;
	uint8 pad;
	int32 looping_effect_index;
};
ASSERT_STRUCT_SIZE(object_region_information, 8);

#ifdef OBJECT_DEBUG
struct objects_information
{
	int16 object_count;
	int16 free_objects;
	int16 active_object_count;
	int16 active_garbage_object_count;
	real32 used_memory;
	real32 contiguous_used_memory;
};
#endif

/* classes */

class c_object_iterator_base
{
public:
	datum get_index(void) const;

protected:
	void object_iterator_begin_internal(int32 type_flags, uint8 flags);
	object_iterator m_iterator;
};

template <typename T>
class c_object_iterator : public c_object_iterator_base
{
public:
	void begin(int32 type_flags, uint8 flags)
	{
		object_iterator_begin_internal(type_flags, flags);
		return;
	}

	bool next(void)
	{
		m_current = (T*)object_iterator_next(&m_iterator);
		return m_current != NULL;
	}

	T* get_datum(void) const
	{
		return m_current;
	}

private:
	T* m_current;
};

/* prototypes */

struct data_array* object_header_data_get(void);

void objects_apply_patches(void);

void __cdecl objects_initialize_for_new_map(void);

// Gets the object and verifies the type, returns NULL if object doesn't match object type flags
void* __cdecl object_try_and_get_and_verify_type(datum object_index, int32 object_type_flags);

void __cdecl object_iterator_new(object_iterator* iterator, int32 type_flags, uint8 flags);

void* __cdecl object_iterator_next(object_iterator* iterator);

void* object_header_block_get(const datum object_index, const object_header_block_reference* reference);

void* object_header_block_get_with_count(const datum object_datum, const object_header_block_reference* reference, uint32 element_size, int32* element_count);

void __cdecl object_placement_data_new(object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, s_damage_owner* damage_owner);

int32 __cdecl object_index_from_name_index(int16 name_index);

bool object_is_connected_to_map(datum object_index);

void __cdecl object_update_collision_culling(datum object_datum);

bool object_header_block_allocate(datum object_index, int16 block_reference_offset, int16 size, uint16 alignment_bits);

e_object_type object_get_type(datum object_index);

// Sets the mode_tag_index and coll_tag_index parameters (if they exist)
// Returns whether or not the render model has PRT or Lighting Info
bool __cdecl object_is_prt_and_lightmapped(datum object_datum, datum* mode_tag_index, datum* coll_tag_index);

pixel32 __cdecl object_set_initial_change_colors(datum object_index, uint32 change_color_override_mask, real_rgb_color* colour);

// Initializes health and shields for the given object based on two floats passed to it
// If the floats for vitality are NULL we just initialize them to the default values in the damage info block in the model tag
void __cdecl object_initialize_vitality(datum object_index, const real32* new_vitality, const real32* new_shield_vitality);

// Wakes object by adding the _object_header_awake_bit to every object and all parents of that object
void __cdecl object_wake(datum object_index);

void __cdecl object_activate(datum object_index);

void __cdecl object_deactivate(datum object_index);

void __cdecl object_compute_node_matrices_with_children(datum object_index);

void __cdecl object_reconnect_to_physics(datum object_index);

void __cdecl objects_update(void);

void __cdecl objects_move(void);

datum __cdecl object_new(object_placement_data* data);

void __cdecl object_delete(datum object_idx);

real_point3d* __cdecl object_get_center_of_mass(datum object_index, real_point3d* point);

void __cdecl object_get_origin(datum object_index, real_point3d* point_out, bool interpolated);

void object_get_origin_interpolated(datum object_index, real_point3d* point_out);

real_matrix4x3* object_get_node_matrix(datum object_datum, int16 node_index);

real_matrix4x3* object_try_get_node_matrix_interpolated(datum object_index, int16 node_index, real_matrix4x3* out_mat);

real_matrix4x3* object_get_node_matrices(datum object_index, int32* out_node_count);

int32 __cdecl object_get_skinning_matrices(datum object_index, int32 skinning_matrix_count, const real_matrix4x3* object_skinning_matrices, real_matrix4x3* out_object_skinning_matrices);

datum object_get_damage_owner(datum damaged_unit_index);

int32 object_get_entity_index(datum object_idx);

void object_get_region_information(
	int32 object_index,
	int32* region_count,
	int8** region_permutation_indices,
	int8** region_render_permutation_indices,
	int8** region_information);

void __cdecl object_apply_function_overlay_node_orientations(
	datum object_index,
	struct render_model_definition* render_model,
	class c_animation_manager* animation_manager,
	int32 a4,
	int32 orientation_count,
	real_orientation* orientations);

real_point3d* object_get_center_of_mass_interpolated(datum object_datum, real_point3d* center_of_mass);

datum __cdecl object_get_parent_recursive(datum parent_index);

void __cdecl object_pre_delete_recursive(datum object_index);

void __cdecl object_delete_recursive(datum object_index, bool remove_from_map_before_deletion);

void __cdecl objects_garbage_collection(void);

void objects_purge_deleted_objects(void);

void __cdecl object_get_velocities(datum object_index, real_vector3d* translational_velocity, real_vector3d* angular_velocity);

uint32 __cdecl object_search_for_objects_in_radius(
	int32 search_flags,
	uint32 object_type_flags,
	s_location* location,
	real_point3d* center,
	real32 radius,
	datum* out_object_indices,
	uint32 object_count);

void __cdecl objects_post_update(void);

int16 __cdecl object_get_markers_by_string_id(datum object_index, string_id marker, object_marker* marker_object, int16 count);

bool __cdecl object_force_inside_bsp(datum object_index, const real_point3d* known_good_point, int32 ignore_object_index);

void* object_get_and_verify_type(datum object_index, int32 object_type_mask);

datum object_get_ultimate_parent(datum object_index);

void object_attach_to_node(datum parent_index, datum child_index, uint16 node_index);

#ifdef OBJECT_DEBUG
void objects_information_get(objects_information* information);
#endif

void object_set_hidden(datum object_index, bool hidden);

#ifdef OBJECT_OVERRIDE_ENABLED
void object_override_set_shader(datum object_index, datum shader_tag_index);

datum object_override_get_shader(datum object_index);
#endif

#ifdef OBJECT_DEBUG
void objects_dump_memory(void);
#endif
