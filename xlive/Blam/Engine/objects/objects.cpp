#include "stdafx.h"
#include "objects.h"

#include "object_early_movers.h"
#include "object_globals.h"

#include "Blam/Cache/TagGroups/device_definition.hpp"
#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "Blam/Cache/TagGroups/model_definition.hpp"
#include "Blam/Cache/TagGroups/render_model_definition.hpp"
#include "Blam/Engine/animations/animation_manager.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/memory/bitstream.h"
#include "Blam/Engine/Players/Players.h"
#include "Blam/Engine/physics/bsp3d.h"
#include "Blam/Engine/physics/collision_bsp.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Blam/Engine/Simulation/simulation.h"
#include "Blam/Engine/structures/cluster_partitions.h"
#include "Blam/Engine/structures/structures.h"
#include "Blam/Engine/Simulation/GameInterface/SimulationGameUnits.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

void object_placement_data_new(object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, void* damage_owner)
{
	LOG_TRACE_GAME("{}: {:X}, object_owner: {:X}, unk: {:X})", __FUNCTION__, object_definition_idx, object_owner_idx, damage_owner);

	typedef void(__cdecl* object_placement_data_new_t)(void*, datum, datum, void*);
	auto p_object_placement_data_new = Memory::GetAddress<object_placement_data_new_t>(0x132163, 0x121033);

	p_object_placement_data_new(object_placement_data, object_definition_idx, object_owner_idx, damage_owner);
}

//Pass datum from new object into object to sync
void simulation_action_object_create(datum object_idx)
{
	typedef void(__cdecl* simulation_action_object_create_t)(datum);
	auto p_simulation_action_object_create = Memory::GetAddress<simulation_action_object_create_t>(0x1B8D14, 0x1B2C44);

	return p_simulation_action_object_create(object_idx);
}

void object_delete(const datum object_idx)
{
	typedef void(__cdecl* object_delete_t)(datum);
	auto p_object_delete = Memory::GetAddress<object_delete_t>(0x136005, 0x124ED5);

	p_object_delete(object_idx);
}

// Wakes object by adding the _object_header_awake_bit to every object and all parents of that object
void object_wake(const datum object_datum)
{
	s_object_header* object_header = get_object_header(object_datum);
	if ((object_header->flags & _object_header_awake_bit) == 0 && object_datum != DATUM_INDEX_NONE)
	{
		datum current_object_datum = object_datum;

		do
		{
			object_header->flags |= _object_header_awake_bit;
			current_object_datum = object_get_fast_unsafe(current_object_datum)->parent_datum;
		} 
		while (current_object_datum != DATUM_INDEX_NONE);
	}
}

void object_disconnect_from_map(const datum object_index)
{
	typedef void(__cdecl* object_disconnect_from_map_t)(const datum object_index);
	auto object_disconnect_from_map = Memory::GetAddress<object_disconnect_from_map_t>(0x136266, 0x125136);

	object_disconnect_from_map(object_index);
}

void object_activate(const datum object_datum)
{
	typedef void(__cdecl* havok_object_activate_t)(const datum object_datum);
	auto p_havok_object_activate = Memory::GetAddress<havok_object_activate_t>(0xA040F, 0x9266F);


	s_object_header* object_header = get_object_header(object_datum);
	if ((object_header->flags & _object_header_active_bit) == 0 && object_header->cluster_index != -1)
	{
		s_object_data_definition* object = (s_object_data_definition*)object_header->object;
		if (object->parent_datum == DATUM_INDEX_NONE)
		{
			object_header->flags |= _object_header_active_bit;
			if ((FLAG(object_header->object_type) & (FLAG(creature) | FLAG(crate) | FLAG(machine) | FLAG(vehicle) | FLAG(biped))) != 0)
			{
				p_havok_object_activate(object_datum);
			}
			if ((object->object_flags & _object_garbage_bit) != 0)
			{
				++s_object_globals::get()->active_garbage_object_count;
			}
		}
	}
}

void __cdecl object_deactivate(const datum object_datum)
{
	typedef void(__cdecl* object_deactivate_t)(const datum object_index);
	auto object_deactivate = Memory::GetAddress<object_deactivate_t>(0x1320B8, 0x120F88);
	object_deactivate(object_datum);
}

void __cdecl object_cleanup_havok(const datum object_datum)
{
	typedef void(__cdecl* object_cleanup_havok_t)(const datum object_index);
	auto object_cleanup_havok = Memory::GetAddress<object_cleanup_havok_t>(0xA2778, 0x949D8);
	object_cleanup_havok(object_datum);
}

bool object_is_connected_to_map(const datum object_datum)
{
	datum current_object_datum = object_datum;

	if (object_datum != DATUM_INDEX_NONE)
	{
		do
		{
			current_object_datum = object_get_fast_unsafe(current_object_datum)->parent_datum;
		} 
		while (current_object_datum != DATUM_INDEX_NONE);
	}
	return get_object_header(current_object_datum)->flags &= _object_header_connected_to_map_bit;
}

void object_connect_lights_recursive(const datum object_datum,
	bool disconnect_this_object,
	bool reconnect_this_object,
	bool reconnect_child_objects,
	bool show_this_object)
{
	typedef void(__cdecl* object_connect_lights_recursive_t)(const datum object_datum,
		bool disconnect_this_object,
		bool reconnect_this_object,
		bool reconnect_child_objects,
		bool show_this_object);
	auto p_object_connect_lights_recursive = Memory::GetAddress<object_connect_lights_recursive_t>(0x134A78, 0x123948);
	p_object_connect_lights_recursive(object_datum, disconnect_this_object, reconnect_this_object, reconnect_child_objects, show_this_object);
}

// Gets important info about the object and populates the s_object_payload argument with the appropriate data
void get_object_payload(const datum object_datum, s_object_payload* cluster_payload)
{
	typedef WORD(__cdecl* collision_compute_object_cull_flags_t)(short object_index);
	auto p_collision_compute_object_cull_flags = Memory::GetAddress<collision_compute_object_cull_flags_t>(0xCBE87, 0x8C1A7);

	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	WORD object_collision_cull_flags = 0;
	if ((object->object_flags & _object_has_collision_bit) != 0)
	{
		object_collision_cull_flags = p_collision_compute_object_cull_flags(DATUM_INDEX_TO_ABSOLUTE_INDEX(object_datum));
	}

	cluster_payload->object_type = object->placement_info.object_type;
	cluster_payload->object_collision_cull_flags = object_collision_cull_flags;
	cluster_payload->origin_point = object->object_origin_point;
	cluster_payload->bounding_sphere_radius = object->shadow_sphere_radius;
}

void object_update_collision_culling(const datum object_datum)
{
	typedef void(__cdecl* cluster_partition_update_payload_t)(
		cluster_partition* partition, 
		datum object_datum,
		int cluster_reference,
		unsigned int payload_size,
		s_object_payload* payload);
	auto p_cluster_partition_update_payload = Memory::GetAddress<cluster_partition_update_payload_t>(0x37A24F, 0x324FBF);

	s_object_payload payload;

	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	const s_object_header* object_header = get_object_header(object_datum);
	if ((object_header->flags & _object_header_connected_to_map_bit) != 0
		&& (object->object_flags & _object_has_collision_bit) != 0)
	{
		get_object_payload(object_datum, &payload);
		p_cluster_partition_update_payload(
			Memory::GetAddress<cluster_partition*>(0x4E4604, 0x50C8D4),
			object_datum,
			object->first_cluster_reference,
			20u,
			&payload);
	}
}

bool object_can_activate_in_cluster(const datum object_datum, s_game_cluster_bit_vectors* cluster_activation)
{
	bool result = false;
	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	const s_object_header* object_header = get_object_header(object_datum);

	if ((object->object_flags & object_data_flag_0x2) != 0)
	{
		return true;
	}
	if ((FLAG(object_header->object_type) & FLAG(machine)) != 0 && object_type_compute_activation(object_datum, cluster_activation, &result) || object_header->cluster_index == -1)
	{
		return result;
	}
	return (FLAG(object_header->cluster_index & 0x1F) & cluster_activation[object_header->cluster_index >> 5].cluster_bitvector) != 0;
}

void* object_header_block_get(const datum object_datum, const object_header_block_reference* reference)
{
	return (void*)((char*)object_get_fast_unsafe(object_datum) + reference->offset);
}

void* object_header_block_get_with_count(const datum object_datum, const object_header_block_reference* reference, DWORD element_size, DWORD* element_count)
{
	void* object_header = nullptr;

	*element_count = 0;
	if (reference->offset != 0xFFFF)
	{
		object_header = object_header_block_get(object_datum, reference);
		*element_count = reference->size / element_size;
	}
	return object_header;
}

// Reconnects an object to the current bsp that's loaded
void object_reconnect_to_map(s_location* location, const datum object_datum)
{
	s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	s_object_header* object_header = get_object_header(object_datum);
	bool cluster_index_is_null = object_header->cluster_index == -1;
	s_location scnr_location;
	s_location* p_scnr_location = location;

	if (!location)
	{
		scenario_location_from_point(&scnr_location, &object->object_origin_point);
		p_scnr_location = &scnr_location;
		if (scnr_location.cluster == -1)
		{
			scenario_location_from_point(&scnr_location, &object->position);
		}
	}

	if (p_scnr_location->cluster == -1)
	{
		object->object_flags |= object_data_flag_0x40000;
	}
	else
	{
		object->location.leaf_index = p_scnr_location->leaf_index;
		object->location.cluster = p_scnr_location->cluster;
		object->location.bsp_index = p_scnr_location->bsp_index;
		object_header->cluster_index = p_scnr_location->cluster;
		object->object_flags &= ~object_data_flag_0x40000;
	}
	s_game_cluster_bit_vectors cluster_bitvector[16];
	s_game_cluster_bit_vectors* p_cluster_bitvector = nullptr;
	bool cluster_overflow = false;
	if (!((object->object_flags & object_data_flag_0x200000) == 0))
	{
		memset(cluster_bitvector, -1, 4 * ((get_global_structure_bsp()->clusters.size + 0x1F) >> 5));
		p_cluster_bitvector = cluster_bitvector;
	}

	s_object_payload payload;
	get_object_payload(object_datum, &payload);

	cluster_partition* partition = Memory::GetAddress<cluster_partition*>(0x4E4604, 0x50C8D4);
	if ((object->object_flags & _object_has_collision_bit) == 0)
	{
		partition = Memory::GetAddress<cluster_partition*>(0x4E45F8, 0x50C8C8);
	}

	cluster_partition_reconnect(
		partition,
		object_datum,
		&object->first_cluster_reference,
		&object->object_origin_point,
		object->shadow_sphere_radius,
		&object->location,
		p_cluster_bitvector,
		20,
		&payload,
		&cluster_overflow);

	object->object_flags |= _object_connected_to_map_bit;
	object_header->flags |= _object_header_connected_to_map_bit;
	object_connect_lights_recursive(object_datum, false, true, false, false);
	
	if (s_game_cluster_bit_vectors* cluster_activation = s_game_globals::game_get_cluster_activation();
		object_can_activate_in_cluster(object_datum, cluster_activation))
	{
		object_activate(object_datum);
	}
	else if ((object->object_flags & object_data_flag_0x20000) == 0 || simulation_query_object_is_predicted(object_datum))
	{
		object_deactivate(object_datum);
	}
	else
	{
		object_delete(object_datum);
	}
	if (cluster_index_is_null && object_header->cluster_index != -1)
	{
		object_cleanup_havok(object_datum);
	}
}

void object_compute_node_matrices_with_children(const datum object_datum)
{
	typedef void(__cdecl* object_compute_node_matrices_non_recursive_t)(const datum object_datum);
	auto object_compute_node_matrices_non_recursive = Memory::GetAddress<object_compute_node_matrices_non_recursive_t>(0x1353E6, 0x1242B6);
	typedef bool(__cdecl* object_compute_node_matrices_locations_t)(const datum object_datum);
	auto object_compute_node_matrices_locations = Memory::GetAddress<object_compute_node_matrices_locations_t>(0x1363D5, 0x1252A5);

	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	s_object_data_definition const* next_object = nullptr;
	object_compute_node_matrices_non_recursive(object_datum);
	object_compute_node_matrices_locations(object_datum);
	for (datum i = object->current_weapon_datum; i != -1; i = next_object->next_index)
	{
		next_object = object_get_fast_unsafe(i);
		if ((FLAG(next_object->placement_info.object_type) & FLAG(machine)) == 0)
			object_compute_node_matrices_with_children(i);
	}
}

void update_object_variant_index(datum object_datum, string_id variant_index)
{
	typedef byte(__cdecl* object_lookup_variant_index_from_name_t)(datum a1, string_id a2);
	auto object_lookup_variant_index_from_name = Memory::GetAddress<object_lookup_variant_index_from_name_t>(0x12FE84, 0x11ED47);

	s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	object->model_variant_id = object_lookup_variant_index_from_name(object_datum, variant_index);

	// update the biped variant index (THIS IS A CARTO ADDITION)
	update_biped_object_variant_data(object_datum, variant_index);
}

real_matrix4x3* object_get_node_matrices(datum object_datum, DWORD* out_node_count)
{
	return (real_matrix4x3*)object_header_block_get_with_count(object_datum, &object_get_fast_unsafe(object_datum)->nodes_block, sizeof(real_matrix4x3), out_node_count);
}

void object_postprocess_node_matrices(datum object_index)
{
	auto object = object_get_fast_unsafe(object_index);
	auto object_tag = tags::get_tag_fast<s_object_group_definition>(object->tag_definition_index);
	if (object_tag->model.TagIndex != DATUM_INDEX_NONE)
	{
		auto model_tag = tags::get_tag_fast<s_model_group_definition>(object_tag->model.TagIndex);
		if (model_tag->render_model.TagIndex != DATUM_INDEX_NONE && model_tag->animation.TagIndex != DATUM_INDEX_NONE)
		{
			DWORD node_count = 0;
			real_matrix4x3* node_matricies = object_get_node_matrices(object_index, &node_count);
			object_type_postprocess_node_matrices(object_index, node_count, node_matricies);
		}
	}
}

void object_initialize_effects(datum object_index)
{
	typedef void(__cdecl* widgets_new_t)(datum object_index);
	auto p_widgets_new = Memory::GetAddress< widgets_new_t>(0x14FFE2);

	typedef void(__cdecl* attachments_new_t)(datum object_index);
	auto p_attachments_new = Memory::GetAddress< attachments_new_t>(0x1348CA);

	auto object_header = get_object_header(object_index);
	s_object_data_definition* object = (s_object_data_definition*)object_header->object;
	if (object_header->object_type == projectile)
	{
		object->object_projectile_datum = -1;
	}
	else
	{
		p_widgets_new(object_index);
	}

	p_attachments_new(object_index);
}

bool set_object_position_if_in_cluster(s_location* location, const datum object_datum)
{
	s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	scenario_location_from_point(location, &object->object_origin_point);

	if (location->cluster != -1)
	{
		return true;
	}

	collision_bsp_test_sphere_result test_result;
	auto g_collision_bsp = *Memory::GetAddress<s_scenario_structure_bsp_group_definition::s_collision_bsp_block**>(0x479E64);
	collision_bsp_test_sphere(g_collision_bsp, 0, nullptr, &object->object_origin_point, object->shadow_sphere_radius, &test_result);

	if (test_result.stack_depth)
	{
		scenario_location_from_leaf(location, test_result.buffer3.leaf_index);
	}
	else
	{
		scenario_location_from_point(location, &object->position);
	}

	return location->cluster != -1;
}

// Sets the mode_tag_index and coll_tag_index parameters (if they exist)
// Returns whether or not the render model has PRT or Lighting Info
bool __cdecl object_render_model_has_prt_info(const datum object_datum, datum* mode_tag_index, datum* coll_tag_index)
{
	const datum hlmt_tag_index = tags::get_tag_fast<s_object_group_definition>(object_get_fast_unsafe(DATUM_INDEX_TO_ABSOLUTE_INDEX(object_datum))->tag_definition_index)->model.TagIndex;

	if (hlmt_tag_index == DATUM_INDEX_NONE) { return false; }

	const s_model_group_definition* hlmt_model_definition = tags::get_tag_fast<s_model_group_definition>(hlmt_tag_index);
	const datum render_model_tag_index = hlmt_model_definition->render_model.TagIndex;

	if (render_model_tag_index == -1 || hlmt_model_definition->collision_model.TagIndex == -1) { return false; }

	const s_render_model_group_definition* render_model = tags::get_tag_fast<s_render_model_group_definition>(render_model_tag_index);

	*mode_tag_index = render_model_tag_index;
	*coll_tag_index = hlmt_model_definition->collision_model.TagIndex;

	bool contains_prt_info = (render_model->prt_info.size <= 0 || render_model->section_render_leaves.size <= 0 ? false : true);
	return contains_prt_info;
}

datum object_header_new(__int16 object_data_size)
{
	const datum object_datum = datum_new(get_object_data_array());
	s_object_header* object_header = get_object_header(object_datum);
	if (object_datum == DATUM_INDEX_NONE) { return object_datum; }

	if (memory_pool_block_allocate_handle(get_object_table(), &object_header->object, object_data_size, 0, 0))
	{
		s_object_data_definition* object = (s_object_data_definition*)object_header->object;
		object_header->object_data_size = object_data_size;
		memset(object, 0, object_data_size);
		return object_datum;
	}
	else
	{
		datum_delete(get_object_data_array(), DATUM_INDEX_TO_ABSOLUTE_INDEX(object_datum));
		return -1;
	}
	return object_datum;
}

// Computes the new change color values of an object 
// Used for objects with the functions datablock set inside the change color tagblock
bool object_compute_change_colors(datum object_datum)
{
	// I really don't wanna get into this can of worms yet...
	typedef bool(__cdecl* object_get_function_value_simple_t)(int object_datum, string_id function_name, float* function_value);
	auto p_object_get_function_value_simple = Memory::GetAddress<object_get_function_value_simple_t>(0x133005, 0x121ED5);


	s_object_data_definition* object = object_get_fast_unsafe<s_object_data_definition>(object_datum);
	const s_object_group_definition* obje_definition = tags::get_tag_fast<s_object_group_definition>(object->tag_definition_index);

	if ((obje_definition->runtime_flags & s_object_group_definition::e_object_group_runtime_runtime_change_colors_allowed) == 0) { return false; }

	DWORD number_of_change_colors = 0;
	real_color_rgb* object_block_change_colors = (real_color_rgb*)object_header_block_get_with_count(object_datum, &object->change_color_block, sizeof(real_color_rgb), &number_of_change_colors);
	
	if (number_of_change_colors <= 0) { return false; }
	
	DWORD half_of_cc_count = number_of_change_colors >> 1;
	
	for (DWORD i = 0; i < half_of_cc_count; ++i)
	{
		real_color_rgb* current_change_color = &object_block_change_colors[half_of_cc_count + i];
		*current_change_color = object_block_change_colors[i];
		
		// Loop through function blocks inside change color block
		// This has been edited from the original since the original one was stupid and referenced a pre-processed value within the tagblock itself for the new tagblock index
		// Turns out Halo 3 dosent do any of this garbage so referenced that when rewriting this
		// Long story short we just loop through every block instead now
		const s_object_group_definition::s_change_colors_block::s_change_colors_functions_block * function_definition = nullptr;
		if (obje_definition->change_colors[i]->functions.size > 0)		// Crashes here
		{
			string_id function_name = NULL;

			// Interpolates colours by function type (if it exists)
			for (int j = 0; j < obje_definition->change_colors[i]->functions.size; ++j)
			{
				function_definition = obje_definition->change_colors[i]->functions[j];
				function_name = function_definition[j].scale_by;

				if (function_name.is_valid())
				{
					float function_value;
					if (!p_object_get_function_value_simple(object_datum, function_name, &function_value)) { function_value = 0.0; }
					
					rgb_colors_interpolate(
						current_change_color,
						function_definition[j].scale_flags,
						&function_definition[j].color_lower_bound,
						&function_definition[j].color_upper_bound,
						function_value);
				}
			}

			// Interpolates colours by function type (if it exists)
			for (int j = 0; j < obje_definition->change_colors[i]->functions.size; ++j)
			{
				function_definition = obje_definition->change_colors[i]->functions[j];

				function_name = function_definition[j].darken_by;
				if (function_name.is_valid())
				{
					float function_value;
					if (!p_object_get_function_value_simple(object_datum, function_name, &function_value)) { function_value = 0.0; }
					current_change_color->red   *= function_value;
					current_change_color->green *= function_value;
					current_change_color->blue  *= function_value;
				}
			}
		}

		if (current_change_color->red >= 0.0f)
		{
			if (current_change_color->red > 1.0f)
				current_change_color->red = 1.0f;
		}
		else
		{
			current_change_color->red = 0.0f;
		}

		if (current_change_color->green >= 0.0f)
		{
			if (current_change_color->green > 1.0f)
				current_change_color->green = 1.0f;
		}
		else
		{
			current_change_color->green = 0.0f;
		}

		if (current_change_color->blue >= 0.0f)
		{
			if (current_change_color->blue > 1.0f)
				current_change_color->blue = 1.0f;
		}
		else
		{
			current_change_color->blue = 0.0f;
		}
	}
	return true;
}

void free_object_memory(const datum object_datum)
{
	typedef int(__cdecl* memory_pool_block_free_t)(s_memory_pool* memory_pool, void** payload_data);
	auto p_memory_pool_block_free = Memory::GetAddress<memory_pool_block_free_t>(0x8BD80, 0x81924);

	s_object_header* object_header = get_object_header(object_datum);
	object_header->flags = (e_object_header_flag)0;
	if (object_header->object != nullptr)
	{
		p_memory_pool_block_free(get_object_table(), &object_header->object);
	}
	datum_delete(get_object_data_array(), DATUM_INDEX_TO_ABSOLUTE_INDEX(object_datum));
}

void object_reset_interpolation(datum object_datum)
{
	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	c_animation_manager* animation_manager = (c_animation_manager*)object_header_block_get(object_datum, &object->animation_manager_block);

	animation_manager->interpolator_control_1.disable();
	object_wake(object_datum);
}

// Gets new damage info block from the model tag related to the object datum
s_model_group_definition::s_new_damage_info_block* object_get_damage_info(const datum object_datum)
{
	if (object_datum == -1) { return nullptr; }

	const datum hlmt_datum = tags::get_tag_fast<s_object_group_definition>(object_get_fast_unsafe(object_datum)->tag_definition_index)->model.TagIndex;

	if (hlmt_datum == -1) { return nullptr; }

	const s_model_group_definition* hlmt_definition = tags::get_tag_fast<s_model_group_definition>(hlmt_datum);

	if (hlmt_definition->new_damage_info.size <= 0)
	{
		return nullptr;
	}

	if (hlmt_definition->new_damage_info.data != -1)
	{
		return hlmt_definition->new_damage_info[0];
	}

	return nullptr;
}

// Initializes health and shields for the given object based on two floats passed to it
// If the floats for vitality are nullptrs we just initialize them to the default values in the damage info block in the model tag
void object_initialize_vitality(const datum object_datum, const float* new_vitality, const float* new_shield_vitality)
{
	s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	const s_model_group_definition::s_new_damage_info_block* damage_info = object_get_damage_info(object_datum);

	float current_shield_vitality = 0.0f;
	float maximum_body_vitality = 0.0f;
	float maximum_shield_vitality = 0.0f;
	float current_body_vitality = 1.0f;

	// if damage info block is valid
	if (damage_info)
	{
		maximum_body_vitality = damage_info->maximum_vitality;
		maximum_shield_vitality = damage_info->maximum_shield_vitality;
	}

	// Set new vitality if one is passed to the function
	if (new_vitality)
	{
		maximum_body_vitality = *new_vitality;
	}

	// Set new shield vitality if one is passed to the function
	if (new_shield_vitality)
	{
		maximum_shield_vitality = *new_shield_vitality;
	}
	
	// Make sure body vitality isn't negative
	if (maximum_body_vitality <= 0.0f)
	{
		current_body_vitality = 0.0f;
	}

	// If max shield vitality is greater than 0 set our shields to full
	if (maximum_shield_vitality > 0.0f)
	{
		current_shield_vitality = 1.0f;
	}

	object->body_max_vitality = maximum_body_vitality;
	object->shield_max_vitality = maximum_shield_vitality;
	object->body_current_vitality = current_body_vitality;
	object->shield_current_vitality = current_shield_vitality;
}

// Allocates memory for object header blocks within an object data struct
bool object_header_block_allocate(const datum object_datum, const short size, const short padded_size, const short alignment_bits)
{
	s_object_header* object_header = get_object_header(object_datum);
	if (padded_size)
	{
		int alignment_minus_one = (1 << alignment_bits) - 1;
		unsigned int alignment_plus_pad = alignment_minus_one + padded_size;
		if (memory_pool_block_reallocate_handle(
			get_object_table(),
			(s_object_data_definition*)&object_header->object,
			(char*)(alignment_plus_pad + object_header->object_data_size),
			nullptr,
			nullptr))
		{
			__int16 object_data_size = object_header->object_data_size;
			object_header->object_data_size = alignment_plus_pad + object_data_size;
			object_header_block_reference* main_object_header_block = (object_header_block_reference*)((byte*)object_get_fast_unsafe(object_datum) + size);

			main_object_header_block->offset = ~(WORD)alignment_minus_one & (alignment_minus_one + object_data_size);
			main_object_header_block->size = padded_size;
			memset((byte*)object_header->object + object_data_size, 0, alignment_plus_pad);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		object_header_block_reference* object_header_block = (object_header_block_reference*)((char*)object_header->object + size);
		object_header_block->offset = -1;
		object_header_block->size = 0;
		return true;
	}
}

typedef datum(__cdecl* p_object_new_t)(object_placement_data* placement_data);
p_object_new_t p_object_new;
// Creates a new object
datum __cdecl object_new(object_placement_data* placement_data)
{
	typedef bool(__cdecl* havok_can_allocate_space_for_instance_of_object_definition_t)(datum a1);
	auto p_havok_can_allocate_space_for_instance_of_object_definition = Memory::GetAddress<havok_can_allocate_space_for_instance_of_object_definition_t>(0x9FE55, 0x920B5);

	typedef void(__cdecl* sub_532F07_t)(datum arg0, int arg4);
	auto update_object_region_information = Memory::GetAddress<sub_532F07_t>(0x132F07, 0x121DD7);

	typedef D3DCOLOR(__cdecl* sub_5310F9_t)(datum a1, int a2, real_color_rgb* a3);
	auto p_object_set_initial_change_colors = Memory::GetAddress<sub_5310F9_t>(0x1310F9, 0x11FFBD);

	typedef void(__cdecl* object_reconnect_to_physics_t)(datum object_datum);
	auto p_object_reconnect_to_physics = Memory::GetAddress< object_reconnect_to_physics_t>(0x1323B3, 0x121282);
	
	typedef void(__cdecl* effect_new_from_object_t)(datum effect_tag_index,
		s_damage_owner* damage_owner,
		datum object_datum,
		float a4,
		float a5,
		const real_color_rgb* color,
		const void* effect_vector_field);
	auto p_effect_new_from_object = Memory::GetAddress<effect_new_from_object_t>(0xAADCE, 0x9CE4E);

	typedef void(__cdecl* sub_66CFDD_t)(datum object_datum);
	auto p_sub_66CFDD = Memory::GetAddress<sub_66CFDD_t>(0x26CFDD);

	if ((placement_data->object_placement_flags & 0x10) == 0)
	{
		if (placement_data->tag_index == DATUM_INDEX_NONE) { return DATUM_INDEX_NONE; }

		object_type_adjust_placement(placement_data);
	}
	if (placement_data->tag_index == DATUM_INDEX_NONE) { return DATUM_INDEX_NONE; }

	const s_object_group_definition* new_object_tag = tags::get_tag_fast<s_object_group_definition>(placement_data->tag_index);
	const object_type_definition* object_type_definition = object_type_definition_get((e_object_type)new_object_tag->object_type);

	const s_model_group_definition* model_definition = nullptr;
	if (new_object_tag->model.TagIndex != DATUM_INDEX_NONE)
	{
		model_definition = tags::get_tag_fast<s_model_group_definition>(new_object_tag->model.TagIndex);
	}

	if ((FLAG(new_object_tag->object_type) & 
		(FLAG(e_object_type::creature) | 
			FLAG(e_object_type::crate) | 
			FLAG(e_object_type::machine) | 
			FLAG(e_object_type::vehicle) | 
			FLAG(e_object_type::biped))) != 0)
	{
		typedef void(__cdecl* havok_memory_garbage_collect_t)();
		havok_memory_garbage_collect_t p_havok_memory_garbage_collect = Memory::GetAddress<havok_memory_garbage_collect_t>(0xF7F78, 0xDEDB3);
		p_havok_memory_garbage_collect();
	}

	datum object_datum = object_header_new(object_type_definition->datum_size);

	if (object_datum == DATUM_INDEX_NONE) { return DATUM_INDEX_NONE; }

	s_object_header* object_header = get_object_header(object_datum);
	s_object_data_definition* object = object_get_fast_unsafe<s_object_data_definition>(object_datum);

	object_header->flags |= e_object_header_flag::_object_header_flags_8;
	object_header->object_type = (e_object_type)new_object_tag->object_type;
	object->tag_definition_index = placement_data->tag_index;

	// If placement type is null then make it use the default placement type
	if (placement_data->placement_info.placement_type == UCHAR_MAX)
	{
		++s_object_globals::get()->unique_id;
		object->placement_info.object_type = (e_object_type)new_object_tag->object_type;
		object->placement_info.placement_type = 2;
		object->placement_info.origin_bsp_index = -1;
		object->unique_id = s_object_globals::get()->unique_id;
		object->placement_index = -1;
		object->structure_bsp_index = (byte)get_global_structure_bsp_index();
	}
	else
	{
		object->unique_id = placement_data->unique_id;
		object->placement_info = placement_data->placement_info;
		object->placement_index = (short)placement_data->placement_index;
		object->structure_bsp_index = (byte)placement_data->placement_info.origin_bsp_index;
	}

	object->position = placement_data->position;
	object->orientation = placement_data->forward;
	object->up = placement_data->up;
	object->translational_velocity = placement_data->translational_velocity;
	object->angular_velocity = placement_data->angular_velocity;
	object->scale = placement_data->scale;

	if ((placement_data->object_placement_flags & 1) != 0)
	{
		object->object_flags |= object_data_flag_0x400;
	}
	else
	{
		object->object_flags &= ~object_data_flag_0x400;
	}

	// Can we collide
	if (model_definition && model_definition->collision_model.TagIndex != DATUM_INDEX_NONE)
	{
		object->object_flags |= _object_has_collision_bit;
	}
	else
	{
		object->object_flags &= ~_object_has_collision_bit;
	}

	// tag_indexes arent actually used, they're just passed into the function but aren't used later on
	if (datum tag_indexes[2]; object_render_model_has_prt_info(object_datum, &tag_indexes[0], &tag_indexes[1]) == true)
	{ 
		object->object_flags |= _object_has_prt_or_lighting_info;
	}
	else { object->object_flags &= ~_object_has_prt_or_lighting_info; }

	object_header->cluster_index = -1;
	location_invalidate(&object->location);
	object->first_cluster_reference = -1;
	object->parent_datum = DATUM_INDEX_NONE;
	object->next_index = DATUM_INDEX_NONE;
	object->current_weapon_datum = DATUM_INDEX_NONE;
	object->name_list_index = (short)DATUM_INDEX_NONE;
	object->netgame_equipment_index = -1;
	object->byte_108 = -1;
	object->byte_109 = -1;
	object->placement_policy = placement_data->placement_policy;
	if ((new_object_tag->object_flags & s_object_group_definition::e_object_flags::does_not_cast_shadow) != 0)  
	{ 
		object->object_flags |= _object_does_not_cast_shadow_bit; 
	}

	if ((object->object_flags & object_data_flag_0x1) != 0)
	{
		object->object_flags &= ~object_data_flag_0x1;
		if (object_is_connected_to_map(object_datum)) 
		{ 
			object_connect_lights_recursive(object_datum, false, true, false, false); 
		}
		object_update_collision_culling(object_datum);
	}
	object->damage_owner_unk3 = placement_data->damage_owner.unk3;
	object->damage_owner_unk1 = placement_data->damage_owner.unk1;
	object->damage_owner_unk2 = placement_data->damage_owner.unk2;
	object->model_variant_id = -1;
	object->field_CC = -1;
	object->field_D0 = -1;
	object->physics_flags = (e_object_physics_flags)0;

	if ((placement_data->object_placement_flags & 8) != 0)
		object->physics_flags |= _object_physics_flag_0x100;
	else
		object->physics_flags &= ~_object_physics_flag_0x100;

	object->havok_datum = -1;
	object->simulation_entity_index = -1;
	object->b_attached_to_simulation = false;
	object->destroyed_constraints_flag = placement_data->destroyed_constraints_flag;
	object->loosened_constraints_flag = placement_data->loosened_constraints_flag;

	size_t nodes_count = 1;
	size_t collision_regions_count = 1;
	size_t damage_info_damage_sections_size = 0;

	bool allow_interpolation = false;
	bool valid_animation_manager = false;

	if (model_definition)
	{
		if (model_definition->nodes.size >= 1) { nodes_count = model_definition->nodes.size; }
		if (model_definition->collision_regions.size >= 1) { collision_regions_count = model_definition->collision_regions.size;}

		if (model_definition->new_damage_info.size > 0 && model_definition->new_damage_info.data != -1) 
		{ 
			damage_info_damage_sections_size = model_definition->new_damage_info[0]->damage_sections.size;
		}

		if (model_definition->animation.TagIndex != DATUM_INDEX_NONE)
		{
			c_animation_manager animation_manager;
			if (animation_manager.reset_graph(model_definition->animation.TagIndex, new_object_tag->model.TagIndex, true))
			{
				valid_animation_manager = true;
				allow_interpolation = ((FLAG(new_object_tag->object_type)) &
					(FLAG(e_object_type::sound_scenery) |
					FLAG(e_object_type::light_fixture) |
					FLAG(e_object_type::control) |
					FLAG(e_object_type::machine) |
					FLAG(e_object_type::scenery) |
					FLAG(e_object_type::projectile))) == 0;

				// allow interpolation if object is device and device flags include interpolation
				if ((FLAG(new_object_tag->object_type) & (e_object_type::light_fixture | e_object_type::control | e_object_type::machine)) != 0
					&& (((const s_device_group_definition*)new_object_tag)->flags & s_device_group_definition::e_device_group_flag_allow_interpolation) != 0)
				{
					allow_interpolation = true;
				}
			}
		}
	}

	short original_orientations = (!allow_interpolation ? 0 : 32 * (short)nodes_count);

	// Allocate object header blocks
	bool b_can_create_object = 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::object_attachments_block), (short)(8 * new_object_tag->attachments.size), 0);
	b_can_create_object = b_can_create_object && 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::damage_sections_block), (short)(8 * damage_info_damage_sections_size), 0);
	b_can_create_object = b_can_create_object && 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::change_color_block), (short)(24 * new_object_tag->change_colors.size), 0);
	b_can_create_object = b_can_create_object && 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::nodes_block), (short)(52 * nodes_count), 0);
	b_can_create_object = b_can_create_object && 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::collision_regions_block), (short)(10 * collision_regions_count), 0);
	b_can_create_object = b_can_create_object && 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::node_orientation_block), original_orientations, 4);
	b_can_create_object = b_can_create_object && 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::original_orientation_block), original_orientations, 4);
	b_can_create_object = b_can_create_object && 
		object_header_block_allocate(object_datum, offsetof(s_object_data_definition, s_object_data_definition::animation_manager_block), (short)(valid_animation_manager ? 144 : 0), 0);
	b_can_create_object = b_can_create_object && p_havok_can_allocate_space_for_instance_of_object_definition(placement_data->tag_index);

	// If one of the object headers cannot be allocated then something has gone horribly wrong and we can't create our object
	bool b_out_of_objects = !b_can_create_object;

	if (b_can_create_object)
	{
		if (valid_animation_manager)
		{
			c_animation_manager* animation_manager = (c_animation_manager*)object_header_block_get(object_datum, &object->animation_manager_block);

			animation_manager->initialize();
			if (animation_manager->reset_graph(model_definition->animation.TagIndex, new_object_tag->model.TagIndex, true))
				object->object_flags |= object_data_flag_0x800;
			else
				object->object_flags &= ~object_data_flag_0x800;
		}

		// Null attachment block
		if (new_object_tag->attachments.size > 0)
		{
			DWORD attachments_count;
			s_object_group_definition::s_attachments_block* object_attachments_block = (s_object_group_definition::s_attachments_block*)object_header_block_get_with_count(
				object_datum,
				&object->object_attachments_block,
				sizeof(s_object_group_definition::s_attachments_block),
				&attachments_count);

			memset(object_attachments_block, -1, sizeof(s_object_group_definition::s_attachments_block) * attachments_count);
		}

		if (object_type_new(object_datum, placement_data, &b_out_of_objects))
		{
			bool b_object_flag_check = (object->object_flags & object_data_flag_0x20000) != 0;
			if ((placement_data->object_placement_flags & 6) != 0)
			{
				object->object_flags &= ~object_data_flag_0x20000;
			}

			update_object_variant_index(object_datum, placement_data->variant_name);
			update_object_region_information(object_datum, placement_data->region_index);
			p_object_set_initial_change_colors(object_datum, placement_data->active_change_colors_mask, placement_data->change_colors);
			object_initialize_vitality(object_datum, nullptr, nullptr);
			object_compute_change_colors(object_datum);
			object->foreground_emblem = placement_data->foreground_emblem;

			if (object->animation_manager_block.offset != DATUM_INDEX_NONE)
			{
				object_reset_interpolation(object_datum);
			}

			object_compute_node_matrices_with_children(object_datum);

			// If the object (can) connect to the map we make sure it gets connected
			if (s_object_globals::objects_can_connect_to_map())
			{
				s_location* p_location = nullptr;
				placement_data->object_is_inside_cluster = set_object_position_if_in_cluster(&placement_data->location, object_datum);
				if (placement_data->object_is_inside_cluster)
				{
					p_location = &placement_data->location;
				}

				object_reconnect_to_map(p_location, object_datum);
			}

			object_postprocess_node_matrices(object_datum);
			object_wake(object_datum);

			if ((placement_data->object_placement_flags & 0x20) != 0)
			{
				object->physics_flags |= _object_physics_flag_0x2;
			}
			else
			{
				object->physics_flags &= ~_object_physics_flag_0x2;
			}

			p_object_reconnect_to_physics(object_datum);
			object_initialize_effects(object_datum);
			object_type_create_children(object_datum);

			if (new_object_tag->creation_effect.TagIndex != DATUM_INDEX_NONE)
			{
				p_effect_new_from_object(new_object_tag->creation_effect.TagIndex, &placement_data->damage_owner, object_datum, 0.0f, 0.0f, nullptr, nullptr);
			}

			// Not 100% sure what this function does but it has to do with occlusion
			// This function is nulled out on the dedi
			if (!Memory::IsDedicatedServer()) { p_sub_66CFDD(object_datum); }

			if (b_object_flag_check)
			{
				object->object_flags |= object_data_flag_0x20000;
			}
			else
			{
				object->object_flags &= ~object_data_flag_0x20000;
			}

			object_early_mover_new(object_datum);

			if ((object->object_flags & object_data_flag_0x20000) == 0 || (object_header->flags & _object_header_active_bit) != 0) { return object_datum; }

			if (!s_object_globals::objects_can_connect_to_map()) { return object_datum; }

			if ((placement_data->object_placement_flags & 2) == 0 && ( (placement_data->object_placement_flags & 4) == 0 || object->location.cluster != -1) ) 
			{ 
				object_delete(object_datum);
			}
			return object_datum;
		}
		object_type_delete(object_datum);
	}
	free_object_memory(object_datum);
	return DATUM_INDEX_NONE;
}

#pragma region Biped variant patches
void update_biped_object_variant_data(datum object_idx, string_id variant_stringid)
{
	s_biped_data_definition* biped_object = (s_biped_data_definition*)object_try_and_get_and_verify_type(object_idx, FLAG(e_object_type::biped));
	// set this data only if we are dealing with a biped
	if (biped_object != NULL)
	{
		biped_object->variant_name = variant_stringid;
	}
}

typedef void(__stdcall* c_simulation_unit_entity_definition_creation_encode_t)(void* thisptr, int creation_data_size, void* creation_data, int a3, bitstream* stream);
c_simulation_unit_entity_definition_creation_encode_t p_c_simulation_unit_entity_definition_encode;

void __stdcall c_simulation_unit_entity_definition_creation_encode_hook(void* thisptr, int creation_data_size, s_simulation_unit_creation_data* creation_data, int a3, bitstream* stream)
{
	int object_permutation_idx = creation_data->variant_name.get_packed();
	stream->data_encode_integer("object-permutation-index", object_permutation_idx, 32);
	//addDebugText("encoded entity creation: variant index: %d", object_permutation_index);
	p_c_simulation_unit_entity_definition_encode(thisptr, creation_data_size, creation_data, a3, stream);
}

typedef bool(__stdcall* c_simulation_unit_entity_definition_creation_decode_t)(void* thisptr, int creation_data_size, void* creation_data, bitstream* stream);
c_simulation_unit_entity_definition_creation_decode_t p_c_simulation_unit_entity_definition_decode;

bool __stdcall c_simulation_unit_entity_definition_creation_decode_hook(void* thisptr, int creation_data_size, s_simulation_unit_creation_data* creation_data, bitstream* stream)
{
	int object_permutation_idx = stream->data_decode_integer("object-permutation-index", 32);
	creation_data->variant_name = object_permutation_idx;
	//addDebugText("decoded entity creation: variant index: %d", object_permutation_index);
	return p_c_simulation_unit_entity_definition_decode(thisptr, creation_data_size, creation_data, stream);
}

int __stdcall c_simulation_object_entity_definition_object_create_object_hook(int thisx,
	s_simulation_unit_creation_data* 
	creation_data, 
	int a2, 
	int a3, 
	object_placement_data* placement_data)
{
	// set the object placement data
	placement_data->variant_name = creation_data->variant_name;
	if(*(byte*)((char*)creation_data + 0x10) != -1)
	{
		auto profile = reinterpret_cast<s_player::s_player_properties::s_player_profile*>((char*)creation_data + 0x10);
		datum player_representation_datum = PlayerRepresentation::get_object_datum_from_representation(profile->player_character_type);
		if (player_representation_datum != DATUM_INDEX_NONE)
			placement_data->tag_index = player_representation_datum;
	}
	//addDebugText("creating object with variant index: %d", object_placement_data->variant_name);
	return Memory::GetAddress<int(__thiscall*)(int, void*, int, int, object_placement_data*)>(0x1F32DB, 0x1DE374)(thisx, creation_data, a2, a3, placement_data);
}

__declspec(naked) void c_simulation_object_entity_definition_object_create_object_to_stdcall()
{
	__asm
	{
		pop eax // pop return address
		push ecx // push ecx as first param
		push eax // push the return address back on stack
		jmp c_simulation_object_entity_definition_object_create_object_hook
	}
}

typedef bool(__cdecl* set_unit_color_data_t)(int, unsigned __int16, int a3);
set_unit_color_data_t p_set_unit_color_data;

bool __cdecl set_unit_color_data_hook(int a1, unsigned __int16 a2, int a3)
{
	auto object_creation_data = (s_simulation_unit_creation_data*)(a1 - 0x10);
	int object_permutation_idx = object_creation_data->variant_name.get_packed();

	//if (object_permutation_index == 0)
	if(*(byte*)((char*)a1) != -1)
		return p_set_unit_color_data(a1, a2, a3);

	return 0;
}

void __stdcall object_build_creation_data_hook(datum object_idx, s_simulation_unit_creation_data* object_creation_data)
{
	auto p_object_build_creation_data = Memory::GetAddress<void(__stdcall*)(datum, void*)>(0x1F24ED, 0x1DD586);

	p_object_build_creation_data(object_idx, object_creation_data);

	auto object = object_get_fast_unsafe<s_biped_data_definition>(object_idx);

	object_creation_data->variant_name = object->variant_name;

	//addDebugText("object build creation data: variant index: %d", object->variant_index);
}
#pragma endregion

void apply_object_hooks()
{
	LOG_INFO_GAME("{} - applying hooks", __FUNCTION__);

	// increase the data size for biped representation
	WriteValue<unsigned short>(Memory::GetAddressRelative(0x81E9A8, 0x7C1EB8) + 8, sizeof(s_biped_data_definition));

	// increase the size of the unit entity creation definition packet
	WriteValue<DWORD>(Memory::GetAddress(0x1F8028, 0x1E1D8E) + 1, sizeof(s_simulation_unit_creation_data));

	// increase the minimum required bitstream size
	WriteValue<BYTE>(Memory::GetAddressRelative(0x5F84B9, 0x5E221F) + 3, 5 + 32); // + 32 is quite much, but idk how bitstream will behave with less than 32 when dealin with integers

	PatchCall(Memory::GetAddressRelative(0x5F807A, 0x5E1DE0), object_build_creation_data_hook);

	//This encodes the unit creation packet, only gets executed on host.
	p_c_simulation_unit_entity_definition_encode = (c_simulation_unit_entity_definition_creation_encode_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8503, 0x1E2269), (BYTE*)c_simulation_unit_entity_definition_creation_encode_hook, 10);

	//This decodes the unit creation packet, only gets executed on client.
	p_c_simulation_unit_entity_definition_decode = (c_simulation_unit_entity_definition_creation_decode_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8557, 0x1E22BD), (BYTE*)c_simulation_unit_entity_definition_creation_decode_hook, 11);

	// Hooks a call within the creat_unit property on the client side in order to set their permutation index before spawn.
	PatchCall(Memory::GetAddress(0x1F9E6C, 0x1E3BD4), c_simulation_object_entity_definition_object_create_object_to_stdcall);

	// Hooks the part of the unit spawn from simulation that handles setting their color data in order to ensure AI do not have their color overridden
	PatchCall(Memory::GetAddress(0x1F9E34, 0x1E3B9C), set_unit_color_data_hook);
	p_set_unit_color_data = Memory::GetAddress<set_unit_color_data_t>(0x6E5C3, 0x6D1BF);


	DETOUR_BEGIN();
	DETOUR_ATTACH(p_object_new, Memory::GetAddress<p_object_new_t>(0x136CA7, 0x125B77), object_new);
	DETOUR_COMMIT();
}

int object_get_count()
{
	s_data_iterator object_it(get_object_data_array());
	return object_it.get_data_count();
}

int object_count_from_iter()
{
	s_data_iterator object_it(get_object_data_array());
	int count = 0;
	while (object_it.get_next_datum())
	{
		count++;
	}
	return count;
}
