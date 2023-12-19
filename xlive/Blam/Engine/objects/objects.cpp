#include "stdafx.h"
#include "objects.h"

#include "object_definition.h"
#include "object_early_movers.h"
#include "object_globals.h"
#include "object_types.h"

#include "Blam/Engine/animations/animation_manager.h"
#include "Blam/Engine/cache/cache_files.h"
#include "Blam/Engine/devices/devices.h"
#include "Blam/Engine/effects/effects.h"
#include "Blam/Engine/game/game.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/main/interpolator.h"
#include "Blam/Engine/memory/bitstream.h"
#include "Blam/Engine/models/models.h"
#include "Blam/Engine/objects/widgets/widgets.h"
#include "Blam/Engine/physics/collisions.h"
#include "Blam/Engine/physics/havok.h"
#include "Blam/Engine/physics/havok_memory.h"
#include "Blam/Engine/Simulation/game_interface/simulation_game_units.h"
#include "Blam/Engine/Simulation/simulation.h"
#include "Blam/Engine/structures/cluster_partitions.h"
#include "Blam/Engine/units/bipeds.h"

#include "Util/Hooks/Hook.h"

s_data_array* object_header_data_get(void)
{
	return *Memory::GetAddress<s_data_array**>(0x4E461C, 0x50C8EC);
};

s_memory_pool* get_object_table(void)
{
	return *Memory::GetAddress<s_memory_pool**>(0x4E4610, 0x50C8E0);
};

void* __cdecl object_try_and_get_and_verify_type(datum object_index, int32 object_type_flags)
{
	return INVOKE(0x1304E3, 0x11F3A6, object_try_and_get_and_verify_type, object_index, object_type_flags);
}

void* object_header_block_get(datum object_datum, const object_header_block_reference* reference)
{
	return (void*)((char*)object_get_fast_unsafe(object_datum) + reference->offset);
}

void* object_header_block_get_with_count(datum object_index, const object_header_block_reference* reference, uint32 element_size, int32* element_count)
{
	void* block;
	if (reference->offset == -1)
	{
		block = 0;
		*element_count = 0;
	}
	else
	{
		block = (void*)object_header_block_get(object_index, reference);
		*element_count = reference->size / element_size;
	}

	return block;
}

datum __cdecl object_header_new(int16 object_data_size)
{
	return INVOKE(0x130AF6, 0x11F9B9, object_header_new, object_data_size);
}

void __cdecl object_placement_data_new(object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, s_damage_owner* damage_owner)
{
	INVOKE(0x132163, 0x121033, object_placement_data_new, object_placement_data, object_definition_idx, object_owner_idx, damage_owner);
	return;
}

bool __cdecl object_is_connected_to_map(datum object_index)
{
	return INVOKE(0x132922, 0x1217F2, object_is_connected_to_map, object_index);
}

void __cdecl object_connect_lights_recursive(datum object_datum, bool disconnect_this_object, bool reconnect_this_object, bool reconnect_child_objects, bool show_this_object)
{
	INVOKE(0x134A78, 0x123948, object_connect_lights_recursive, object_datum, disconnect_this_object, reconnect_this_object, reconnect_child_objects, show_this_object);
	return;
}

void __cdecl object_update_collision_culling(datum object_datum)
{
	INVOKE(0x1324E9, 0x1213B9, object_update_collision_culling, object_datum);
	return;
}

bool __cdecl object_header_block_allocate(datum object_datum, int16 offset, int16 padded_size, int16 alignment_bits)
{
	return INVOKE(0x130BC6, 0x1213B9, object_header_block_allocate, object_datum, offset, padded_size, alignment_bits);
}

// Sets the mode_tag_index and coll_tag_index parameters (if they exist)
// Returns whether or not the render model has PRT or Lighting Info
bool __cdecl object_is_prt_and_lightmapped(datum object_datum, datum* mode_tag_index, datum* coll_tag_index)
{
	return INVOKE(0x131B0E, 0x1209DE, object_is_prt_and_lightmapped, object_datum, mode_tag_index, coll_tag_index);
}

// Returns the index of the variant block inside the model tag of the passed object_index
int8 __cdecl object_lookup_variant_index_from_name(datum object_index, string_id variant)
{
	return INVOKE(0x12FE84, 0x11ED47, object_lookup_variant_index_from_name, object_index, variant);
}

// Updates variant index from variant string id
void update_object_variant_index(datum object_index, string_id variant)
{
	object_datum* object = object_get_fast_unsafe(object_index);
	object->model_variant_id = object_lookup_variant_index_from_name(object_index, variant);
	return;
}

void __cdecl update_object_region_information(datum object_datum, int32 region_index)
{
	INVOKE(0x132F07, 0x121DD7, update_object_region_information, object_datum, region_index);
	return;
}

pixel32 __cdecl object_set_initial_change_colors(datum object_index, uint32 change_color_override_mask, real_rgb_color* colour)
{
	return INVOKE(0x1310F9, 0x11FFBD, object_set_initial_change_colors, object_index, change_color_override_mask, colour);
}

// Initializes health and shields for the given object based on two floats passed to it
// If the floats for vitality are NULL we just initialize them to the default values in the damage info block in the model tag
void object_initialize_vitality(datum object_index, const real32* new_vitality, const real32* new_shield_vitality)
{
	INVOKE(0x175A62, 0x14D2C5, object_initialize_vitality, object_index, new_vitality, new_shield_vitality);
	return;
}

bool __cdecl object_compute_change_colors(datum object_index)
{
	return INVOKE(0x13456E, 0x12343E, object_compute_change_colors, object_index);
}

// Wakes object by adding the _object_header_awake_bit to every object and all parents of that object
void __cdecl object_wake(datum object_index) 
{
	return INVOKE(0x12FA1E, 0x11E8E1, object_wake, object_index);
}

void __cdecl object_activate(datum object_index)
{
	INVOKE(0x13204A, 0x120F1A, object_activate, object_index);
	return;
}

void __cdecl object_deactivate(datum object_index)
{
	INVOKE(0x1320B8, 0x120F88, object_deactivate, object_index);
	return;
}

void __cdecl object_cleanup_havok(datum object_index)
{
	INVOKE(0xA2778, 0x949D8, object_cleanup_havok, object_index);
	return;
}

void object_reset_interpolation(datum object_index)
{
	const object_datum* object = object_get_fast_unsafe(object_index);
	c_animation_manager* animation_manager = (c_animation_manager*)object_header_block_get(object_index, &object->animation_manager_block);

	animation_manager->interpolator_control_1.disable();
	object_wake(object_index);
	return;
}

void __cdecl object_compute_node_matrices_with_children(datum object_index)
{
	INVOKE(0x136924, 0x1257F4, object_compute_node_matrices_with_children, object_index);
	return;
}

bool object_can_activate_in_cluster(datum object_index, s_game_cluster_bit_vectors* cluster_activation)
{
	bool result = false;
	s_object_header* object_header = (s_object_header*)datum_get(object_header_data_get(), object_index);
	const object_datum* object = (object_datum*)object_header->object;

	if (object->object_flags.test(_object_always_active_bit))
	{
		return true;
	}
	if (TEST_BIT(FLAG(object_header->object_type), _object_type_machine) && object_type_compute_activation(object_index, cluster_activation, &result) || object_header->cluster_index == NONE)
	{
		return result;
	}
	return (TEST_FLAG(FLAG(object_header->cluster_index & 0x1F), cluster_activation[object_header->cluster_index >> 5].cluster_bitvector));
}


bool set_object_position_if_in_cluster(s_location* location, datum object_index)
{
	object_datum* object = object_get_fast_unsafe(object_index);
	scenario_location_from_point(location, &object->object_origin_point);

	if (location->cluster_index == NONE)
	{
		collision_bsp_test_sphere_result test_result;
		collision_bsp_test_sphere(global_collision_bsp_get(), 0, NULL, &object->object_origin_point, object->shadow_sphere_radius, &test_result);

		if (test_result.stack_depth)
		{
			scenario_location_from_leaf(location, test_result.buffer3.leaf_index);
		}
		else
		{
			scenario_location_from_point(location, &object->position);
		}
	}

	return location->cluster_index != NONE;
}

// Gets important info about the object and populates the s_object_payload argument with the appropriate data
void get_object_payload(datum object_index, s_object_payload* payload)
{
	const object_datum* object = object_get_fast_unsafe(object_index);
	uint16 object_collision_cull_flags = 0;
	if (object->object_flags.test(_object_uses_collidable_list_bit))
	{
		object_collision_cull_flags = collision_compute_object_cull_flags(object_index);
	}

	payload->object_type = object->object_identifier.get_type();
	payload->object_collision_cull_flags = object_collision_cull_flags;
	payload->origin_point = object->object_origin_point;
	payload->bounding_sphere_radius = object->shadow_sphere_radius;
	return;
}

// Reconnects an object to the current bsp that's loaded
void object_reconnect_to_map(s_location* location, datum object_index)
{
	s_object_header* object_header = (s_object_header *)datum_get(object_header_data_get(), object_index);
	object_datum* object = (object_datum *)object_header->object;

	bool cluster_index_is_null = object_header->cluster_index == NONE;
	s_location* p_location = location;

	if (!p_location)
	{
		s_location scnr_location;
		scenario_location_from_point(&scnr_location, &object->object_origin_point);
		p_location = &scnr_location;
		if (scnr_location.cluster_index == NONE)
		{
			scenario_location_from_point(&scnr_location, &object->position);
		}
	}

	if (p_location->cluster_index == NONE)
	{
		object->object_flags.set(_object_outside_of_map_bit, true);
	}
	else
	{
		object->location.leaf_index = p_location->leaf_index;
		object->location.cluster_index = p_location->cluster_index;
		object->location.bsp_index = p_location->bsp_index;
		object_header->cluster_index = p_location->cluster_index;
		object->object_flags.set(_object_outside_of_map_bit, false);
	}
	s_game_cluster_bit_vectors cluster_bitvector[16];
	s_game_cluster_bit_vectors* p_cluster_bitvector = NULL;
	bool cluster_overflow = false;
	if (object->object_flags.test(_object_cinematic_visibility_bit))
	{
		memset(cluster_bitvector, -1, 4 * ((get_global_structure_bsp()->clusters.size + 0x1F) >> 5));
		p_cluster_bitvector = cluster_bitvector;
	}

	s_object_payload payload;
	get_object_payload(object_index, &payload);

	cluster_partition* partition = collideable_object_cluster_partition_get();
	if (!object->object_flags.test(_object_uses_collidable_list_bit))
	{
		partition = noncollideable_object_cluster_partition_get();
	}

	cluster_partition_reconnect(partition,
		object_index,
		&object->first_cluster_reference,
		&object->object_origin_point,
		object->shadow_sphere_radius,
		&object->location,
		p_cluster_bitvector,
		20,
		&payload,
		&cluster_overflow);

	object->object_flags.set(_object_connected_to_map_bit, true);
	object_header->flags.set(_object_header_connected_to_map_bit, true);
	object_connect_lights_recursive(object_index, false, true, false, false);

	if (s_game_cluster_bit_vectors* cluster_activation = game_get_cluster_activation();
		object_can_activate_in_cluster(object_index, cluster_activation))
	{
		object_activate(object_index);
	}
	else if (!object->object_flags.test(_object_deleted_when_deactivated_bit)|| simulation_query_object_is_predicted(object_index))
	{
		object_deactivate(object_index);
	}
	else
	{
		object_delete(object_index);
	}
	if (cluster_index_is_null && object_header->cluster_index != NONE)
	{
		object_cleanup_havok(object_index);
	}

	return;
}

void object_postprocess_node_matrices(datum object_index)
{
	const object_datum* object = object_get_fast_unsafe(object_index);
	const object_definition* object_tag = (object_definition*)tag_get_fast(object->tag_definition_index);
	if (object_tag->model.TagIndex != NONE)
	{
		const s_model_definition* model_tag = (s_model_definition*)tag_get_fast(object_tag->model.TagIndex);
		if (model_tag->render_model.TagIndex != NONE && model_tag->animation.TagIndex != NONE)
		{
			int32 node_count;
			real_matrix4x3* node_matricies = object_get_node_matrices(object_index, &node_count);
			object_type_postprocess_node_matrices(object_index, node_count, node_matricies);
		}
	}
	return;
}

void __cdecl object_reconnect_to_physics(datum object_index)
{
	INVOKE(0x1323B3, 0x121282, object_reconnect_to_physics, object_index);
	return;
}

void __cdecl attachments_new(datum object_index)
{
	INVOKE(0x1348CA, 0x12379A, attachments_new, object_index);
	return;
}

void object_initialize_effects(datum object_index)
{
	const s_object_header* object_header = (s_object_header*)datum_get(object_header_data_get(), object_index);
	object_datum* object = (object_datum*)object_header->object;

	if (object_header->object_type == _object_type_projectile)
	{
		object->object_projectile_datum = NONE;
	}
	else
	{
		widgets_new(object_index);
	}
	attachments_new(object_index);
	return;
}

// Not used on dedicated servers
void object_occlusion_data_initialize(datum object_index)
{
	INVOKE(0x26CFDD, 0x0, object_occlusion_data_initialize, object_index);
	return;
}

void free_object_memory(datum object_index)
{
	s_object_header* object_header = (s_object_header*)datum_get(object_header_data_get(), object_index);
	object_header->flags = (e_object_header_flag)0;
	if (object_header->object != NULL)
	{
		memory_pool_block_free(get_object_table(), &object_header->object);
	}
	datum_delete(object_header_data_get(), object_index);
	return;
}


void object_initialize_for_interpolation(datum object_index)
{
	uint16 abs_object_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
	object_datum* object = object_get_fast_unsafe(object_index);
	object_definition* object_def = (object_definition*)tag_get_fast(object->tag_definition_index);

	if (object_def->model.TagIndex == NONE)
	{
		if (object_def->attachments.size <= 0)
		{
			return;
		}

		int32 tag_block_index = 0;
		while (1)
		{
			object_attachment_definition* attachment = object_def->attachments[tag_block_index];
			if (attachment->type.TagIndex != NONE)
			{
				blam_tag group = attachment->type.TagGroup;
				if (group.as_int() == 'lens'
					|| group.as_int() == 'ligh'
					|| group.as_int() == 'MGS2'
					|| group.as_int() == 'tdtl'
					|| group.as_int() == 'cont'
					|| group.as_int() == 'effe')
				{
					break;
				}
			}
			if (++tag_block_index >= object_def->attachments.size)
			{
				return;
			}
		}
	}

	int32 nodes_count;
	real_point3d center_of_mass;
	real_matrix4x3* matrices = (real_matrix4x3*)object_header_block_get_with_count(object_index, &object->nodes_block, sizeof(real_matrix4x3), &nodes_count);
	object_get_center_of_mass(object_index, &center_of_mass);
	halo_interpolator_object_populate_interpolation_data(object_index, matrices, nodes_count, &object->position, &object->forward, &object->up, &center_of_mass);
	return;
}

#define USE_REWRITTEN_OBJECT_NEW
// Creates a new object
datum __cdecl object_new(object_placement_data* placement_data)
{
#ifdef USE_REWRITTEN_OBJECT_NEW
	datum object_index = NONE;

	if (!placement_data->flags.test(_scenario_object_placement_bit_4) && placement_data->tag_index != NONE)
	{
		object_type_adjust_placement(placement_data);
	}

	if (placement_data->tag_index != NONE)
	{
	 	const object_definition* object_def = (object_definition*)tag_get_fast(placement_data->tag_index);
		const object_type_definition* object_type_definition = object_type_definition_get(object_def->object_type);
		const s_model_definition* model_definition = NULL;

		if (object_def->model.TagIndex != NONE)
		{
			model_definition = (s_model_definition*)tag_get_fast(object_def->model.TagIndex);
		}

		if (TEST_FLAG(FLAG(object_def->object_type), (FLAG(_object_type_creature) | FLAG(_object_type_crate) | FLAG(_object_type_machine) | FLAG(_object_type_vehicle) | FLAG(_object_type_biped))))
		{
			havok_memory_garbage_collect();
		}

		object_index = object_header_new(object_type_definition->datum_size);

		if (object_index != NONE)
		{
			halo_interpolator_setup_new_object(object_index);
			s_object_header* object_header = (s_object_header*)datum_get(object_header_data_get(), object_index);
			object_datum* object = object_get_fast_unsafe(object_index);

			object_header->flags.set(_object_header_post_update_bit, true);
			object_header->object_type = object_def->object_type;
			object->tag_definition_index = placement_data->tag_index;

			if (placement_data->object_identifier.get_source() == NONE)
			{
				object->object_identifier.create_dynamic(object_def->object_type);
				object->placement_index = NONE;
				object->structure_bsp_index = get_global_structure_bsp_index();
			}
			else
			{
				object->object_identifier = placement_data->object_identifier;
				object->placement_index = placement_data->placement_index;
				object->structure_bsp_index = placement_data->object_identifier.get_origin_bsp();
			}

			object->position = placement_data->position;
			object->forward = placement_data->forward;
			object->up = placement_data->up;
			object->translational_velocity = placement_data->translational_velocity;
			object->angular_velocity = placement_data->angular_velocity;
			object->scale = placement_data->scale;

			bool enable = placement_data->flags.test(_scenario_object_placement_bit_0);
			object->object_flags.set(_object_mirrored_bit, enable);
			enable = model_definition && model_definition->collision_model.TagIndex != NONE;
			object->object_flags.set(_object_uses_collidable_list_bit, enable);

			datum mode_index;
			datum coll_index;
			enable = object_is_prt_and_lightmapped(object_index, &mode_index, &coll_index);
			object->object_flags.set(_object_has_prt_or_lighting_info_bit, enable);
			
			object_header->cluster_index = NONE;
			location_invalidate(&object->location);
			object->first_cluster_reference = NONE;
			object->parent_index = NONE;
			object->next_index = NONE;
			object->current_weapon_datum = NONE;
			object->name_list_index = NONE;
			object->netgame_equipment_index = NONE;
			object->byte_108 = NONE;
			object->byte_109 = NONE;
			object->placement_policy = placement_data->placement_policy;
			if (TEST_FLAG(object_def->flags, _object_definition_does_not_cast_shadow))
			{
				object->object_flags.set(_object_shadowless_bit, true);
			}

			if (object->object_flags.test(_object_hidden_bit))
			{
				object->object_flags.set(_object_hidden_bit, false);
				if (object_is_connected_to_map(object_index))
				{
					object_connect_lights_recursive(object_index, false, true, false, false);
				}
				object_update_collision_culling(object_index);
			}

			object->damage_owner_target_model_abs_index = placement_data->damage_owner.target_model_abs_index;
			object->damage_owner_owner_index = placement_data->damage_owner.owner_index;
			object->damage_owner_object_index = placement_data->damage_owner.entity_index;
			object->model_variant_id = NONE;
			object->cached_object_render_state_index = NONE;
			object->field_D0 = NONE;
			object->physics_flags.set_raw_bits(0);
			object->physics_flags.set(_object_physics_bit_8, placement_data->flags.test(_scenario_object_placement_bit_3));
			object->havok_datum = NONE;
			object->simulation_entity_index = -1;
			object->attached_to_simulation = 0;
			object->destroyed_constraints_flag = placement_data->destroyed_constraints_flag;
			object->loosened_constraints_flag = placement_data->loosened_constraints_flag;

			uint32 nodes_count = 1;
			uint32 collision_regions_count = 1;
			uint32 damage_info_damage_sections_size = 0;

			bool allow_interpolation = false;
			bool valid_animation_manager = false;

			if (model_definition)
			{
				if (model_definition->nodes.size >= 1) 
				{ 
					nodes_count = model_definition->nodes.size; 
				}
				if (model_definition->collision_regions.size >= 1) 
				{ 
					collision_regions_count = model_definition->collision_regions.size; 
				}

				if (model_definition->new_damage_info.size > 0 && model_definition->new_damage_info.data != NONE)
				{
					damage_info_damage_sections_size = model_definition->new_damage_info[0]->damage_sections.size;
				}

				if (model_definition->animation.TagIndex != NONE)
				{
					c_animation_manager animation_manager;
					if (animation_manager.reset_graph(model_definition->animation.TagIndex, object_def->model.TagIndex, true))
					{
						valid_animation_manager = true;
						allow_interpolation = !TEST_FLAG(FLAG(object_def->object_type), (
							FLAG(_object_type_sound_scenery) |
							FLAG(_object_type_light_fixture) |
							FLAG(_object_type_control) |
							FLAG(_object_type_machine) |
							FLAG(_object_type_scenery) |
							FLAG(_object_type_projectile))
						);

						// allow interpolation if object is device and device flags include interpolation
						if (TEST_FLAG(FLAG(object_def->object_type), (FLAG(_object_type_light_fixture) | FLAG(_object_type_control) | FLAG(_object_type_machine))))
						{
							_device_definition* device_def = (_device_definition*)tag_get_fast(placement_data->tag_index);
							if (TEST_FLAG(device_def->flags, _device_definition_allow_interpolation))
							{
								allow_interpolation = true;
							}
						}
					}
				}
			}
			
			int16 orientation_size = (!allow_interpolation ? 0 : 32 * nodes_count);

			// Allocate object header blocks
			bool can_create_object = object_header_block_allocate(object_index, offsetof(object_datum, object_datum::object_attachments_block), (int16)8 * object_def->attachments.size, 0);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object_datum::damage_sections_block), 8 * damage_info_damage_sections_size, 0);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object_datum::change_color_block), (int16)24 * object_def->change_colors.size, 0);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object_datum::nodes_block), 52 * nodes_count, 0);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object_datum::collision_regions_block), 10 * collision_regions_count, 0);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object_datum::node_orientation_block), orientation_size, 4);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object_datum::original_orientation_block), orientation_size, 4);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object_datum::animation_manager_block), (valid_animation_manager ? 144 : 0), 0);
			can_create_object &= havok_can_allocate_space_for_instance_of_object_definition(placement_data->tag_index);

			// If one of the object headers cannot be allocated then something has gone horribly wrong and we can't create our object
			bool out_of_objects = !can_create_object;
			if (can_create_object)
			{
				if (valid_animation_manager)
				{
					c_animation_manager* animation_manager = (c_animation_manager*)object_header_block_get(object_index, &object->animation_manager_block);
					animation_manager->initialize();
					bool graph_reset = animation_manager->reset_graph(model_definition->animation.TagIndex, object_def->model.TagIndex, true);
					object->object_flags.set(_object_data_bit_11, graph_reset);
				}

				// Null attachment block
				if (object_def->attachments.size > 0)
				{
					int32 attachments_count;
					object_attachment* object_attachments_block = (object_attachment*)object_header_block_get_with_count(object_index,
						&object->object_attachments_block,
						sizeof(object_attachment_definition),
						&attachments_count);

					memset(object_attachments_block, NONE, sizeof(object_attachment) * attachments_count);
				}
				
				if (object_type_new(object_index, placement_data, &out_of_objects))
				{
					bool object_flag_check = object->object_flags.test(_object_deleted_when_deactivated_bit);
					if (placement_data->flags.test(_scenario_object_placement_bit_1) || placement_data->flags.test(_scenario_object_placement_bit_2))
					{
						object->object_flags.set(_object_deleted_when_deactivated_bit, false);
					}

					update_object_variant_index(object_index, placement_data->variant_name);
					update_object_region_information(object_index, placement_data->region_index);
					object_set_initial_change_colors(object_index, placement_data->active_change_colors_mask, placement_data->change_colors);
					object_initialize_vitality(object_index, NULL, NULL);
					object_compute_change_colors(object_index);
					object->emblem_info = placement_data->emblem_info;

					if (object->animation_manager_block.offset != NONE)
					{
						object_reset_interpolation(object_index);
					}

					object_compute_node_matrices_with_children(object_index);

					// If the object (can) connect to the map we make sure it gets connected
					if (objects_can_connect_to_map())
					{
						placement_data->object_is_inside_cluster = set_object_position_if_in_cluster(&placement_data->location, object_index);
						
						// If the object is inside a cluster set the location to the one passed in the placement data
						// If not then pass null
						s_location* p_location = (placement_data->object_is_inside_cluster ? &placement_data->location : NULL);
						object_reconnect_to_map(p_location, object_index);
					}

					object_postprocess_node_matrices(object_index);
					object_initialize_for_interpolation(object_index);
					object_wake(object_index);

					object->physics_flags.set(_object_physics_bit_2, placement_data->flags.test(_scenario_object_placement_bit_5));

					object_reconnect_to_physics(object_index);
					object_initialize_effects(object_index);
					object_type_create_children(object_index);

					if (object_def->creation_effect.TagIndex != NONE)
					{
						effect_new_from_object(object_def->creation_effect.TagIndex, &placement_data->damage_owner, object_index, 0.0f, 0.0f, NULL, NULL);
					}

					// Not 100% sure what this function does but it has to do with occlusion
					// This function is nulled out on the dedi
					if (!Memory::IsDedicatedServer()) 
					{ 
						object_occlusion_data_initialize(object_index);
					}

					object->object_flags.set(_object_deleted_when_deactivated_bit, object_flag_check);
					object_early_mover_new(object_index);

					if (object->object_flags.test(_object_deleted_when_deactivated_bit) && !object_header->flags.test(_object_header_active_bit))
					{ 
						if (objects_can_connect_to_map())
						{
							if (!placement_data->flags.test(_scenario_object_placement_bit_1) && (!placement_data->flags.test(_scenario_object_placement_bit_2) || object->location.cluster_index != NONE))
							{
								object_delete(object_index);
							}
						}
					}
				}
				else
				{
					object_type_delete(object_index);
					free_object_memory(object_index);
					object_index = NONE;
				}
			}
			else
			{
				free_object_memory(object_index);
				object_index = NONE;
			}
		}
	}
	
	return object_index;
#else
	return INVOKE(0x136CA7, 0x125B77, object_new, placement_data);
#endif
}

void __cdecl object_delete(datum object_index)
{
	INVOKE(0x136005, 0x124ED5, object_delete, object_index);
	return;
}

real_point3d* __cdecl object_get_center_of_mass(datum object_index, real_point3d* point)
{
	return INVOKE(0x132A23, 0x1218F3, object_get_center_of_mass, object_index, point);
}

real_point3d* object_get_origin_interpolated(datum object_index, real_point3d* point_out)
{
	real_point3d point;
	real_point3d interpolated_object_position;
	object_datum* object = object_get_fast_unsafe(object_index);
	if (halo_interpolator_interpolate_object_position(object_index, &interpolated_object_position))
	{
		point = interpolated_object_position;
	}
	else
	{
		point = object->position;
		interpolated_object_position = object->position;
	}

	if (object->parent_index == NONE)
	{
		*point_out = point;
	}
	else
	{
		real_matrix4x3 interpolated_matrix;
		real_matrix4x3* transform_matrix = &interpolated_matrix;
		if (!halo_interpolator_interpolate_object_node_matrix(object->parent_index, object->matrix_index, &interpolated_matrix))
		{
			transform_matrix = object_get_node_matrix(object->parent_index, object->matrix_index);
		}
		matrix4x3_transform_point(transform_matrix, &interpolated_object_position, point_out);
	}
	return point_out;
}

real_matrix4x3* object_get_node_matrix(datum object_index, int16 node_index)
{
	real_matrix4x3* nodes = (real_matrix4x3*)object_header_block_get(object_index, &object_get_fast_unsafe(object_index)->nodes_block);
	return &nodes[node_index];
}

real_matrix4x3* object_get_node_matrices(datum object_datum, int32* out_node_count)
{
	return (real_matrix4x3*)object_header_block_get_with_count(object_datum, &object_get_fast_unsafe(object_datum)->nodes_block, sizeof(real_matrix4x3), out_node_count);
}

datum object_get_damage_owner(datum object_index)
{
	object_datum* object = (object_datum*)object_try_and_get_and_verify_type(object_index, -1);
	return object->damage_owner_object_index;
}

void __cdecl object_apply_function_overlay_node_orientations(datum object_index, 
	render_model_definition* render_model, 
	c_animation_manager* animation_manager, 
	int32 a4, 
	int32 orientation_count, 
	real_orientation* orientations)
{
	INVOKE(0x134214, 0x1230E4, object_apply_function_overlay_node_orientations, object_index, render_model, animation_manager, a4, orientation_count, orientations);
	return;
}

real_point3d* __cdecl object_get_center_of_mass_interpolated(datum object_index, real_point3d* center_of_mass)
{
	real_point3d* result;
	if (halo_interpolator_interpolate_center_of_mass(object_index, center_of_mass))
	{
		result = center_of_mass;
	}
	else
	{
		result = object_get_center_of_mass(object_index, center_of_mass);
	}
	return result;
}

datum __cdecl object_get_parent_recursive(datum parent_index)
{
	return INVOKE(0x132574, 0x121444, object_get_parent_recursive, parent_index);
}

typedef void(__cdecl* t_object_move_t)(int);
t_object_move_t p_object_move;
void __cdecl object_move_hook(int a1)
{
	p_object_move(a1);
	object_initialize_for_interpolation(a1);
}

typedef void(__cdecl* t_object_update)(datum object_index);
t_object_update p_object_update;

void __cdecl object_update_hook(datum object_index)
{
	p_object_update(object_index);
	object_initialize_for_interpolation(object_index);
}

int16 __cdecl internal_object_get_markers_by_string_id(datum object_index, string_id marker, object_marker* marker_object, int16 count, bool is_unit)
{
	int32 marker_index = 0;

	datum index = object_index;
	if (!is_unit)
	{
		index = object_get_parent_recursive(index);
	}
	if (index != NONE)
	{
		object_datum* object = object_get_fast_unsafe(index);
		object_definition* object_def = (object_definition*)tag_get_fast(object->tag_definition_index);
		if (object_def->model.TagIndex != NONE)
		{
			s_model_definition* model_def = (s_model_definition*)tag_get_fast(object_def->model.TagIndex);

			int32 node_count;
			real_matrix4x3* node_matrices;
			if (!halo_interpolator_interpolate_object_node_matrices(object_index, &node_matrices, &node_count))
			{
				node_matrices = object_get_node_matrices(object_index, &node_count);
			}

			void* collision_regions = (real_matrix4x3*)object_header_block_get(object_index, &object->collision_regions_block);

			int32 marker_index = render_model_get_markers_by_name(model_def->render_model.TagIndex, 
				marker, 
				collision_regions, 
				NONE, 
				NULL, 
				node_count, 
				node_matrices, 
				object->object_flags.test(_object_mirrored_bit), 
				marker_object,
				count);

			if (marker_index)
			{
				return marker_index;
			}
		}
	}

	object_datum* object = object_get_fast_unsafe(object_index);
	marker_object->node_index = 0;
	matrix4x3_identity(&marker_object->matrix0);
		
	if (halo_interpolator_interpolate_object_node_matrix(object_index, 0, &marker_object->matrix1))
	{
		marker_object->matrix1 = marker_object->matrix1;
	}
	else
	{
		marker_object->matrix1 = *object_get_node_matrix(object_index, 0);
	}
		
	marker_object->field_6C = 0;
	if (object->object_flags.test(_object_mirrored_bit))
	{
		marker_object->matrix1.vectors.left.i = -marker_object->matrix1.vectors.left.i;
		marker_object->matrix1.vectors.left.j = -marker_object->matrix1.vectors.left.j;
		marker_object->matrix1.vectors.left.k = -marker_object->matrix1.vectors.left.k;
	}

	return (marker.get_id() ? marker_index : 1);
}

// Replace calls to internal_object_get_markers_by_string_id
void internal_object_get_markers_by_string_id_replace_calls(void)
{
	PatchCall(Memory::GetAddress(0x132792, 0x121662), internal_object_get_markers_by_string_id);

	PatchCall(Memory::GetAddress(0x1327B1, 0x121681), internal_object_get_markers_by_string_id);
	PatchCall(Memory::GetAddress(0x134C26, 0x123AF6), internal_object_get_markers_by_string_id);
	PatchCall(Memory::GetAddress(0x13823D, 0x12710D), internal_object_get_markers_by_string_id);
	PatchCall(Memory::GetAddress(0x138257, 0x1214CB), internal_object_get_markers_by_string_id);

	return;
}

// Replace calls to object_new with our own
void object_new_replace_calls(void)
{
	PatchCall(Memory::GetAddress(0x3B603, 0x3F34B), object_new);
	PatchCall(Memory::GetAddress(0x52A2E, 0x5AF3E), object_new);
	PatchCall(Memory::GetAddress(0x55C06, 0x5E0FE), object_new);
	PatchCall(Memory::GetAddress(0x5D6EB, 0x6F074), object_new);
	PatchCall(Memory::GetAddress(0x70573, 0x8E454), object_new);
	PatchCall(Memory::GetAddress(0xA8C90, 0x9AD10), object_new);
	PatchCall(Memory::GetAddress(0xD4B8C, 0xD28A1), object_new);
	PatchCall(Memory::GetAddress(0x10D6AF, 0xD9C5F), object_new);
	PatchCall(Memory::GetAddress(0x1102AF, 0xDC85F), object_new);
	PatchCall(Memory::GetAddress(0x138057, 0x126F27), object_new);
	PatchCall(Memory::GetAddress(0x1385AD, 0x12747D), object_new);
	PatchCall(Memory::GetAddress(0x13E3A8, 0x12D1F7), object_new);
	PatchCall(Memory::GetAddress(0x142B85, 0x1319D5), object_new);
	PatchCall(Memory::GetAddress(0x144B89, 0x1339D9), object_new);
	PatchCall(Memory::GetAddress(0x144F2E, 0x133D7E), object_new);
	PatchCall(Memory::GetAddress(0x15D4D1, 0x141791), object_new);
	PatchCall(Memory::GetAddress(0x166787, 0x15151C), object_new);
	PatchCall(Memory::GetAddress(0x16B931, 0x15C247), object_new);
	PatchCall(Memory::GetAddress(0x16BD2C, 0x1613F1), object_new);
	PatchCall(Memory::GetAddress(0x179CBC, 0x1617EC), object_new);
	PatchCall(Memory::GetAddress(0x1F32F2, 0x1DE38B), object_new);
	PatchCall(Memory::GetAddress(0x317863, 0x2C25CD), object_new);
	PatchCall(Memory::GetAddress(0x3178A3, 0x2C260D), object_new);
	PatchCall(Memory::GetAddress(0x318DEC, 0x2C3B56), object_new);
	PatchCall(Memory::GetAddress(0x33992A, 0x2E469A), object_new);
	PatchCall(Memory::GetAddress(0x3438C0, 0x2EE630), object_new);
	PatchCall(Memory::GetAddress(0x355E03, 0x300B73), object_new);
	PatchCall(Memory::GetAddress(0x358E31, 0x303BA1), object_new);
	return;
}

void objects_apply_patches(void)
{
#ifdef USE_REWRITTEN_OBJECT_NEW
	object_new_replace_calls();
	//DETOUR_ATTACH(p_object_move, Memory::GetAddress<t_object_move_t>(0x137E6D, 0), object_move_hook);
	DETOUR_ATTACH(p_object_update, Memory::GetAddress<t_object_update>(0x135219, 0), object_update_hook);
#endif
	internal_object_get_markers_by_string_id_replace_calls();
	return;
}