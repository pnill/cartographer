#include "stdafx.h"
#include "objects.h"

#include "object_definition.h"
#include "object_early_movers.h"
#include "object_globals.h"
#include "object_types.h"

#include "animations/animation_manager.h"
#include "cache/cache_files.h"
#include "devices/device_definitions.h"
#include "effects/effects.h"
#include "game/game.h"
#include "game/game_time.h"
#include "items/weapons.h"
#include "main/interpolator.h"
#include "memory/memory_pool.h"
#include "models/models.h"
#include "models/model_definitions.h"
#include "objects/lights.h"
#include "objects/widgets/widgets.h"
#include "physics/collision_bsp.h"
#include "physics/collisions.h"
#include "physics/havok.h"
#include "physics/havok_memory.h"
#include "scenario/scenario.h"
#include "shell/shell.h"
#include "simulation/simulation.h"
#include "structures/cluster_partitions.h"
#include "structures/structure_bsp_definitions.h"
#include "tag_files/tag_files.h"
#include "units/units.h"

/* constants */

enum
{
	k_maximum_object_override_count = 16
};

/* typedefs */

typedef datum(__cdecl* t_object_new)(object_placement_data* data);

/* structures */

#ifdef OBJECT_OVERRIDE_ENABLED
struct s_object_override_data
{
	bool valid;
	int8 pad[3];
	datum object_index;
	datum shader_tag_index;
};
#endif

#ifdef OBJECT_DEBUG
struct dump_datum
{
	int32 definition_index;
	e_object_type object_type;
	int16 maximum_size;
	int32 total_size;
	int16 count;
	int16 active_count;
	int16 garbage_count;
	int16 dead_count;
	int16 outside_map_count;
	int16 at_rest_count;
};
#endif

/* prototypes */

static s_memory_pool* object_memory_pool_get(void);

static datum** game_state_object_name_list_get(void);

static void objects_apply_interpolation_patches(void);

static void object_move_replace_calls(void);

// Replace calls to object_new with our own
static void object_new_replace_calls(void);

// Replace calls to functions getting the marker by string id
static void object_get_markers_by_string_id_replace_calls(void);

static void __cdecl object_move(datum object_index);

static datum object_allocate_header(datum tag_definition_index);

static void object_header_delete(datum object_index);

static void object_postprocess_node_matrices(datum object_index);

static void object_initialize_effects(datum object_index);

// Not used on dedicated servers
static void object_occlusion_data_initialize(datum object_index);

static void __cdecl object_cleanup_havok(datum object_index);

static bool object_find_initial_location(datum object_index, s_location* location);

static bool __cdecl object_compute_change_colors(datum object_index);

static void __cdecl update_object_region_information(datum object_datum, int32 region_index);

// Updates variant index from variant string id
static void object_variant_index_update(datum object_index, string_id variant);

// Returns the index of the variant block inside the model tag of the passed object_index
static int8 __cdecl object_lookup_variant_index_from_name(datum object_index, string_id variant);

static bool object_can_activate_in_cluster(datum object_index, s_game_cluster_bit_vectors* cluster_activation);

// Reconnects an object to the current bsp that's loaded
static void object_reconnect_to_map(datum object_index, s_location* location);

// Gets important info about the object and populates the s_object_payload argument with the appropriate data
static void object_get_payload(datum object_index, s_object_payload* payload);

static void __cdecl object_connect_lights_recursive(datum object_datum, bool disconnect_this_object, bool reconnect_this_object, bool reconnect_child_objects, bool show_this_object);

static void __cdecl attachments_new(datum object_index);

static bool __cdecl object_update(datum object_index);

static void object_initialize_for_interpolation(datum object_index);

static void object_reset_interpolation(datum object_index);

static int16 __cdecl internal_object_get_markers_by_string_id(datum object_index, string_id marker, object_marker* marker_object, int16 count, bool is_unit);

static void object_disconnect_from_map(datum object_index, bool disconnect_this_object, bool reconnect_this_object);

#ifdef OBJECT_OVERRIDE_ENABLED
static s_object_override_data* object_override_data_get(size_t override_index);

static int32 object_find_override_index(datum object_index);

static int32 object_get_override_index(datum object_index);
#endif

#ifdef OBJECT_DEBUG
static void object_add_to_dump(datum object_index, dump_datum* dump);

static void object_dump_print_info(_iobuf* handle, const dump_datum* dump);

static int sort_dumps(const dump_datum* dump1, const dump_datum* dump2);
#endif

/* globals */

static t_object_new p_object_new;

#ifdef OBJECT_OVERRIDE_ENABLED
static s_object_override_data g_object_override_data[k_maximum_object_override_count];
#endif

#ifdef OBJECT_DEBUG
static int32 debug_last_out_of_memory_dump_game_time;
static int32 g_object_memory_dump_number;
#endif

/* public code */

data_array* object_header_data_get(void)
{
	return *Memory::GetAddress<data_array**>(0x4E461C, 0x50C8EC);
};

void objects_apply_patches(void)
{
	objects_apply_interpolation_patches();

	s_game_systems* g_game_systems = get_game_systems();
	WritePointer((uintptr_t)&g_game_systems[28].initialize_for_new_map_proc, objects_initialize_for_new_map);
	return;
}

void c_object_iterator_base::object_iterator_begin_internal(int32 type_flags, uint8 flags)
{
	data_verify(object_header_data_get());
	m_iterator.signature = 0x86868686;
	m_iterator.type_flags = type_flags != 0 ? type_flags : _object_mask_all;
	m_iterator.flags = flags;
	m_iterator.absolute_index = 0;
	m_iterator.index = NONE;
	return;
}

datum c_object_iterator_base::get_index(void) const
{
	return m_iterator.index;
}

void __cdecl objects_initialize_for_new_map(void)
{
	damage_initialize_for_new_map();
	widgets_initialize_for_new_map();
	object_types_initialize_for_new_map();
	lights_initialize_for_new_map();
	data_make_valid(object_header_data_get());
	csmemset(*game_state_object_name_list_get(), NONE, 2560);
	cluster_partition_make_valid(collideable_object_cluster_partition_get());
	cluster_partition_make_valid(noncollideable_object_cluster_partition_get());
	s_object_globals* object_globals = object_globals_get();
	object_globals->objects_updating = false;
	object_globals->field_8 = NONE;
	object_globals->active_garbage_object_count = 0;
	object_globals->field_C = 0;
	object_globals->field_10 = 0;
	object_globals->total_game_time_ticks = 0;
	object_globals->object_custom_animations_prevent_lipsync_head_movement = false;
	object_globals->object_custom_animations_hold_on_last_frame = false;

#ifdef OBJECT_OVERRIDE_ENABLED
	csmemset(g_object_override_data, 0, sizeof(g_object_override_data));
#endif
#ifdef OBJECT_DEBUG
	debug_last_out_of_memory_dump_game_time = NONE;
#endif
	return;
}

void* __cdecl object_try_and_get_and_verify_type(datum object_index, int32 object_type_flags)
{
	return INVOKE(0x1304E3, 0x11F3A6, object_try_and_get_and_verify_type, object_index, object_type_flags);
}

void __cdecl object_iterator_new(object_iterator* iterator, int32 type_flags, uint8 flags)
{
	INVOKE(0x130547, 0x11F40A, object_iterator_new, iterator, type_flags, flags);
	return;
}

void* __cdecl object_iterator_next(object_iterator* iterator)
{
	return INVOKE(0x130574, 0x11F437, object_iterator_next, iterator);
}

void* object_header_block_get(datum object_index, const object_header_block_reference* reference)
{
	object_header_datum* object_header = object_header_get(object_index);
	void* object = object_get(object_index);

	ASSERT(reference->offset > 0);
	ASSERT(reference->size > 0);
	ASSERT(reference->offset + reference->size <= object_header->data_size);
	return (uint8*)object + reference->offset;
}

void* object_header_block_get_with_count(datum object_index, const object_header_block_reference* reference, uint32 element_size, int32* element_count)
{
	ASSERT(element_count);

	void* block;
	if (reference->offset == NONE)
	{
		block = NULL;
		*element_count = 0;
	}
	else
	{
		block = (void*)object_header_block_get(object_index, reference);
		
		ASSERT(reference->size % element_size == 0);

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

int32 __cdecl object_index_from_name_index(int16 name_index)
{
	return INVOKE(0x1335FB, 0x0, object_index_from_name_index, name_index);
}

bool object_is_connected_to_map(datum object_index)
{
	const datum parent = object_get_ultimate_parent(object_index);
	return object_header_get(parent)->flags.test(_object_header_connected_to_map_bit);
}

void __cdecl object_update_collision_culling(datum object_datum)
{
	INVOKE(0x1324E9, 0x1213B9, object_update_collision_culling, object_datum);
	return;
}

bool object_header_block_allocate(datum object_index, int16 block_reference_offset, int16 size, uint16 alignment_bits)
{
	bool result = false;
	object_header_datum* object_header = object_header_get(object_index);

	ASSERT(size >= 0);
	ASSERT(object_header->data_size + size <= SHORT_MAX);
	ASSERT(alignment_bits >= 0 && alignment_bits < 8);

	ASSERT(block_reference_offset >= 0);
	ASSERT(block_reference_offset + sizeof(struct object_header_block_reference) <= (uint16)object_header->data_size);
	
	if (size > 0)
	{
		const int32 alignment_size = (1 << alignment_bits) - 1;
		const int32 padded_size = alignment_size + size;
		ASSERT(padded_size >= size);
		if (memory_pool_block_reallocate(object_memory_pool_get(), (void**)&object_header->datum, padded_size + object_header->data_size))
		{
			const int32 offset = object_header->data_size;
			object_header->data_size = (int16)(padded_size + offset);
			
			object_header_block_reference* block = (object_header_block_reference*)(
				block_reference_offset + (int8*)object_get(object_index)
			);
			block->offset = ~(uint16)alignment_size & (alignment_size + offset);
			block->size = size;

			ASSERT(block->offset >= offset);
			ASSERT((block->offset - offset) + size <= padded_size);
			csmemset((uint8*)object_header->datum + offset, 0, padded_size);
			result = true;
		}
	}
	else
	{
		object_header_block_reference* block = (object_header_block_reference*)(
			block_reference_offset + (int8*)object_get(object_index)
		);
		block->offset = NONE;
		block->size = 0;
		result = true;
	}
	return result;
}

e_object_type object_get_type(datum object_index)
{
	return (e_object_type)object_header_get(object_index)->type;
}

bool __cdecl object_is_prt_and_lightmapped(datum object_datum, datum* mode_tag_index, datum* coll_tag_index)
{
	return INVOKE(0x131B0E, 0x1209DE, object_is_prt_and_lightmapped, object_datum, mode_tag_index, coll_tag_index);
}

pixel32 __cdecl object_set_initial_change_colors(datum object_index, uint32 change_color_override_mask, real_rgb_color* colour)
{
	return INVOKE(0x1310F9, 0x11FFBD, object_set_initial_change_colors, object_index, change_color_override_mask, colour);
}

void __cdecl object_initialize_vitality(datum object_index, const real32* new_vitality, const real32* new_shield_vitality)
{
	INVOKE(0x175A62, 0x14D2C5, object_initialize_vitality, object_index, new_vitality, new_shield_vitality);
	return;
}

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

void __cdecl object_compute_node_matrices_with_children(datum object_index)
{
	INVOKE(0x136924, 0x1257F4, object_compute_node_matrices_with_children, object_index);
	return;
}

void __cdecl object_reconnect_to_physics(datum object_index)
{
	INVOKE(0x1323B3, 0x121283, object_reconnect_to_physics, object_index);
	return;
}

void __cdecl objects_update(void)
{
	INVOKE(0x135EBF, 0x124D8F, objects_update);
	return;
}

void __cdecl objects_move(void)
{
	INVOKE(0x1380F6, 0x126FC6, objects_move);
	return;
}

datum __cdecl object_new(object_placement_data* data)
{
	datum object_index = NONE;
	bool out_of_objects = false;

	assert_valid_real_point3d(&data->position);
	assert_valid_real_vector3d_axes2(&data->forward, &data->up);
	assert_valid_real_vector3d(&data->angular_velocity);
	assert_valid_real_vector3d(&data->translational_velocity);
	
	ASSERT(data->scale > 0.f);

	if (!TEST_BIT(data->flags, 4) && data->definition_index != NONE)
	{
		object_type_adjust_placement(data);
	}

	if (data->definition_index != NONE)
	{
		const bool process_is_game_client = !shell_is_dedicated_server();

		struct object_definition const* object_definition = object_definition_get(data->definition_index);
		s_model_definition const* model_definition = NULL;

		const datum object_model_index = object_definition->object.model.index;
		if (object_model_index != NONE)
		{
			model_definition = model_definition_get(object_model_index);
		}

		if (data->object_identifier.is_scenario_object())
		{
			const datum found_object_index = data->object_identifier.find_object_index();
			if (found_object_index != NONE)
			{
				DISPLAY_ASSERT(g_static_string_assert_text.print("object_new creating duplicate of 0x%x", found_object_index));
			}
		}

		if (TEST_BIT(_object_mask_havok, object_definition->object.object_type))
		{
			havok_memory_garbage_collect();
		}

		object_index = object_allocate_header(data->definition_index);
		
		if (object_index == NONE)
		{
			out_of_objects = true;
		}
		else
		{
			halo_interpolator_setup_new_object(object_index);
			object_header_datum* object_header = object_header_get(object_index);
			object_datum* object = object_get(object_index);

			bool object_animation_and_attachments_cleared = false;
			bool object_type_created = false;
			bool can_create_object = true;
			bool valid_animation_manager = false;

			object_header->flags.set(_object_header_post_update_bit, true);
			object_header->type = (int8)object_definition->object.object_type;
			object->definition_index = data->definition_index;

			if (data->object_identifier.get_source() == NONE)
			{
				object->object.object_identifier.create_dynamic((e_object_type)object_definition->object.object_type);
				object->object.placement_index = NONE;
				object->object.structure_bsp_index = (uint8)get_global_structure_bsp_index();
			}
			else
			{
				if (data->object_identifier.is_scenario_object())
				{
					ASSERT(data->scenario_datum_index != NONE);
				}
				ASSERT(data->object_identifier.get_type() == object_definition->object.object_type);
				object->object.object_identifier = data->object_identifier;
				object->object.placement_index = (int16)data->scenario_datum_index;
				object->object.structure_bsp_index = (uint8)data->object_identifier.get_origin_bsp();
			}

			object->object.position = data->position;
			object->object.forward = data->forward;
			object->object.up = data->up;
			object->object.translational_velocity = data->translational_velocity;
			object->object.angular_velocity = data->angular_velocity;
			object->object.scale = data->scale;

			bool enable = TEST_BIT(data->flags, 0);
			object->object.flags.set(_object_mirrored_bit, enable);
			enable = model_definition && model_definition->collision_model.index != NONE;
			object->object.flags.set(_object_uses_collidable_list_bit, enable);

			datum mode_index;
			datum coll_index;
			enable = object_is_prt_and_lightmapped(object_index, &mode_index, &coll_index);
			object->object.flags.set(_object_has_prt_or_lighting_info_bit, enable);

			object_header->cluster_index = NONE;
			location_invalidate(&object->object.location);
			object->object.first_cluster_reference = NONE;
			object->object.parent_object_index = NONE;
			object->object.next_object_index = NONE;
			object->object.first_child_object_index = NONE;
			object->object.name_list_index = NONE;
			object->object.netgame_equipment_index = NONE;
			object->object.shield_damage_decay_timer = NONE;
			object->object.body_damage_decay_timer = NONE;
			object->object.placement_policy = data->placement_policy;

			if (object_definition->object.flags.test(_object_definition_does_not_cast_shadow_bit))
			{
				object->object.flags.set(_object_shadowless_bit, true);
			}

			object_set_hidden(object_index, false);

			object->object.damage_owner_target_model_abs_index = data->damage_owner.owner_team_index;
			object->object.damage_owner_owner_index = data->damage_owner.owner_player_index;
			object->object.damage_owner_object_index = data->damage_owner.owner_object_index;
			object->object.model_variant_id = NONE;
			object->object.cached_render_state_index = NONE;
			object->object.structure_bsp_fake_lightprobe_index = NONE;
			object->object.physics_flags.clear();
			object->object.physics_flags.set(_object_physics_bit_8, TEST_BIT(data->flags, 3));
			object->object.havok_datum = NONE;
			object->object.simulation_entity_index = NONE;
			object->object.simulation_flags.clear();
			object->object.destroyed_constraints_flag = data->destroyed_constraints_flag;
			object->object.loosened_constraints_flag = data->loosened_constraints_flag;

			uint32 node_count = 1;
			uint32 region_count = 1;
			uint32 damage_section_count = 0;
			bool can_interpolate = false;

			if (model_definition)
			{
				if (model_definition->runtime_nodes.count < 1)
				{
					error(
						_error_category_objects,
						_error_delayed,
						"object '%s' model '%s' has invalid node count %d!",
						tag_get_name(data->definition_index),
						tag_get_name(object_model_index),
						model_definition->runtime_nodes.count
					);
				}
				else
				{
					node_count = model_definition->runtime_nodes.count;
				}

				if (model_definition->runtime_regions.count < 1)
				{
					error(
						_error_category_objects,
						_error_delayed,
						"object '%s' model '%s' has invalid region count %d!",
						tag_get_name(data->definition_index),
						tag_get_name(object_model_index),
						model_definition->runtime_regions.count
					);
				}
				else
				{
					region_count = model_definition->runtime_regions.count;
				}

				if (model_definition->new_damage_info.count > 0 && model_definition->new_damage_info.data != NONE)
				{
					damage_section_count = model_definition->new_damage_info[0]->damage_sections.count;
				}

				if (model_definition->animation_graph.index != NONE)
				{
					c_animation_manager animation_manager;
					if (animation_manager.reset_graph(model_definition->animation_graph.index, object_model_index, true))
					{
						valid_animation_manager = true;
						can_interpolate = !TEST_FLAG(FLAG(object_definition->object.object_type), _object_mask_cannot_interpolate);

						// allow interpolation if object is device and device flags include interpolation
						if (TEST_FLAG(FLAG(object_definition->object.object_type), _object_mask_device))
						{
							struct device_definition const* device_definition = device_definition_get(data->definition_index);

							if (TEST_FLAG(device_definition->device.flags, _device_definition_allow_interpolation))
							{
								can_interpolate = true;
							}
						}
					}
					else
					{
						error(
							_error_category_animation,
							_error_delayed,
							"graph '%s' is not compatible with model '%s'",
							tag_name_strip_path(tag_get_name(model_definition->animation_graph.index)),
							tag_name_strip_path(tag_get_name(object_model_index))
						);
					}
				}
			}

			// Allocate object header blocks

			ASSERT(object_definition->object.attachments.count <= SHORT_MAX / sizeof(struct object_attachment));
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.object_attachments_block), (uint16)(sizeof(object_attachment) * object_definition->object.attachments.count), 0);
			
			ASSERT(damage_section_count <= SHORT_MAX / sizeof(struct object_damage_section));
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.damage_sections_block), (int16)(sizeof(object_damage_section) * damage_section_count), 0);
			

			ASSERT(object_definition->object.change_colors.count <= SHORT_MAX / (2 * sizeof(real_rgb_color)));
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.change_color_block), (int16)(2 * sizeof(real_rgb_color) * object_definition->object.change_colors.count), 0);
			

			ASSERT(node_count <= SHORT_MAX / (sizeof(real_matrix4x3)));
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.nodes_block), (int16)(sizeof(real_matrix4x3) * node_count), 0);


			ASSERT(region_count <= SHORT_MAX / (2 * sizeof(char) + sizeof(struct object_region_information)));
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.collision_regions_block), (int16)((2 * sizeof(char) + sizeof(struct object_region_information)) * region_count), 0);


			ASSERT(!can_interpolate || node_count <= SHORT_MAX / (sizeof(real_orientation)));
			const int16 orientation_size = (int16)(!can_interpolate ? 0 : 32 * node_count);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.original_orientation_block), orientation_size, 4);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.node_orientation_block), orientation_size, 4);
			can_create_object &= object_header_block_allocate(object_index, offsetof(object_datum, object.animation_manager_block), (valid_animation_manager ? 144 : 0), 0);
			can_create_object &= havok_can_allocate_space_for_instance_of_object_definition(data->definition_index);

			// If one of the object headers cannot be allocated then something has gone horribly wrong and we can't create our object
			out_of_objects = !can_create_object;

			// Get the object, we do this so we can verify that we're able to retrieve it in the future
			object = object_get(object_index);
			
			if (can_create_object)
			{
				if (valid_animation_manager)
				{
					ASSERT(model_definition);
					ASSERT(model_definition->animation_graph.index != NONE);
					ASSERT(object);

					c_animation_manager* animation_manager = (c_animation_manager*)object_header_block_get(object_index, &object->object.animation_manager_block);
					animation_manager->initialize();

					bool graph_reset = animation_manager->reset_graph(model_definition->animation_graph.index, object_model_index, true);
					ASSERT(graph_reset);

					object->object.flags.set(_object_dynamic_lighting_recompute_bit, graph_reset);
				}

				// Null attachment block
				if (object_definition->object.attachments.count > 0)
				{
					ASSERT(object);
					int32 attachments_count;
					object_attachment* object_attachments_block = (object_attachment*)object_header_block_get_with_count(object_index,
						&object->object.object_attachments_block,
						sizeof(object_attachment),
						&attachments_count);

					csmemset(object_attachments_block, NONE, sizeof(object_attachment) * attachments_count);
				}
				object_animation_and_attachments_cleared = true;
			}

			if (can_create_object)
			{
				can_create_object = object_type_new(object_index, data, &out_of_objects);
				object_type_created = true;
				// Get the object, we do this so we can verify that we're able to retrieve it in the future
				object = object_get(object_index);
			}

			if (can_create_object)
			{
				bool object_flag_check = object->object.flags.test(_object_deleted_when_deactivated_bit);
				if (TEST_BIT(data->flags, 1) || TEST_BIT(data->flags, 2))
				{
					object->object.flags.set(_object_deleted_when_deactivated_bit, false);
				}

				object_variant_index_update(object_index, data->variant_name);
				update_object_region_information(object_index, data->region_index);
				object_set_initial_change_colors(object_index, data->change_color_override_mask, data->change_color_overrides);
				object_initialize_vitality(object_index, NULL, NULL);
				object_compute_change_colors(object_index);
				object->object.emblem_info = data->emblem_info;

				if (object->object.animation_manager_block.offset != NONE)
				{
					object_reset_interpolation(object_index);
				}

				object_compute_node_matrices_with_children(object_index);

				// If the object (can) connect to the map we make sure it gets connected
				if (objects_can_connect_to_map())
				{
					data->location_valid = object_find_initial_location(object_index, &data->location);

					// If the object is inside a cluster set the location to the one passed in the placement data
					// If not then pass null
					s_location* initial_location = (data->location_valid ? &data->location : NULL);
					object_reconnect_to_map(object_index, initial_location);
				}

				object_postprocess_node_matrices(object_index);
				object_initialize_for_interpolation(object_index);

				object_wake(object_index);

				object->object.physics_flags.set(_object_was_in_local_physics_bit, TEST_BIT(data->flags, 5));

				object_reconnect_to_physics(object_index);
				object_initialize_effects(object_index);
				object_type_create_children(object_index);

				const datum object_creation_effect_index = object_definition->object.creation_effect.index;
				if (object_creation_effect_index != NONE)
				{
					effect_new_from_object(object_creation_effect_index, &data->damage_owner, object_index, 0.f, 0.f, NULL, NULL);
				}

				// Not 100% sure what this function does but it has to do with occlusion
				// This function is nulled out on the dedi
				if (process_is_game_client)
				{
					object_occlusion_data_initialize(object_index);
				}

				object->object.flags.set(_object_deleted_when_deactivated_bit, object_flag_check);
				object_early_mover_new(object_index);

				if (
					object->object.flags.test(_object_deleted_when_deactivated_bit) && 
					!object_header->flags.test(_object_header_active_bit) && 
					objects_can_connect_to_map() && 
					!TEST_BIT(data->flags, 1))
				{
					if (TEST_BIT(data->flags, 2))
					{
						if (object->object.location.cluster_index != NONE)
						{
							object_delete(object_index);
						}
					}
					else
					{
						object_delete(object_index);
					}
				}
			}

			if (!can_create_object)
			{
				if (object_type_created)
				{
					object_type_delete(object_index);
				}
				object_header_delete(object_index);
				object_index = NONE;
			}
		}
	}

	if (object_index == NONE && data->definition_index != NONE)
	{
		error(
			_error_category_objects,
			_error_immediate,
			"%s: cannot create %s",
			out_of_objects ? "OUT OF OBJECTS" : "OBJECT CREATION FAILED",
			tag_name_strip_path(tag_get_name(data->definition_index))
		);

#ifdef OBJECT_DEBUG
		if (debug_last_out_of_memory_dump_game_time == NONE || (debug_last_out_of_memory_dump_game_time + game_seconds_integer_to_ticks(5) >= (int32)game_time_get()))
		{
			objects_dump_memory();
			debug_last_out_of_memory_dump_game_time = game_time_get();
		}
#endif
	}

	return object_index;
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

void __cdecl object_get_origin(datum object_index, real_point3d* point_out, bool interpolated)
{
	real_point3d point;
	real_point3d interpolated_object_position;
	const object_datum* object = object_get(object_index);
	if (interpolated && halo_interpolator_interpolate_object_position(object_index, &interpolated_object_position))
	{
		point = interpolated_object_position;
	}
	else
	{
		point = object->object.position;
		interpolated_object_position = object->object.position;
	}

	if (object->object.parent_object_index == NONE)
	{
		*point_out = point;
	}
	else
	{
		real_matrix4x3 interpolated_matrix;
		real_matrix4x3* transform_matrix = &interpolated_matrix;
		if (!interpolated || !halo_interpolator_interpolate_object_node_matrix(object->object.parent_object_index, object->object.matrix_index, &interpolated_matrix))
		{
			transform_matrix = object_get_node_matrix(object->object.parent_object_index, object->object.matrix_index);
		}
		matrix4x3_transform_point(transform_matrix, &interpolated_object_position, point_out);
	}
	return;
}

void __cdecl object_get_origin_interpolated(datum object_index, real_point3d* point_out)
{
	object_get_origin(object_index, point_out, true);
	return;
}

real_matrix4x3* object_get_node_matrix(datum object_index, int16 node_index)
{
	int32 node_count;
	real_matrix4x3* nodes = (real_matrix4x3*)object_get_node_matrices(object_index, &node_count);

	ASSERT(VALID_INDEX(node_index, node_count));
	return &nodes[node_index];
}

real_matrix4x3* object_try_get_node_matrix_interpolated(datum object_index, int16 node_index, real_matrix4x3* out_mat)
{
	if (!halo_interpolator_interpolate_object_node_matrix(object_index, node_index, out_mat))
	{
		*out_mat = *object_get_node_matrix(object_index, node_index);
	}
	return out_mat;
}

real_matrix4x3* object_get_node_matrices(datum object_index, int32* out_node_count)
{
	object_datum* object = object_get(object_index);
	
	ASSERT(out_node_count);
	ASSERT(object);

	return (real_matrix4x3*)object_header_block_get_with_count(object_index, &object->object.nodes_block, sizeof(real_matrix4x3), out_node_count);
}

int32 __cdecl object_get_skinning_matrices(datum object_index, int32 skinning_matrix_count, const real_matrix4x3* object_skinning_matrices, real_matrix4x3* out_object_skinning_matrices)
{
	return INVOKE(0x1963C7, 0x0, object_get_skinning_matrices, object_index, skinning_matrix_count, object_skinning_matrices, out_object_skinning_matrices);
}

datum object_get_damage_owner(datum object_index)
{
	object_datum* object = object_get(object_index);
	return object->object.damage_owner_object_index;
}

int32 object_get_entity_index(datum object_idx)
{
	return object_get(object_idx)->object.simulation_entity_index;
}

void object_get_region_information(
	int32 object_index,
	int32* region_count,
	int8** region_permutation_indices,
	int8** region_render_permutation_indices,
	int8** region_information)
{
	const object_datum* object = object_get(object_index);

	ASSERT(region_count);

	int8* regions_indices = (int8*)object_header_block_get_with_count(
		object_index,
		&object->object.collision_regions_block,
		1,
		region_count
	);

	*region_count /= 10;
	if (region_permutation_indices)
	{
		*region_permutation_indices = regions_indices;
	}
	if (region_render_permutation_indices)
	{
		*region_render_permutation_indices = regions_indices + *region_count;
	}
	if (region_information)
	{
		*region_information = regions_indices + 2 * *region_count;
	}
	return;
}

void __cdecl object_apply_function_overlay_node_orientations(
	datum object_index, 
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
	if (!halo_interpolator_interpolate_center_of_mass(object_index, center_of_mass))
	{
		center_of_mass = object_get_center_of_mass(object_index, center_of_mass);
	}
	return center_of_mass;
}

datum __cdecl object_get_parent_recursive(datum parent_index)
{
	return INVOKE(0x132574, 0x121444, object_get_parent_recursive, parent_index);
}

void __cdecl object_pre_delete_recursive(datum object_index)
{
	INVOKE(0x1386E1, 0x1275B1, object_pre_delete_recursive, object_index);
	return;
}

void __cdecl object_delete_recursive(datum object_index, bool remove_from_map_before_deletion)
{
	INVOKE(0x13683D, 0x12570D, object_delete_recursive, object_index, remove_from_map_before_deletion);
	return;
}

void __cdecl objects_garbage_collection(void)
{
	INVOKE(0x1316A4, 0x120574, objects_garbage_collection);
	return;
}

void objects_purge_deleted_objects(void)
{
	data_iterator object_header_it;
	iterator_new(&object_header_it, object_header_data_get());

	const object_header_datum* object_header = (object_header_datum*)iterator_next(&object_header_it);

	while (object_header)
	{
		if (object_header->flags.test(_object_header_being_deleted_bit))
		{
			object_pre_delete_recursive(object_header_it.index);
			object_delete_recursive(object_header_it.index, true);
		}

		object_header = (object_header_datum*)iterator_next(&object_header_it);
	}
	return;
}

void __cdecl object_get_velocities(datum object_index, real_vector3d* translational_velocity, real_vector3d* angular_velocity)
{
	INVOKE(0x132BDE, 0, object_get_velocities, object_index, translational_velocity, angular_velocity);
	return;
}

uint32 __cdecl object_search_for_objects_in_radius(
	int32 search_flags,
	uint32 object_type_flags,
	s_location *location,
	real_point3d* center,
	real32 radius,
	datum* out_object_indices,
	uint32 object_count)
{
	return INVOKE(0x1331BA, 0x12208A, object_search_for_objects_in_radius, search_flags, object_type_flags, location, center, radius, out_object_indices, object_count);
}

void __cdecl objects_post_update(void)
{
	object_globals_get()->objects_updating = true;

	data_iterator iterator;
	iterator_new(&iterator, object_header_data_get());

	object_header_datum* object_header = (object_header_datum*)iterator_next(&iterator);

	while (object_header)
	{
		const datum object_index = iterator.index;
		object_datum* object = object_get(object_index);

		object_header->flags.set(_object_header_do_not_update_bit, false);

		if (object_header->flags.test(_object_header_post_update_bit) 
			&& object_header->flags.test(_object_header_active_bit) 
			&& object_header->flags.test(_object_header_awake_bit) 
			&& !object_header->flags.test(_object_header_being_deleted_bit)
			)
		{
			ASSERT(object_get(object_index)->object.parent_object_index == NONE);
			ASSERT(object_get(object_index)->object.next_object_index == NONE);


			object_header->flags.set(_object_header_post_update_bit, false);
			
			object_update(iterator.index);

			if (object_header->flags.test(_object_header_requires_motion_bit))
			{
				ASSERT(object_header->flags.test(_object_header_active_bit));
				ASSERT(object_header->flags.test(_object_header_awake_bit));

				object_move(iterator.index);
			}
		}

		if (object_header->flags.test(_object_header_active_bit)
			&& object_header->flags.test(_object_header_awake_bit)
			&& !object_header->flags.test(_object_header_being_deleted_bit))
		{
			if (object->object.flags.test(_object_hidden_bit) && !shell_is_dedicated_server())
			{
				// reset the interpolator for this object, if hidden
				// ### FIXME maybe hook object_hide and reset it there?
				object_initialize_for_interpolation(iterator.index);
			}
		}

		object_header = (object_header_datum*)iterator_next(&iterator);
	}

	weapons_fire_barrels();
	objects_purge_deleted_objects();
	object_globals_get()->objects_updating = false;
	objects_garbage_collection();
	return;
}

int16 __cdecl object_get_markers_by_string_id(datum object_index, string_id marker, object_marker* marker_object, int16 count)
{
	return internal_object_get_markers_by_string_id(object_index, marker, marker_object, count, false);
}

bool __cdecl object_force_inside_bsp(datum object_index, const real_point3d* known_good_point, int32 ignore_object_index)
{
	return INVOKE(0x137BCC, 0x0, object_force_inside_bsp, object_index, known_good_point, ignore_object_index);
}

void* object_get_and_verify_type(datum object_index, int32 object_type_mask)
{
	const object_header_datum* object_header = object_header_get(object_index);
	const object_datum* object = object_header->datum;
	
	const e_object_type type = object->object.object_identifier.get_type();
	vassert(TEST_BIT(object_type_mask, type), "got an object type we didn't expect (expected one of 0x%08x but got #%d).", object_type_mask, type);
	return (void*)object;
}

datum object_get_ultimate_parent(datum object_index)
{
	datum result = NONE;
	while (object_index != NONE)
	{
		result = object_index;
		object_index = object_get(object_index)->object.parent_object_index;
	}
	return result;
}

void object_attach_to_node(datum parent_index, datum child_index, uint16 node_index)
{
	INVOKE(0x13768C, 0x12655C, object_attach_to_node, parent_index, child_index, node_index);
}

#ifdef OBJECT_DEBUG
void objects_information_get(objects_information* information)
{
	csmemset(information, 0, sizeof(objects_information));
	
	data_array* header_data = object_header_data_get();
	object_header_datum* header = (object_header_datum*)header_data->data;

	for (int16 index = 0; index < (int16)header_data->first_free_absolute_index; ++index)
	{
		if (header->identifier)
		{
			++information->object_count;
			if (header->flags.test(_object_header_active_bit))
			{
				++information->active_object_count;
			}
		}
		++header;
	}
	
	const s_memory_pool* object_memory_pool = object_memory_pool_get();
	information->free_objects = k_maximum_objects_per_map - information->object_count;
	information->active_garbage_object_count = (int16)object_globals_get()->active_garbage_object_count;
	information->used_memory = 1.f - memory_pool_get_free_size(object_memory_pool) / 1048576.f;
	information->contiguous_used_memory = memory_pool_get_contiguous_free_size(object_memory_pool) / 1048576.f;
	return;
}
#endif

void __cdecl object_set_hidden(datum object_index, bool hidden)
{
	//INVOKE(0x1362C9, 0x125199, object_set_hidden, object_index, hidden);
	
	item_datum* object = (item_datum*)object_get(object_index);
	if (TEST_BIT(_object_mask_item, object_get_type(object_index)) && object->item.flags.test(_item_in_unit_inventory_bit))
	{
		const bool hidden_in_inventory = object->item.flags.test(_item_hidden_in_unit_inventory_bit);
		if (unit_does_not_show_readied_weapon(object->item.inventory_owner_unit_index))
		{
			ASSERT(hidden || !hidden_in_inventory);
		}
		else
		{
			ASSERT(hidden == hidden_in_inventory);
		}
	}

	if (!hidden || object->object.flags.test(_object_hidden_bit))
	{
		if (!hidden && object->object.flags.test(_object_hidden_bit))
		{
			object->object.flags.set(_object_hidden_bit, false);
			if (object_is_connected_to_map(object_index))
			{
				object_disconnect_from_map(object_index, false, true);
			}
			object_update_collision_culling(object_index);
		}
	}
	else
	{
		if (object_is_connected_to_map(object_index))
		{
			object_disconnect_from_map(object_index, true, false);
		}
		object->object.flags.set(_object_hidden_bit, true);
		object_update_collision_culling(object_index);
	}
	return;
}

#ifdef OBJECT_OVERRIDE_ENABLED
void object_override_set_shader(datum object_index, datum shader_tag_index)
{
	const int32 override_index = object_get_override_index(object_index);
	if (override_index != NONE)
	{
		object_override_data_get(override_index)->shader_tag_index = shader_tag_index;
	}
	return;
}

datum object_override_get_shader(datum object_index)
{
	const int32 override_index = object_find_override_index(object_index);

	datum override_shader = NONE;
	if (override_index != NONE)
	{
		override_shader = object_override_data_get(override_index)->shader_tag_index;
	}
	return override_shader;
}
#endif

#ifdef OBJECT_DEBUG
void objects_dump_memory(void)
{
	dump_datum dumps[k_maximum_objects_per_map];
	dump_datum dumps_by_type[NUMBER_OF_OBJECT_TYPES];

	int16 object_count = 0;
	int16 overflowed_object_count = 0;

	csmemset(dumps, 0, sizeof(dumps));
	csmemset(dumps_by_type, 0, sizeof(dumps_by_type));

	for (e_object_type type = _object_type_biped; type < NUMBEROF(dumps_by_type); ++type)
	{
		dumps_by_type[type].object_type = type;
		dumps_by_type[type].definition_index = NONE;
	}

	c_object_iterator<object_datum> object_iterator;
	object_iterator.begin(_object_mask_all, 0);

	while (object_iterator.next())
	{
		const object_datum* object = object_iterator.get_datum();
		int32 index = NONE;
		for (int16 object_num = 0; object_num < object_count; ++object_num)
		{
			if (dumps[object_num].definition_index == object->definition_index)
			{
				index = object_num;
				break;
			}
		}

		if (index == NONE)
		{
			if (object_count<k_maximum_objects_per_map)
			{
				index = object_count++;
				dumps[index].object_type = _object_type_none;
				dumps[index].definition_index = object->definition_index;
			}
			else
			{
				++overflowed_object_count;
			}
		}

		const object_header_datum* header = object_header_get(object_iterator.get_index());
		if (index != NONE)
		{
			object_add_to_dump(object_iterator.get_index(), &dumps[index]);
		}

		ASSERT((header->type >= 0) && (header->type < NUMBER_OF_OBJECT_TYPES));
		object_add_to_dump(object_iterator.get_index(), &dumps_by_type[(uint8)header->type]);
	}

	qsort(dumps, object_count, sizeof(dump_datum), (_CoreCrtNonSecureSearchSortCompareFunction)sort_dumps);
	qsort(dumps_by_type, NUMBEROF(dumps_by_type), sizeof(dump_datum), (_CoreCrtNonSecureSearchSortCompareFunction)sort_dumps);

	const char* tag_path = tag_get_name(global_scenario_index_get());
	const char* tag_name = tag_name_strip_path(tag_path);

	char filepath[256];
	csprintf(filepath, NUMBEROF(filepath), "%s_object_memory%d.txt", tag_name, g_object_memory_dump_number++);
	_iobuf* file;
	const errno_t error = fopen_s(&file, filepath, "a+b");
	if (error == 0)
	{
		objects_information information;
		objects_information_get(&information);
		fprintf(file,
			"#%d objects (#%d active) using %3.2f%% of available memory\n\n",
			information.object_count,
			information.active_object_count,
			information.used_memory * 100.f);
		fprintf(file, "OBJECTS BY TYPE\n");
		fprintf(file, "number (active) [garbage/   dead/outside/at-rest] maxsize totsize\n");
		
		for (int16 type_num = 0; type_num < NUMBEROF(dumps_by_type); ++type_num)
		{
			object_dump_print_info(file, &dumps_by_type[type_num]);
		}

		fprintf(file, "\n");
		fprintf(file, "OBJECTS BY DEFINITION\n");
		fprintf(file, "number (active) [garbage/   dead/outside/at-rest] maxsize totsize\n");
		for (int16 object_num = 0; object_num < object_count; ++object_num)
		{
			object_dump_print_info(file, &dumps[object_num]);
		}

		fprintf(file, "\n");
		if (overflowed_object_count > 0)
		{
			fprintf(
				file,
				"WARNING: overflowed k_maximum_objects_per_map (%d), this dump does not include %d objects that would not fit!\n",
				k_maximum_objects_per_map,
				overflowed_object_count);
		}
			
		fprintf(file, "\n");
		fclose(file);
	}
	return;
}
#endif

/* private code */

static s_memory_pool* object_memory_pool_get(void)
{
	return *Memory::GetAddress<s_memory_pool**>(0x4E4610, 0x50C8E0);
};

static datum** game_state_object_name_list_get(void)
{
	return Memory::GetAddress<datum**>(0x4E2614, 0x50A8E4);
}

static void objects_apply_interpolation_patches(void)
{
	object_new_replace_calls();

	if (!shell_is_dedicated_server())
	{
		object_move_replace_calls();
		object_get_markers_by_string_id_replace_calls();

		PatchCall(Memory::GetAddress(0xCD744, 0xB8ABD), object_get_origin_interpolated);
		PatchCall(Memory::GetAddress(0x1549AE), object_get_origin_interpolated);
		PatchCall(Memory::GetAddress(0x13D406, 0x12C255), object_get_center_of_mass_interpolated);

		// Prevents the game from passing the runtime_node_flags to the animation manager when updating object_node_matricies
		// When they are passed to the animation manager it causes the game to reset? node positions causing a flipping state between frames.
		WriteValue<uint8>(Memory::GetAddress(0x135657, 0x124527), JMP_OP_CODE);
	}
	return;
}

static void object_move_replace_calls(void)
{
	PatchCall(Memory::GetAddress(0x137f8f), object_move);
	PatchCall(Memory::GetAddress(0x1381b1), object_move);
	PatchCall(Memory::GetAddress(0x13814b), object_move);
	return;
}

static void object_new_replace_calls(void)
{
	DETOUR_ATTACH(p_object_new, Memory::GetAddress<t_object_new>(0x136CA7, 0x125B77), object_new);
	return;
}

static void object_get_markers_by_string_id_replace_calls(void)
{
	// PatchCall(Memory::GetAddress(0x132792, 0x121662), internal_object_get_markers_by_string_id);	(Disabled as this is not interpolated)	TODO: Add parameter to internal_object_get_markers_by_string_id instead of not patchcalling
	// PatchCall(Memory::GetAddress(0x1327B1, 0x121681), internal_object_get_markers_by_string_id);	(Disabled as this is not interpolated)	TODO: Add parameter to internal_object_get_markers_by_string_id instead of not patchcalling
	// PatchCall(Memory::GetAddress(0x13823D, 0x12710D), internal_object_get_markers_by_string_id);	(Disabled as this is not interpolated)	TODO: Add parameter to internal_object_get_markers_by_string_id instead of not patchcalling
	// PatchCall(Memory::GetAddress(0x138257, 0x1214CB), internal_object_get_markers_by_string_id);	(Disabled as this is not interpolated)	TODO: Add parameter to internal_object_get_markers_by_string_id instead of not patchcalling
	// PatchCall(Memory::GetAddress(0x134C26, 0x123AF6), internal_object_get_markers_by_string_id);	// object_animation_callback

	PatchCall(Memory::GetAddress(0xAAFA4), object_get_markers_by_string_id);			// Function for creating effects at markers
	PatchCall(Memory::GetAddress(0xFDF7D), object_get_markers_by_string_id);			// effect_on_new_object_marker	
	return;
}

static void __cdecl object_move(datum object_index)
{
	INVOKE(0x137E6D, 0x126D3D, object_move, object_index);
	object_initialize_for_interpolation(object_index);
	return;
}

static datum object_allocate_header(datum tag_definition_index)
{
	datum object_index = NONE;

	if (tag_definition_index != NONE)
	{
		struct object_definition const* object_definition = object_definition_get(tag_definition_index);
		object_type_definition const* object_type_definition = object_type_definition_get((e_object_type)object_definition->object.object_type);
		
		object_index = object_header_new(object_type_definition->datum_size);
	}

	return object_index;
}

static void object_header_delete(datum object_index)
{
	object_header_datum* object_header = object_header_get(object_index);
	object_header->flags = 0;
	if (object_header->datum != NULL)
	{
		ASSERT(!object_header->datum->object.physics_flags.test(_object_allocated_havok_component_bit));
		memory_pool_block_free(object_memory_pool_get(), (void**)&object_header->datum);
		object_header->datum = NULL;
	}

	datum_delete(object_header_data_get(), object_index);
	return;
}

static void object_postprocess_node_matrices(datum object_index)
{
	object_datum const* object = object_get(object_index);
	struct object_definition const* object_definition = object_definition_get(object->definition_index);
	
	ASSERT(object_definition);

	const datum model_index = object_definition->object.model.index;
	if (model_index != NONE)
	{
		s_model_definition const* model_tag = model_definition_get(model_index);
		if (model_tag->render_model.index != NONE && model_tag->animation_graph.index != NONE)
		{
			int32 node_count;
			real_matrix4x3* node_matricies = object_get_node_matrices(object_index, &node_count);
			object_type_postprocess_node_matrices(object_index, node_count, node_matricies);
		}
	}
	return;
}

static void object_initialize_effects(datum object_index)
{
	const object_header_datum* object_header = object_header_get(object_index);
	object_datum* object = object_header->datum;

	if (object_header->type == _object_type_projectile)
	{
		object->object.object_projectile_datum = NONE;
	}
	else
	{
		widgets_new(object_index);
	}
	attachments_new(object_index);
	return;
}

static void object_occlusion_data_initialize(datum object_index)
{
	INVOKE(0x26CFDD, 0x0, object_occlusion_data_initialize, object_index);
	return;
}

static void __cdecl object_cleanup_havok(datum object_index)
{
	INVOKE(0xA2778, 0x949D8, object_cleanup_havok, object_index);
	return;
}

static bool object_find_initial_location(datum object_index, s_location* location)
{
	object_datum* object = object_get(object_index);

	ASSERT(object->object.parent_object_index == NONE);

	scenario_location_from_point(location, &object->object.object_origin_point);

	if (location->cluster_index == NONE)
	{
		collision_bsp_test_sphere_result test_result;
		collision_bsp_test_sphere(global_collision_bsp_get(), 0, NULL, &object->object.object_origin_point, object->object.shadow_sphere_radius, &test_result);

		if (test_result.stack_depth)
		{
			scenario_location_from_leaf(location, test_result.buffer3.leaf_index);
		}
		else
		{
			scenario_location_from_point(location, &object->object.position);
		}
	}

	return location->cluster_index != NONE;
}

static bool __cdecl object_compute_change_colors(datum object_index)
{
	return INVOKE(0x13456E, 0x12343E, object_compute_change_colors, object_index);
}

static void __cdecl update_object_region_information(datum object_datum, int32 region_index)
{
	INVOKE(0x132F07, 0x121DD7, update_object_region_information, object_datum, region_index);
	return;
}

static void object_variant_index_update(datum object_index, string_id variant)
{
	object_datum* object = object_get(object_index);
	object->object.model_variant_id = object_lookup_variant_index_from_name(object_index, variant);
	return;
}

static int8 __cdecl object_lookup_variant_index_from_name(datum object_index, string_id variant)
{
	return INVOKE(0x12FE84, 0x11ED47, object_lookup_variant_index_from_name, object_index, variant);
}

static bool object_can_activate_in_cluster(datum object_index, s_game_cluster_bit_vectors* cluster_activation)
{
	bool result = false;
	object_header_datum* object_header = object_header_get(object_index);
	const object_datum* object = object_header->datum;

	if (object->object.flags.test(_object_always_active_bit))
	{
		return true;
	}
	if (TEST_FLAG(FLAG(object_header->type), _object_mask_machine) && object_type_compute_activation(object_index, cluster_activation, &result) || object_header->cluster_index == NONE)
	{
		return result;
	}
	return BIT_VECTOR_TEST_FLAG(cluster_activation->cluster_bitvector, object_header->cluster_index);
}

static void object_reconnect_to_map(datum object_index, s_location* location)
{
	object_header_datum* object_header = object_header_get(object_index);
	object_datum* object = object_header->datum;
	bool cluster_index_is_null = object_header->cluster_index == NONE;

	ASSERT(DATUM_INDEX_TO_IDENTIFIER(object_index));
	ASSERT(object->object.parent_object_index == NONE);
	ASSERT(!object_header->flags.test(_object_header_connected_to_map_bit));
	ASSERT(!object_header->flags.test(_object_header_child_bit));
	ASSERT(!object->object.flags.test(_object_connected_to_map_bit));
	ASSERT(!object->object.flags.test(_object_in_limbo_bit));
	ASSERT(objects_can_connect_to_map());

	s_location* p_location = location;
	s_location scnr_location;

	if (!p_location)
	{
		scenario_location_from_point(&scnr_location, &object->object.object_origin_point);
		p_location = &scnr_location;
		if (scnr_location.cluster_index == NONE)
		{
			scenario_location_from_point(&scnr_location, &object->object.position);
		}
	}

	if (p_location->cluster_index == NONE)
	{
		object->object.flags.set(_object_outside_of_map_bit, true);
	}
	else
	{
		object->object.location.leaf_index = p_location->leaf_index;
		object->object.location.cluster_index = p_location->cluster_index;
		object->object.location.bsp_index = p_location->bsp_index;
		object_header->cluster_index = p_location->cluster_index;
		object->object.flags.set(_object_outside_of_map_bit, false);
	}
	s_game_cluster_bit_vectors cluster_bitvector;
	s_game_cluster_bit_vectors* p_cluster_bitvector = NULL;
	bool cluster_overflow = false;
	if (object->object.flags.test(_object_cinematic_visibility_bit))
	{
		csmemset(cluster_bitvector.cluster_bitvector, NONE, BIT_VECTOR_SIZE_IN_BYTES(global_structure_bsp_get()->clusters.count));
		p_cluster_bitvector = &cluster_bitvector;
	}

	s_object_payload payload;
	object_get_payload(object_index, &payload);

	cluster_partition* partition = collideable_object_cluster_partition_get();
	if (!object->object.flags.test(_object_uses_collidable_list_bit))
	{
		partition = noncollideable_object_cluster_partition_get();
	}

	cluster_partition_reconnect(
		partition,
		object_index,
		&object->object.first_cluster_reference,
		&object->object.object_origin_point,
		object->object.shadow_sphere_radius,
		&object->object.location,
		p_cluster_bitvector,
		sizeof(payload),
		&payload,
		&cluster_overflow);

	object->object.flags.set(_object_connected_to_map_bit, true);
	object_header->flags.set(_object_header_connected_to_map_bit, true);

	if (cluster_overflow)
	{
		error(
			_error_category_objects,
			_error_delayed,
			"### WARNING object %s touched too many clusters",
			tag_get_name(object->definition_index)
		);
	}

	object_connect_lights_recursive(object_index, false, true, false, false);

	if (object_can_activate_in_cluster(object_index, game_get_cluster_activation()))
	{
		object_activate(object_index);
	}
	else if (!object->object.flags.test(_object_deleted_when_deactivated_bit) || simulation_query_object_is_predicted(object_index))
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

static void object_get_payload(datum object_index, s_object_payload* payload)
{
	const object_datum* object = object_get(object_index);
	uint16 object_collision_cull_flags = 0;
	if (object->object.flags.test(_object_uses_collidable_list_bit))
	{
		object_collision_cull_flags = collision_compute_object_cull_flags(object_index);
	}

	ASSERT(payload);

	payload->object_type = (int16)object->object.object_identifier.get_type();
	payload->object_collision_cull_flags = object_collision_cull_flags;
	payload->origin_point = object->object.object_origin_point;
	payload->bounding_sphere_radius = object->object.shadow_sphere_radius;
	return;
}

static void __cdecl object_connect_lights_recursive(datum object_datum, bool disconnect_this_object, bool reconnect_this_object, bool reconnect_child_objects, bool show_this_object)
{
	INVOKE(0x134A78, 0x123948, object_connect_lights_recursive, object_datum, disconnect_this_object, reconnect_this_object, reconnect_child_objects, show_this_object);
	return;
}

static void __cdecl attachments_new(datum object_index)
{
	INVOKE(0x1348CA, 0x12379A, attachments_new, object_index);
	return;
}

static bool __cdecl object_update(datum object_index)
{
	return INVOKE(0x1352a9, 0x124179, object_update, object_index);
}

static void object_initialize_for_interpolation(datum object_index)
{
	object_datum* object = object_get(object_index);
	struct object_definition const* object_definition = object_definition_get(object->definition_index);

	if (object_definition->object.model.index == NONE)
	{
		if ((int16)object_definition->object.attachments.count <= 0)
		{
			return;
		}

		int16 tag_block_index = 0;
		while (1)
		{
			object_attachment_definition const* attachment = TAG_BLOCK_GET_ELEMENT(
				&object_definition->object.attachments,
				tag_block_index,
				object_attachment_definition
			);

			if (attachment->type.index != NONE)
			{
				tag_group type = attachment->type.group;
				if (type == _tag_group_lens_flare
					|| type == _tag_group_light
					|| type == _tag_group_light_volume
					|| type == _tag_group_liquid
					|| type == _tag_group_contrail
					|| type == _tag_group_effect)
				{
					break;
				}
			}
			if (++tag_block_index >= (int16)object_definition->object.attachments.count)
			{
				return;
			}
		}
	}

	int32 nodes_count;
	real_point3d center_of_mass;
	real_matrix4x3* matrices = (real_matrix4x3*)object_header_block_get_with_count(object_index, &object->object.nodes_block, sizeof(real_matrix4x3), &nodes_count);
	object_get_center_of_mass(object_index, &center_of_mass);
	halo_interpolator_object_populate_interpolation_data(object_index, matrices, nodes_count, &object->object.position, &object->object.forward, &object->object.up, &center_of_mass);
	return;
}

static void object_reset_interpolation(datum object_index)
{
	const object_datum* object = object_get(object_index);

	ASSERT(object);

	c_animation_manager* animation_manager = (c_animation_manager*)object_header_block_get(object_index, &object->object.animation_manager_block);
	animation_manager->interpolator_controls[1].disable();
	object_wake(object_index);
	return;
}

static int16 __cdecl internal_object_get_markers_by_string_id(
	datum object_index,
	string_id name,
	object_marker* markers,
	int16 maximum_marker_count, 
	bool local_markers_only)
{
	int16 marker_index = 0;

	datum parent_object_index = object_index;
	if (!local_markers_only)
	{
		parent_object_index = object_get_parent_recursive(parent_object_index);
	}

	if (parent_object_index != NONE)
	{
		object_datum* parent_object = object_get(parent_object_index);
		struct object_definition const* object_definition = object_definition_get(parent_object->definition_index);

		ASSERT(object_definition);

		const datum object_model_index = object_definition->object.model.index;
		if (object_model_index != NONE)
		{
			s_model_definition const* model_definition = model_definition_get(object_model_index);

			int32 node_count = 0;
			const real_matrix4x3* node_matrices;

			int32 region_count = 0;
			int8* region_permutation_indices;
			object_get_region_information(parent_object_index, &region_count, &region_permutation_indices, NULL, NULL);
			if (!halo_interpolator_interpolate_object_node_matrices(parent_object_index, &node_matrices, &node_count))
			{
				node_matrices = object_get_node_matrices(parent_object_index, &node_count);
			}

			ASSERT(region_count == model_definition->runtime_regions.count);
			ASSERT(node_count == model_definition->runtime_nodes.count);

			marker_index = (int16)render_model_get_markers_by_name(
				model_definition->render_model.index,
				name,
				region_permutation_indices,
				NONE,
				NULL,
				node_count,
				node_matrices,
				parent_object->object.flags.test(_object_mirrored_bit),
				markers,
				maximum_marker_count
			);
		}
	}

	// Fallback if we weren't able to get a marker using the parent index
	if (!marker_index)
	{
		object_datum* object = object_get(object_index);
		ASSERT(maximum_marker_count > 0);

		markers->node_index = 0;
		matrix4x3_identity(&markers->node_matrix);

		if (!halo_interpolator_interpolate_object_node_matrix(object_index, 0, &markers->matrix))
		{
			markers->matrix = *object_get_node_matrix(object_index, 0);
		}

		markers->radius = 0.f;
		if (object->object.flags.test(_object_mirrored_bit))
		{
			negate_vector3d(&markers->matrix.vectors.left, &markers->matrix.vectors.left);
		}

		if (name == _string_id_empty_string)
		{
			marker_index = 1;
		}
	}

	return marker_index;
}

static void object_disconnect_from_map(datum object_index, bool disconnect_this_object, bool reconnect_this_object)
{
	object_connect_lights_recursive(object_index, disconnect_this_object, reconnect_this_object, false, false);
	return;
}

#ifdef OBJECT_OVERRIDE_ENABLED
static s_object_override_data* object_override_data_get(size_t override_index)
{
	ASSERT(override_index >= 0 && override_index < k_maximum_object_override_count);
	return &g_object_override_data[override_index];
}

static int32 object_find_override_index(datum object_index)
{
	const object_datum* object = object_get(object_index);
	int32 override_index = NONE;
	
	if (object->object.flags.test(_object_has_override_bit))
	{
		// Find the object override with the same object index
		for (int32 i = 0; i < k_maximum_object_override_count; ++i)
		{
			const s_object_override_data* override = object_override_data_get(i);
			ASSERT(override_index >= 0 && override_index < k_maximum_object_override_count);
			if (override[i].valid && override[i].object_index == object_index)
			{
				override_index = i;
				break;
			}
		}
	}

	ASSERT(object->object.flags.test(_object_has_override_bit) == (override_index != NONE));
	return override_index;
}

static int32 object_get_override_index(datum object_index)
{
	object_datum* object = object_get(object_index);
	int32 override_index = object_find_override_index(object_index);
	if (override_index == NONE)
	{
		ASSERT(!object->object.flags.test(_object_has_override_bit));

		// Initialize the first object override that isn't valid
		for (int32 i = 0; i < k_maximum_object_override_count; ++i)
		{
			s_object_override_data* override = object_override_data_get(i);
			if (!override->valid)
			{
				override_index = i;
				override->valid = true;
				override->object_index = object_index;
				override->shader_tag_index = NONE;
				break;
			}
		}

		if (override_index == NONE)
		{
			s_object_override_data* override = object_override_data_get(0);
			ASSERT(override->valid);
			ASSERT(override->object_index != NONE);

			// This specific call to object_get_and_verify_type was inlined by default for some reason?
			object_datum* override_object = object_get(g_object_override_data[0].object_index);
			if (override_object)
			{
				override_object->object.flags.set(_object_has_override_bit, false);
			}

			memmove_guarded(
				g_object_override_data,
				&g_object_override_data[1],
				sizeof(s_object_override_data) * k_maximum_object_override_count - 1,
				g_object_override_data,
				sizeof(s_object_override_data) * k_maximum_object_override_count);

			override = object_override_data_get(k_maximum_object_override_count - 1);
			override->valid = true;
			override->object_index = object_index;
			override->shader_tag_index = NONE;
			override_index = k_maximum_object_override_count - 1;
		}
		object->object.flags.set(_object_has_override_bit, true);
	}

	ASSERT(object->object.flags.test(_object_has_override_bit) == (override_index != NONE));
	return override_index;
}
#endif

#ifdef OBJECT_DEBUG
static void object_add_to_dump(datum object_index, dump_datum* dump)
{
	const object_header_datum* header = object_header_get(object_index);
	const object_datum* object = object_get(object_index);
	
	if (header->data_size > dump->maximum_size)
	{
		dump->maximum_size = header->data_size;
	}

	dump->total_size += header->data_size;
	++dump->count;

	if (header->flags.test(_object_header_active_bit))
	{
		++dump->active_count;
	}

	if (object->object.flags.test(_object_garbage_bit))
	{
		++dump->garbage_count;
	}

	if (object->object.object_damage_flags.test(_object_is_dead_bit))
	{
		++dump->dead_count;
	}

	if (object->object.physics_flags.test(_object_physics_bit_8))
	{
		++dump->at_rest_count;
	}

	const object_datum* parent = object_get(object_get_ultimate_parent(object_index));
	if (parent->object.flags.test(_object_outside_of_map_bit) || parent->object.location.cluster_index == NONE)
	{
		++dump->outside_map_count;
	}
	return;
}

static void object_dump_print_info(_iobuf* handle, const dump_datum* dump)
{
	const char* name = "unknown";
	if (dump->definition_index == NONE)
	{
		if (dump->object_type != NONE)
		{
			name = object_type_get_name(dump->object_type);
		}
	}
	else
	{
		name = tag_get_name(dump->definition_index);
	}

	fprintf(
		handle,
		"% 6d (% 6d) [% 7d/% 7d/% 7d/% 7d] % 7d % 7d %s\r\n",
		dump->count,
		dump->active_count,
		dump->garbage_count,
		dump->dead_count,
		dump->outside_map_count,
		dump->at_rest_count,
		dump->maximum_size,
		dump->total_size,
		name);
	return;
}


static int sort_dumps(const dump_datum* dump1, const dump_datum* dump2)
{
	int result = NONE;
	if (dump1->total_size < dump2->total_size)
	{
		result = 1;
	}
	else if (dump1->total_size <= dump2->total_size)
	{
		result = 0;
	}

	return result;
}
#endif
