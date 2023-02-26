#include "stdafx.h"
#include "objects.h"

#include "object_early_movers.h"
#include "object_globals.h"

#include "Blam/Cache/TagGroups/device_definition.hpp"
#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "Blam/Cache/TagGroups/model_definition.hpp"
#include "Blam/Cache/TagGroups/render_model_definition.hpp"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/memory/bitstream.h"
#include "Blam/Engine/memory/memory_pool.h"
#include "Blam/Engine/Players/Players.h"
#include "Blam/Engine/physics/bsp3d.h"
#include "Blam/Engine/physics/collision_bsp.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Blam/Engine/structures/cluster_partitions.h"
#include "Blam/Engine/structures/structures.h"
#include "Blam/Engine/Simulation/GameInterface/SimulationGameUnits.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

void create_new_placement_data(object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, int a4)
{
	LOG_TRACE_GAME("{}: {:X}, object_owner: {:X}, unk: {:X})", __FUNCTION__, object_definition_idx, object_owner_idx, a4);

	typedef void(__cdecl* object_placement_data_new_t)(void*, datum, datum, int);
	auto p_object_placement_data_new = Memory::GetAddress<object_placement_data_new_t>(0x132163, 0x121033);

	p_object_placement_data_new(object_placement_data, object_definition_idx, object_owner_idx, a4);
}

//Pass new placement data into Create_object_new
/*
datum object_new(object_placement_data* object_placement_data)
{
	LOG_TRACE_GAME("{}", __FUNCTION__);

	typedef datum(__cdecl* object_new_t)(object_placement_data*);
	auto p_object_new = Memory::GetAddress<object_new_t>(0x136CA7, 0x125B77);

	return p_object_new(object_placement_data);
}*/

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

void object_wake(const datum object_datum)
{
	typedef void(__cdecl* object_wake_t)(const datum object_datum);
	auto object_wake = Memory::GetAddress<object_wake_t>(0x12FA1E, 0x11E8E1);

	object_wake(object_datum);
}

void __cdecl object_disconnect_from_map(const datum object_index)
{
	typedef void(__cdecl* object_disconnect_from_map_t)(const datum object_index);
	auto object_disconnect_from_map = Memory::GetAddress<object_disconnect_from_map_t>(0x136266, 0x125136);

	object_disconnect_from_map(object_index);
}

void __cdecl object_activate(const datum object_index)
{
	typedef void(__cdecl* object_activate_t)(const datum object_index);
	auto object_activate = Memory::GetAddress<object_activate_t>(0x13204A);

	object_activate(object_index);
}

void __cdecl object_deactivate(const datum object_index)
{
	typedef void(__cdecl* object_deactivate_t)(const datum object_index);
	auto object_deactivate = Memory::GetAddress<object_deactivate_t>(0x1320B8);
	object_deactivate(object_index);
}

void __cdecl object_cleanup_havok(const datum object_index)
{
	typedef void(__cdecl* object_cleanup_havok_t)(const datum object_index);
	auto object_cleanup_havok = Memory::GetAddress<object_cleanup_havok_t>(0xA2778);
	object_cleanup_havok(object_index);
}

bool __cdecl simulation_query_object_is_predicted(const datum object_index)
{
	typedef bool(__cdecl* simulation_query_object_is_predicted_t)(const datum object_index);
	auto simulation_query_object_is_predicted = Memory::GetAddress<simulation_query_object_is_predicted_t>(0x1B6494);
	return simulation_query_object_is_predicted(object_index);
}

bool object_can_activate_in_cluster(datum object_datum, DWORD* cluster_activation)
{
	bool result = false;
	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	const s_object_header* object_header = get_object_header(object_datum);

	if ((object->object_flags & 2) != 0)
		return true;
	if ((FLAG(object_header->object_type) & FLAG(machine)) != 0 && object_type_compute_activation(object_datum, cluster_activation, &result) || object_header->cluster_index == -1)
	{
		return result;
	}
	return (FLAG(object_header->cluster_index & 0x1F) & cluster_activation[object_header->cluster_index >> 5]) != 0;
}

void get_object_payload(datum object_datum, s_object_payload* cluster_payload)
{
	typedef WORD (__cdecl* collision_compute_object_cull_flags_t)(datum object_index);
	auto p_collision_compute_object_cull_flags = Memory::GetAddress<collision_compute_object_cull_flags_t>(0xCBE87);

	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	WORD cull_flags = 0;
	if ((object->object_flags & object_data_flag_has_collision) != 0)
		cull_flags = p_collision_compute_object_cull_flags(object_datum);

	cluster_payload->object_type = object->placement_info.object_type;
	cluster_payload->cull_flags = cull_flags;
	cluster_payload->origin_point = object->object_origin_point;
	cluster_payload->bounding_sphere_radius = object->shadow_sphere_radius;
}

void __cdecl object_reconnect_to_map(s_location* location, const datum object_datum)
{
	typedef void(__cdecl* cluster_partition_reconnect_t)(
		cluster_partition* partition,
		datum object_datum,
		DWORD* first_cluster_reference,
		real_point3d* position,
		float radius,
		s_location* location,
		void* cluster_bitvector,
		int payload_size,
		void* payload,
		bool* cluster_overflow);
	auto p_cluster_partition_reconnect = Memory::GetAddress<cluster_partition_reconnect_t>(0x37A13E);

	typedef void(__cdecl* object_connect_lights_recursive_t)(
		datum object_datum,
		bool disconnect_this_object,
		bool reconnect_this_object,
		bool reconnect_child_objects,
		bool show_this_object);
	auto p_object_connect_lights_recursive = Memory::GetAddress<object_connect_lights_recursive_t>(0x134A78);

	typedef DWORD*(__cdecl* game_get_cluster_activation_t)();
	auto p_game_get_cluster_activation = Memory::GetAddress<game_get_cluster_activation_t>(0x499FD);

	s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	s_object_header* object_header = get_object_header(object_datum);
	bool cluster_index_is_null = (object_header->cluster_index == -1);
	s_location scnr_location;

	if (!location)
	{
		scenario_location_from_point(&scnr_location, &object->object_origin_point);
		location = &scnr_location;
		if (location->cluster == 0xFFFF)
		{
			scenario_location_from_point(&scnr_location, &object->position);
		}
	}

	if (location->cluster == 0xFFFF)
	{
		object->object_flags |= object_data_flag_0x40000;
	}
	else
	{
		object->location = *location;
		object_header->cluster_index = location->cluster;
		object->object_flags &= ~object_data_flag_0x40000;
	}
	DWORD cluster_bitvector[16];
	bool cluster_overflow = false;
	if ((object->object_flags & object_data_flag_0x200000) != 0)
	{
		memset(cluster_bitvector, -1, 64);
	}

	s_object_payload payload;
	get_object_payload(object_datum, &payload);
	cluster_partition noncollideable_object_cluster_partition = *Memory::GetAddress<cluster_partition*>(0x4E45F8);
	cluster_partition collideable_object_cluster_partition = *Memory::GetAddress<cluster_partition*>(0x4E4604);

	cluster_partition* partition = &collideable_object_cluster_partition;
	if ((object->object_flags & object_data_flag_has_collision) == 0)
		partition = &noncollideable_object_cluster_partition;

	p_cluster_partition_reconnect(
		partition,
		object_datum,
		&object->first_cluster_reference,
		&object->object_origin_point,
		object->shadow_sphere_radius,
		&object->location,
		cluster_bitvector,
		20,
		&payload,
		&cluster_overflow);

	object->object_flags |= object_data_flag_0x100;
	object_header->flags |= _object_header_child_bit;
	p_object_connect_lights_recursive(object_datum, false, true, false, false);
	
	if (DWORD* cluster_activation = p_game_get_cluster_activation(); 
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
	if (cluster_index_is_null && object_header->cluster_index != 0xFFFF)
		object_cleanup_havok(object_datum);
}

void object_compute_node_matrices_with_children(const datum object_datum)
{
	typedef void(__cdecl* object_compute_node_matrices_non_recursive_t)(const datum object_datum);
	auto object_compute_node_matrices_non_recursive = Memory::GetAddress<object_compute_node_matrices_non_recursive_t>(0x1353E6);
	typedef bool(__cdecl* object_compute_node_matrices_locations_t)(const datum object_datum);
	auto object_compute_node_matrices_locations = Memory::GetAddress<object_compute_node_matrices_locations_t>(0x1363D5);

	const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	s_object_data_definition const* next_object = nullptr;
	object_compute_node_matrices_non_recursive(object_datum);
	object_compute_node_matrices_locations(object_datum);
	for (unsigned long i = object->current_weapon_datum; i != -1; i = next_object->next_index)
	{
		next_object = object_get_fast_unsafe(i);
		if ((FLAG(next_object->placement_info.object_type) & FLAG(machine)) == 0)
			object_compute_node_matrices_with_children(i);
	}
}

void object_evaluate_placement_variant(datum object_header_datum, string_id variant_index)
{
	typedef byte(__cdecl* sub_52FE84_t)(unsigned __int16 a1, string_id a2);
	auto p_sub_53FE84 = Memory::GetAddress<sub_52FE84_t>(0x12FE84);
	auto object = (s_object_data_definition*)get_object_header(object_header_datum)->object;

	object->model_variant_id = p_sub_53FE84(DATUM_INDEX_TO_ABSOLUTE_INDEX(object_header_datum), variant_index);
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
			typedef real_matrix4x3*(__cdecl* object_get_node_matrices_t)(datum, int* a2);
			auto p_object_get_node_matrices = Memory::GetAddress<object_get_node_matrices_t>(0x12FCA5);
			int node_count = 0;
			real_matrix4x3* node_matricies = p_object_get_node_matrices(object_index, &node_count);
			object_type_postprocess_node_matrices(object_index, node_count, node_matricies);
		}
	}
}

void object_initialize_effects(datum object_datum)
{
	typedef void(__cdecl* widgets_new_t)(datum a1);
	auto p_widgets_new = Memory::GetAddress< widgets_new_t>(0x14FFE2);

	typedef void(__cdecl* attachments_new_t)(datum a1);
	auto p_attachments_new = Memory::GetAddress< attachments_new_t>(0x1348CA);

	auto object_header = get_object_header(object_datum);
	s_object_data_definition* object = (s_object_data_definition*)object_header->object;
	if (object_header->object_type == projectile)
	{
		object->object_projectile_datum = -1;
	}
	else
	{
		p_widgets_new(object_datum);
	}

	p_attachments_new(object_datum);
}

bool set_object_position_if_in_cluster(s_location* location, const datum object_datum)
{
	typedef void(__cdecl* collision_bsp_test_sphere_t)(s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp,
		__int16 flags,
		void* breakable_surface_set,
		real_point3d* point,
		float scale,
		collision_bsp_test_sphere_result* test_result);
	auto p_collision_bsp_test_sphere = Memory::GetAddress<collision_bsp_test_sphere_t>(0xE9890);

	s_object_data_definition* object = object_get_fast_unsafe(object_datum);
	scenario_location_from_point(location, &object->object_origin_point);

	if (location->cluster != 0xFFFF)
		return true;

	collision_bsp_test_sphere_result test_result;
	auto g_collision_bsp = *Memory::GetAddress<s_scenario_structure_bsp_group_definition::s_collision_bsp_block**>(0x479E64);
	p_collision_bsp_test_sphere(g_collision_bsp, 0, nullptr, &object->object_origin_point, object->shadow_sphere_radius, &test_result);

	if (test_result.stack_depth)
		scenario_location_from_leaf(location, test_result.buffer3[0]);
	else
		scenario_location_from_point(location, &object->position);

	return location->cluster != 0xFFFF;
}

// Sets the mode_tag_index and coll_tag_index parameters (if they exist)
// Returns whether or not the render model has PRT or Lighting Info
bool object_render_model_has_prt_info(const datum object_datum, datum* mode_tag_index, datum* coll_tag_index)
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

datum __cdecl object_header_new(__int16 object_data_size)
{
	s_memory_pool* object_table = *Memory::GetAddress<s_memory_pool**>(0x4E4610);

	const datum object_datum = datum_new(get_object_data_array());
	s_object_header* object_header = get_object_header(object_datum);
	if (object_datum == -1)
		return object_datum;
	if (memory_pool_block_allocate_handle(object_table, &object_header->object, object_data_size, 0, 0))
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



// Not 100% sure this function works correctly 
// CHECK THIS BEFORE RELEASING
// Computes the new change color values of an object 
// Used for objects with the functions datablock set inside the change color tagblock
bool __cdecl object_compute_change_colors(datum object_datum)
{
	// I really don't wanna get into this can of worms yet...
	typedef bool(__cdecl* object_get_function_value_simple_t)(int object_datum, string_id function_name, float* function_value);
	auto p_object_get_function_value_simple = Memory::GetAddress<object_get_function_value_simple_t>(0x133005, 0x121ED5);


	const s_object_data_definition* object = object_get_fast_unsafe<s_object_data_definition>(object_datum);
	const s_object_group_definition* obje_definition = tags::get_tag_fast<s_object_group_definition>(object->tag_definition_index);

	if ((obje_definition->runtime_flags & s_object_group_definition::e_object_group_runtime_runtime_change_colors_allowed) == 0) { return false; }

	real_color_rgb* object_change_colors = (real_color_rgb*)((char*)object + object->change_color_offset);
	int number_of_change_colors = (object->change_color_size / sizeof(real_color_rgb)) / 2;
	if (number_of_change_colors <= 0) { return false; }

	real_color_rgb* color = &object_change_colors[number_of_change_colors];

	for (long i = 0; i < number_of_change_colors; ++i)
	{
		s_object_group_definition::s_change_colors_block* change_color_definition = nullptr;
		if (obje_definition->change_colors.data != -1) { change_color_definition = obje_definition->change_colors[i]; }

		color->red = object_change_colors->red;
		color->green = object_change_colors->green;
		color->blue = object_change_colors->blue;
		
		// Loop through function blocks inside change color block
		// This has been edited from the original since the original one was stupid and referenced a pre-processed value within the tagblock itself for the new tagblock index
		// Turns out Halo 3 dosent do any of this garbage so referenced that when rewriting this
		// Long story short we just loop through every block instead now
		const s_object_group_definition::s_change_colors_block::s_change_colors_functions_block * function_definition = nullptr;
		if (change_color_definition->functions.size > 0)
		{
			string_id function_name = NULL;

			// Interpolates colours by function type (if it exists)
			for (int j = 0; j < change_color_definition->functions.size; ++j)
			{
				function_definition = change_color_definition->functions[j];
				function_name = function_definition[j].scale_by;

				if (function_name.is_valid())
				{
					float function_value;
					if (!p_object_get_function_value_simple(object_datum, function_name, &function_value)) { function_value = 0.0; }
					
					rgb_colors_interpolate(
						color,
						function_definition[j].scale_flags,
						&function_definition[j].color_lower_bound,
						&function_definition[j].color_upper_bound,
						function_value);
				}
			}

			// Interpolates colours by function type (if it exists)
			for (int j = 0; j < change_color_definition->functions.size; ++j)
			{
				function_definition = change_color_definition->functions[j];

				function_name = function_definition[j].darken_by;
				if (function_name.is_valid())
				{
					float function_value;
					if (!p_object_get_function_value_simple(object_datum, function_name, &function_value)) { function_value = 0.0; }
					color->red   *= function_value;
					color->green *= function_value;
					color->blue  *= function_value;
				}
			}
		}

		float red;
		float green;
		float blue;
		red = color->red;
		if (color->red >= 0.0)
		{
			if (red > 1.0)
				red = 1.0;
		}
		else
		{
			red = 0.0;
		}
		color->red = red;
		green = color->green;
		if (green >= 0.0)
		{
			if (green > 1.0)
				green = 1.0;
		}
		else
		{
			green = 0.0;
		}
		color->green = green;
		blue = color->blue;
		if (blue >= 0.0)
		{
			if (blue > 1.0)
				blue = 1.0;
		}
		else
		{
			blue = 0.0;
		}
		color->blue = blue;
		++color;
	}
	return true;
}

void free_object_memory(unsigned __int16 a1)
{
	typedef int(__cdecl* memory_pool_block_free_t)(s_memory_pool* memory_pool, void** payload_data);
	auto p_memory_pool_block_free = Memory::GetAddress<memory_pool_block_free_t>(0x8BD80, 0x81924);

	s_memory_pool* object_table = Memory::GetAddress<s_memory_pool*>(0x4E4610);

	s_object_header* object_header = get_object_header(a1);
	object_header->flags = (e_object_header_flag)0;
	if (object_header->object != nullptr)
	{
		p_memory_pool_block_free(object_table, &object_header->object);
	}
	datum_delete(get_object_data_array(), a1);
}

typedef datum(__cdecl* p_object_new_t)(object_placement_data* placement_data);
p_object_new_t p_object_new;
datum __cdecl object_new(object_placement_data* placement_data)
{
	typedef void(__thiscall* sub_4F3B64_t)(byte* this_ptr);
	auto c_animation_manager__c_animation_manager = Memory::GetAddress<sub_4F3B64_t>(0xF3B64);

	typedef bool(__thiscall* sub_4F59AD_t)(byte* this_ptr, int a2, int a3, char a4);
	auto c_animation_manager__reset_graph = Memory::GetAddress<sub_4F59AD_t>(0xF59AD);

	typedef void(__thiscall* sub_4F3240_t)(byte* this_ptr);
	auto c_animation_manager__destructor_c_animation_manager = Memory::GetAddress<sub_4F3240_t>(0xf3240);

	typedef bool(__cdecl* object_header_block_allocate_t)(datum a1, __int16 a2, __int16 a3, char a4);
	auto p_object_header_block_allocate = Memory::GetAddress<object_header_block_allocate_t>(0x130BC6);

	typedef bool(__cdecl* havok_can_allocate_space_for_instance_of_object_definition_t)(datum a1);
	auto p_havok_can_allocate_space_for_instance_of_object_definition = Memory::GetAddress<havok_can_allocate_space_for_instance_of_object_definition_t>(0x9FE55);

	typedef void(__thiscall* sub_4F31E7_t)(byte* this_ptr);
	auto c_animation_manager__initialize = Memory::GetAddress<sub_4F31E7_t>(0xF31E7);

	typedef void(__cdecl* sub_532F07_t)(datum arg0, int arg4);
	auto update_object_region_information = Memory::GetAddress<sub_532F07_t>(0x132F07);

	typedef char(__cdecl* sub_5310F9_t)(datum a1, int a2, real_color_rgb* a3);
	auto p_object_set_initial_change_colors = Memory::GetAddress<sub_5310F9_t>(0x1310F9);

	typedef void(__cdecl* object_initialize_vitality_t)(datum a1, float* a2, float* a3);
	auto p_object_initialize_vitality = Memory::GetAddress<object_initialize_vitality_t>(0x175A62);

	//typedef char(__cdecl* object_compute_change_colors_t)(datum a1);
	//auto p_object_compute_change_colors = Memory::GetAddress<object_compute_change_colors_t>(0x13456E);

	typedef void(__cdecl* sub_52FE4D_t)(datum a1);
	auto p_object_reset_interpolation = Memory::GetAddress<sub_52FE4D_t>(0x12FE4D);

	typedef void(__cdecl* sub_5323B3_t)(datum a1);
	auto p_connect_objects_havok_component_to_world = Memory::GetAddress< sub_5323B3_t>(0x1323B3);

	typedef void(__cdecl* effect_new_from_object_t)(int arg0, s_damage_owner* arg4, datum a1, float a4, float a5, int a6);
	auto p_effect_new_from_object = Memory::GetAddress< effect_new_from_object_t>(0xAADCE);

	typedef int(__cdecl* sub_66CFDD_t)(datum a1);
	auto p_sub_66CFDD = Memory::GetAddress< sub_66CFDD_t>(0x26CFDD);

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

	if (FLAG(new_object_tag->object_type) & 
		(FLAG(e_object_type::creature) | 
			FLAG(e_object_type::crate) | 
			FLAG(e_object_type::machine) | 
			FLAG(e_object_type::vehicle) | 
			FLAG(e_object_type::biped)) != 0)
	{
		typedef void(__cdecl* havok_memory_garbage_collect_t)();
		havok_memory_garbage_collect_t p_havok_memory_garbage_collect = Memory::GetAddress<havok_memory_garbage_collect_t>(0xF7F78);
		p_havok_memory_garbage_collect();
	}

	datum object_datum = object_header_new(object_type_definition->datum_size);

	if (object_datum == DATUM_INDEX_NONE) { return DATUM_INDEX_NONE; }

	s_object_header* object_header = get_object_header(object_datum);
	s_object_data_definition* object = object_get_fast_unsafe<s_object_data_definition>(object_datum);

	object_header->flags |= e_object_header_flag::_object_header_being_deleted_bit;
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

	if (placement_data->object_placement_flags & 1)
	{
		object->object_flags |= object_data_flag_0x400;
	}
	else
	{
		object->object_flags &= ~object_data_flag_0x400;
	}

	if (model_definition && model_definition->collision_model.TagIndex != DATUM_INDEX_NONE)
	{
		object->object_flags |= object_data_flag_has_collision;
	}
	else
	{
		object->object_flags &= ~object_data_flag_has_collision;
	}

	// tag_indexes arent actually used, they're just passed into the function but aren't used
	if (datum tag_indexes[2]; object_render_model_has_prt_info(object_datum, &tag_indexes[0], &tag_indexes[1]) == true)
	{ 
		object->object_flags |= object_data_flag_has_prt_or_lighting_info; 
	}
	else { object->object_flags &= ~object_data_flag_has_prt_or_lighting_info; }

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
	if (new_object_tag->object_flags & s_object_group_definition::e_object_flags::does_not_cast_shadow) 
	{ 
		object->object_flags |= object_data_flag_object_does_not_cast_shadow; 
	}

	if (object->object_flags & object_data_flag_0x1)
	{
		object->object_flags &= ~object_data_flag_0x1;
		if (s_object_globals::object_is_connected_to_map(object_datum)) { s_object_globals::object_connect_lights_recursive(object_datum, 0, 1, 0, 0); }
		s_object_globals::object_update_collision_culling(object_datum);
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
	byte animation_manager[144];

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
			int test = 0;
			c_animation_manager__c_animation_manager(animation_manager);
			if (c_animation_manager__reset_graph(animation_manager, model_definition->animation.TagIndex, new_object_tag->model.TagIndex, 1))
			{
				valid_animation_manager = true;
				allow_interpolation = (FLAG(new_object_tag->object_type) &
					FLAG(e_object_type::sound_scenery) | 
					FLAG(e_object_type::light_fixture) |
					FLAG(e_object_type::control) |
					FLAG(e_object_type::machine) |
					FLAG(e_object_type::scenery) |
					FLAG(e_object_type::projectile)) != 0;

				// allow interpolation if object is device and device flags include interpolation
				if ((FLAG(new_object_tag->object_type) & e_object_type::light_fixture | e_object_type::control | e_object_type::machine) != 0
					&& (((const s_device_group_definition*)new_object_tag)->flags & s_device_group_definition::e_device_group_flag_allow_interpolation) != 0)
				{
					allow_interpolation = true;
				}
			}
			test = -1;
			c_animation_manager__destructor_c_animation_manager(animation_manager);
		}
	}

	long original_orientations = (!allow_interpolation ? 0 : nodes_count * 32);

	bool b_attachments = p_object_header_block_allocate(object_datum, 0x11C, (short)(8 * new_object_tag->attachments.size), 0);
	bool b_damage_sections = p_object_header_block_allocate(object_datum, 0x120, (short)(8 * damage_info_damage_sections_size), 0);
	bool b_change_colors = p_object_header_block_allocate(object_datum, 0x124, (short)(24 * new_object_tag->change_colors.size), 0);
	bool b_nodes = p_object_header_block_allocate(object_datum, 0x114, (short)(52 * nodes_count), 0);
	bool b_collision = p_object_header_block_allocate(object_datum, 0x118, (short)(10 * collision_regions_count), 0);
	bool b_interpolation_nodes = p_object_header_block_allocate(object_datum, 0x110, (short)original_orientations, 4);
	bool b_interpolation_nodes_2 = p_object_header_block_allocate(object_datum, 0x10C,(short)original_orientations, 4);
	bool b_animation = p_object_header_block_allocate(object_datum, 0x128, (valid_animation_manager ? 144 : 0), 0);
	bool b_havok = p_havok_can_allocate_space_for_instance_of_object_definition(placement_data->tag_index);

	bool unk_creation_bool = false;
	bool graph_reset = false;
	if (b_attachments && 
		b_damage_sections && 
		b_change_colors && 
		b_nodes && 
		b_collision && 
		b_interpolation_nodes && 
		b_interpolation_nodes_2 && 
		b_animation && 
		b_havok)
	{
		graph_reset = true;
	}
	else { unk_creation_bool = true; }

	if (graph_reset)
	{
		if (valid_animation_manager)
		{
			byte* c_animation_manager = ((byte*)object + object->animation_manager_offset);

			c_animation_manager__initialize(c_animation_manager);
			if (c_animation_manager__reset_graph(c_animation_manager, model_definition->animation.TagIndex, new_object_tag->model.TagIndex, 1))
				object->object_flags |= object_data_flag_0x800;
			else
				object->object_flags &= ~object_data_flag_0x800;
		}
		if (new_object_tag->attachments.size > 0)
		{
			byte* object_attachments_block = (byte*)object + object->object_attachments_block_offset;
			memset(object_attachments_block, -1, 8 * ((unsigned int)object->object_attachments_block_size >> 3));
		}

		if (object_type_new(object_datum, placement_data, &unk_creation_bool))
		{
			auto b_object_flag_check = (object->object_flags & 0x20000) != 0;
			if ((placement_data->object_placement_flags & 6) != 0)
			{
				object->object_flags &= ~object_data_flag_0x20000;
			}

			object_evaluate_placement_variant(object_datum, placement_data->variant_name);
			update_object_region_information(object_datum, placement_data->region_index);
			p_object_set_initial_change_colors(object_datum, placement_data->active_change_colors_mask, placement_data->change_colors);
			p_object_initialize_vitality(object_datum, nullptr, nullptr);
			object_compute_change_colors(object_datum);
			object->foreground_emblem = placement_data->foreground_emblem;

			if (object->animation_manager_offset != DATUM_INDEX_NONE)
				p_object_reset_interpolation(object_datum);

			object_compute_node_matrices_with_children(object_datum);
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
				object->physics_flags |= _object_physics_flag_0x2;
			else
				object->physics_flags &= ~_object_physics_flag_0x2;

			p_connect_objects_havok_component_to_world(object_datum);
			object_initialize_effects(object_datum);
			object_type_create_children(object_datum);

			if (new_object_tag->creation_effect.TagIndex != DATUM_INDEX_NONE)
				p_effect_new_from_object(new_object_tag->creation_effect.TagIndex, &placement_data->damage_owner, object_datum, 0.0f, 0.0f, 0);

			p_sub_66CFDD(object_datum);

			(b_object_flag_check ? object->object_flags |= object_data_flag_0x20000 : object->object_flags &= ~object_data_flag_0x20000);

			object_early_mover_new(object_datum);

			if ((object->object_flags & 0x20000) == 0 || (object_header->flags & _object_header_active_bit) != 0) { return object_datum; }

			if (!s_object_globals::objects_can_connect_to_map()) { return object_datum; }

			if ((placement_data->object_placement_flags & 2) == 0 && ((placement_data->object_placement_flags & 4) == 0 
				|| object->location.cluster != 0xFFFF)) 
			{ 
				object_delete(object_datum);
			}
			return object_datum;
		}
		object_type_delete(object_datum);
	}
	free_object_memory(DATUM_INDEX_TO_ABSOLUTE_INDEX(object_datum));
	return DATUM_INDEX_NONE;
}

#pragma region Biped variant patches
void update_biped_object_variant_data(datum object_idx, int variant_index)
{
	s_biped_data_definition* biped_object = (s_biped_data_definition*)object_try_and_get_and_verify_type(object_idx, FLAG(e_object_type::biped));
	// set this data only if we are dealing with a biped
	if (biped_object != NULL)
	{
		biped_object->variant_name = variant_index;
		// addDebugText("set the variant data to: %u for biped", variant_index);
	}
}

void __cdecl update_object_variant_index_hook(datum object_idx, int variant_index)
{
	auto p_resolve_variant_index_to_new_variant = Memory::GetAddressRelative<int(__cdecl*)(datum, int)>(0x52FE84, 0x51ED47);
	auto object = object_get_fast_unsafe<s_biped_data_definition>(object_idx);

	object->model_variant_id = p_resolve_variant_index_to_new_variant(object_idx, variant_index);
	// update the biped variant index
	update_biped_object_variant_data(object_idx, variant_index);

	//addDebugText("object_index: %d, model_variant_id: %u", DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index), object->variant_index);
}

// original update_object_variant_index is usercall, with data in CX register as first param
__declspec(naked) void update_object_variant_index_to_cdecl()
{
	__asm
	{
		// we don't preserve eax, because it's a volatile register thats expected to have 
		// it's value changed

		mov eax, [esp + 4] // copy the variant index
		push eax		   // then push to stack for update_object_variant_index_hook as second param
		push ecx		   // ecx stores the object index (in CX register size), push as first param
		call update_object_variant_index_hook
		add esp, 8			// reset stack after update_object_variant_index_hook
		retn				// return to procedure
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

void apply_biped_object_definition_patches()
{
	LOG_INFO_GAME("{} - applying hooks", __FUNCTION__);

	// increase the data size for biped representation
	WriteValue<unsigned short>(Memory::GetAddressRelative(0x81E9A8, 0x7C1EB8) + 8, sizeof(s_biped_data_definition));

	// hook the function that updates the variant
	WriteJmpTo(Memory::GetAddressRelative(0x52FED3, 0x51ED96), update_object_variant_index_to_cdecl);

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
	DETOUR_ATTACH(p_object_new, Memory::GetAddress<p_object_new_t>(0x136CA7), object_new);
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
#pragma endregion

