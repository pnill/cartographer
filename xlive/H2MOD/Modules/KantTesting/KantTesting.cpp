#include "stdafx.h"

#include "KantTesting.h"

#include "Blam/Cache/TagGroups/device_definition.hpp"
#include "Blam/Engine/Objects/ObjectGlobals.h"
#include "Blam/Engine/Objects/ObjectTypes.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Blam/Engine/physics/collision_bsp.h"
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/globals_definition.hpp"
#include "Blam/Cache/TagGroups/model_definition.hpp"
#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/Cache/TagGroups/scenario_lightmap_definition.hpp"
#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"
#include "Blam/Engine/Game/GameEngineGlobals.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "Blam/Engine/Players/Players.h"
#include "Blam/LazyBlam/LazyBlam.hpp"
#include "H2MOD/Engine/Engine.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "Blam/Engine/Memory/bitstream.h"
#include "Blam/Engine/Memory/bitstream.h"
#include "H2MOD/Modules/Shell/Shell.h"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "Util/Hooks/Hook.h"


namespace KantTesting
{
	void MapLoad()
	{
		if (h2mod->GetEngineType() == _multiplayer)
		{
		}
	}
	c_object_type_definition** object_types;

	typedef bool(__cdecl* object_type_new_t)(datum object_datum, s_object_placement_data* a2, bool* a3);
	object_type_new_t p_object_type_definition_object_new_evaluate;

	bool __cdecl object_type_new(datum object_datum, s_object_placement_data* a2, bool* a3)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		bool result = true;
		auto header = get_objects_header(object_datum);
		for (byte i = 0; i < 3; i++)
		{
			auto object_base_type = object_type->base_object_types[i];
			if (object_base_type)
				if (object_base_type->object_new)
				{
					result = result && object_base_type->object_new(object_datum, a2, a3);
					if(header->flags & e_object_header_flag::object_header_flags_10 != 0)
						LOG_INFO_GAME("{}", header->flags);
				}
		}
		return result;
	}

	typedef void(__cdecl* object_type_definitions_adjust_placement_evaluate_t)(s_object_placement_data* placement_data);
	object_type_definitions_adjust_placement_evaluate_t p_object_type_definitions_adjust_placement_evaluate;

	void __cdecl object_type_definitions_adjust_placement_evaluate(s_object_placement_data* placement_data)
	{
		auto object_type = c_object_type_definition::get_tag_type_definition(placement_data->tag_index);
		for (byte i = 0; i < 3; i++)
		{
			auto object_base_type = object_type->base_object_types[i];
			if (object_base_type)
				if (object_base_type->adjust_placement)
					object_base_type->adjust_placement(placement_data);
		}
	}

	typedef void(__cdecl* object_type_definitions_disconnect_from_structure_bsp_evalulate_t)(datum object_datum);
	object_type_definitions_disconnect_from_structure_bsp_evalulate_t p_object_type_definitions_disconnect_from_structure_bsp_evalulate;

	void __cdecl object_type_definitions_disconnect_from_structure_bsp_evalulate(datum object_datum)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (byte i = 0; i < 3; i++)
		{
			auto object_base_type = object_type->base_object_types[i];
			if (object_base_type)
				if (object_base_type->disconnect_from_structure_bsp)
					object_base_type->disconnect_from_structure_bsp(object_datum);
		}
	}

	typedef void(__cdecl* object_type_object_delete_evaluate_t)(datum object_datum, int a2);
	object_type_object_delete_evaluate_t p_object_type_object_delete_evaluate;

	void __cdecl object_type_object_delete_evaluate(datum object_datum, int a2)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->object_delete)
					object_base_type->object_delete(object_datum, a2);
		}
	}

	typedef void(__cdecl* object_type_unknown38_t)(datum object_datum);
	object_type_unknown38_t p_object_type_unknown38;

	void __cdecl object_type_unknown38_evaluate(datum object_datum)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->unknown38)
					object_base_type->unknown38(object_datum);
		}
	}

	typedef void(__cdecl* object_type_unknown7C_t)(datum object_datum, int a2, int a3);
	object_type_unknown7C_t p_object_type_unknown7C;

	void __cdecl object_type_unknown7C_evaluate(datum object_datum, int a2, int a3)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->unknown7C)
					object_base_type->unknown7C(object_datum, a2, a3);
		}
	}

	typedef void(__cdecl* object_type_unknown3C_t)(datum object_datum);
	object_type_unknown3C_t p_object_type_unknown3C;

	void __cdecl object_type_unknown3C_evaluate(datum object_datum)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->unknown3C)
					object_base_type->unknown3C(object_datum);
		}
	}

	typedef void(__cdecl* object_type_object_move_t)(datum object_datum);
	object_type_object_move_t p_object_type_object_move;

	void __cdecl object_type_object_move_evaluate(datum object_datum)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->object_move)
					object_base_type->object_move(object_datum);
		}
	}

	typedef void(__cdecl* object_type_object_can_activate_t)(datum object_datum, int a2, int a3);
	object_type_object_can_activate_t p_object_type_object_can_activate;

	void __cdecl object_type_object_can_activate_evaluate(datum object_datum, int a2, int a3)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->object_can_activate)
					object_base_type->object_can_activate(object_datum, a2, a3);
		}
	}

	typedef void(__cdecl* object_type_assign_new_entity_t)(datum object_datum);
	object_type_assign_new_entity_t p_object_assign_new_entity;

	void __cdecl object_type_assign_new_entity_evaluate(datum object_datum)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->assign_new_entity)
					object_base_type->assign_new_entity(object_datum);
		}
	}

	typedef void(__cdecl* object_type_detach_gamestate_entity_t)(datum object_datum);
	object_type_detach_gamestate_entity_t p_object_type_detach_gamestate_entity;

	void __cdecl object_type_detach_gamestate_entity_evaluate(datum object_datum)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->detach_entity)
					object_base_type->detach_entity(object_datum);
		}
	}

	typedef void(__cdecl* object_type_process_node_matrices_t)(datum object_datum, int node_count, int node_matracies);
	object_type_process_node_matrices_t p_object_type_process_node_matrices;

	void __cdecl object_type_process_node_matrices(datum object_datum, int node_count, int node_matracies)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (auto object_base_type : object_type->base_object_types)
		{
			if (object_base_type)
				if (object_base_type->detach_entity)
					object_base_type->process_node_matricies(object_datum, node_count, node_matracies);
		}
	}
	
	template<class T> inline T operator~ (T a) { return (T)~(int)a; }
	template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
	template<class T> inline bool operator& (T a, T b) { return ((int)a & (int)b != 0); }
	template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
	template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
	template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
	template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

	void object_evaluate_placement_variant(datum object_header_datum, string_id variant_index)
	{
		typedef byte(__cdecl* sub_52FE84_t)(unsigned __int16 a1, string_id a2);
		auto p_sub_53FE84 = Memory::GetAddress<sub_52FE84_t>(0x12FE84);
		auto object = (s_object_data_definition*)get_objects_header(object_header_datum)->object;

		object->model_variant_id = p_sub_53FE84(DATUM_INDEX_TO_ABSOLUTE_INDEX(object_header_datum), variant_index);
	}

	void object_postprocess_node_matrices(datum object_datum)
	{
		auto object = object_get_fast_unsafe(object_datum);
		auto object_tag = tags::get_tag_fast<s_object_group_definition>(object->tag_definition_index);
		if(object_tag->model.TagIndex != DATUM_INDEX_NONE)
		{
			auto model_tag = tags::get_tag_fast<s_model_group_definition>(object_tag->model.TagIndex);
			if(model_tag->render_model.TagIndex != DATUM_INDEX_NONE && model_tag->render_model.TagIndex != DATUM_INDEX_NONE)
			{
				typedef int(__cdecl* object_get_node_matrices_t)(unsigned __int16 a1, int* a2);
				auto p_object_get_node_matrices = Memory::GetAddress<object_get_node_matrices_t>(0x12FCA5);
				int node_count = 0;
				int node_matricies = p_object_get_node_matrices(DATUM_INDEX_TO_ABSOLUTE_INDEX(object_datum), &node_count);
				object_type_process_node_matrices(object_datum, node_count, node_matricies);
			}
		}
	}

	void object_initialize_effects(datum object_datum)
	{
		typedef void(__cdecl* widgets_new_t)(datum a1);
		auto p_widgets_new = Memory::GetAddress< widgets_new_t>(0x14FFE2);

		typedef void(__cdecl* attachments_new_t)(datum a1);
		auto p_attachments_new = Memory::GetAddress< attachments_new_t>(0x1348CA);

		auto object_header = get_objects_header(object_datum);
		auto object = (s_object_data_definition*)object_header->object;
		if (object_header->type == projectile)
			*((DWORD*)object_header->object + 55) = -1; ///??????
		else
			p_widgets_new(object_datum);

		p_attachments_new(object_datum);
	}

	bool set_object_position_if_in_cluster(s_location* location, long object_datum)
	{
		s_object_data_definition* object = object_get_fast_unsafe(object_datum);
		scenario_location_from_point(location, &object->object_origin_point);

		if (location->cluster != 0xFFFF)
			return true;

		collision_bsp_test_sphere_result test_result;
		auto g_collision_bsp = Memory::GetAddress<s_scenario_structure_bsp_group_definition::s_collision_bsp_block*>(0x479E64);
		collision_bsp_test_sphere(g_collision_bsp, 0, 0, &object->object_origin_point, object->shadow_sphere_radius, &test_result);

		if (*(DWORD*)&test_result.gap[3080])
			scenario_location_from_leaf(location, *(DWORD*)&test_result.gap[3084]);
		else
			scenario_location_from_point(location, &object->position);

		return location->cluster != 0xFFFF;
	}

	typedef datum(__cdecl* object_new_t)(s_object_placement_data* placement_data);
	object_new_t p_object_new;

	datum __cdecl object_new(s_object_placement_data* placement_data)
	{

		typedef datum(__cdecl* datum_header_new_t)(__int16 object_size);
		auto p_datum_header_new = Memory::GetAddress<datum_header_new_t>(0x130AF6);

		typedef __int16(__cdecl* structure_bsp_index_t)();
		auto p_structure_bsp_index = Memory::GetAddress<structure_bsp_index_t>(0x277C9);

		typedef bool(__cdecl* sub_531B0E_t)(unsigned __int16 a1, size_t* a2, size_t* a3);
		auto p_sub_531B0E = Memory::GetAddress<sub_531B0E_t>(0x131B0E);

		typedef void(__cdecl* sub_4276B9_t)(DWORD* a1);
		auto p_sub_4276B9 = Memory::GetAddress<sub_4276B9_t>(0x276B9);

		typedef void(__thiscall* sub_4F3B64_t)(char* this_ptr);
		auto p_sub_4F3B64 = Memory::GetAddress<sub_4F3B64_t>(0xF3B64);

		typedef bool(__thiscall* sub_4F59AD_t)(char* this_ptr, int a2, int a3, char a4);
		auto p_sub_4F59AD = Memory::GetAddress<sub_4F59AD_t>(0xF59AD);

		typedef void(__thiscall* sub_4F3240_t)(char* this_ptr);
		auto p_sub_4F3240 = Memory::GetAddress<sub_4F3240_t>(0xf3240);

		typedef bool(__cdecl* object_header_block_allocate_t)(unsigned __int16 a1, __int16 a2, __int16 a3, char a4);
		auto p_object_header_block_allocate = Memory::GetAddress<object_header_block_allocate_t>(0x130BC6);

		typedef bool(__cdecl* havok_can_allocate_space_for_instance_of_object_definition_t)(unsigned __int16 a1);
		auto p_havok_can_allocate_space_for_instance_of_object_definition = Memory::GetAddress<havok_can_allocate_space_for_instance_of_object_definition_t>(0x9FE55);

		typedef void(__thiscall* sub_4F31E7_t)(char* this_ptr);
		auto p_sub_4f31E7 = Memory::GetAddress<sub_4F31E7_t>(0xF31E7);

		typedef void(__cdecl* sub_532F07_t)(unsigned __int16 arg0, int arg4);
		auto p_sub_532F07 = Memory::GetAddress<sub_532F07_t>(0x132F07);

		typedef char(__cdecl* sub_5310F9_t)(unsigned __int16 a1, int a2, real_color_rgb * a3);
		auto p_sub_5310F9 = Memory::GetAddress<sub_5310F9_t>(0x1310F9);

		typedef void(__cdecl* object_initialize_vitality_t)(datum a1, float* a2, float* a3);
		auto p_object_initialize_vitality = Memory::GetAddress<object_initialize_vitality_t>(0x175A62);
		
		typedef char(__cdecl* object_compute_change_colors_t)(datum a1);
		auto p_object_compute_change_colors = Memory::GetAddress<object_compute_change_colors_t>(0x13456E);

		typedef void(__cdecl* sub_52FE4D_t)(datum a1);
		auto p_sub_52FE4D = Memory::GetAddress<sub_52FE4D_t>(0x12FE4D);

		typedef void(__cdecl* sub_5323B3_t)(unsigned __int16 a1);
		auto p_sub_5323B3 = Memory::GetAddress< sub_5323B3_t>(0x1323B3);

		typedef void(__cdecl* object_compute_node_matrices_with_children_t)(datum a1);
		auto p_object_compute_node_matrices_with_children = Memory::GetAddress< object_compute_node_matrices_with_children_t>(0x136924);

		typedef void(__cdecl* object_reconnect_to_map_t)(const void* location_struct, const datum object_index);
		auto p_object_reconnect_to_map = Memory::GetAddress<object_reconnect_to_map_t>(0x1360CE, 0x124F9E);

		typedef void(__cdecl* effect_new_from_object_t)(int arg0, int* arg4, datum a1, float a4, float a5, int a6);
		auto p_effect_new_from_object = Memory::GetAddress< effect_new_from_object_t>(0xAADCE);

		typedef int(__cdecl* sub_66CFDD_t)(datum a1);
		auto p_sub_66CFDD = Memory::GetAddress< sub_66CFDD_t>(0x26CFDD);

		typedef void(__cdecl* sub_54BA87_t)(datum a1);
		auto p_sub_54BA87 = Memory::GetAddress< sub_54BA87_t>(0x14BA87);

		typedef void(__cdecl* object_delete_t)(datum a1);
		auto p_object_delete_t = Memory::GetAddress< object_delete_t>(0x136005);

		auto unk_placement_flag = (placement_data->object_placement_flags & 0x10) == 0;
		auto unk_creation_bool = false;
		if (unk_placement_flag)
		{
			if (placement_data->tag_index == DATUM_INDEX_NONE)
				return DATUM_INDEX_NONE;
			//object_type_definitions_adjust_placement_evaluate(placement_data);
		}
		if (placement_data->tag_index == DATUM_INDEX_NONE)
			return DATUM_INDEX_NONE;

		auto new_object_tag = tags::get_tag_fast<s_object_group_definition>(placement_data->tag_index);
		auto new_object_definition = c_object_type_definition::get_object_type_definitions()[(byte)new_object_tag->object_type];

		s_model_group_definition* new_object_model_tag;
		if (new_object_tag->model.TagIndex != DATUM_INDEX_NONE)
			new_object_model_tag = tags::get_tag_fast<s_model_group_definition>(new_object_tag->model.TagIndex);

		if (((1 << new_object_tag->object_type) & (_object_is_creature | _object_is_crate | _object_is_machine | _object_is_vehicle | _object_is_biped)) != 0)
		{
			typedef void(__cdecl* havok_memory_garbage_collect_t)();
			havok_memory_garbage_collect_t p_havok_memory_garbage_collect = Memory::GetAddress<havok_memory_garbage_collect_t>(0xF7F78);
			p_havok_memory_garbage_collect();
		}

		auto new_object_header_datum = p_datum_header_new(new_object_definition->datum_size);

		if (new_object_header_datum == DATUM_INDEX_NONE)
			return DATUM_INDEX_NONE;

		auto new_object_header = get_objects_header(new_object_header_datum);
		auto new_object = object_get_fast_unsafe<s_object_data_definition>(new_object_header_datum);

		new_object_header->flags |= _object_header_being_deleted_bit;
		new_object_header->type = new_object_tag->object_type;
		new_object->tag_definition_index = placement_data->tag_index;

		auto unk_placement_check = placement_data->unk_12 == 0xFF;
		if (unk_placement_check)
		{
			++s_object_globals::get()->unique_id;
			new_object->field_AB = 2;
			new_object->origin_bsp_index = -1;
			new_object->unique_id = s_object_globals::get()->unique_id;
			new_object->placement_index = -1;
			new_object->structure_bsp_index = p_structure_bsp_index();
		}
		else
		{
			new_object->unique_id = placement_data->unique_id;
			new_object->origin_bsp_index = placement_data->origin_bsp_index;
			new_object->placement_index = placement_data->unk_10;
			new_object->structure_bsp_index = placement_data->origin_bsp_index;
		}

		new_object->position = placement_data->position;
		new_object->orientation = placement_data->orientation;
		new_object->up = placement_data->up;
		new_object->translational_velocity = placement_data->translational_velocity;
		new_object->angular_velocity = placement_data->angular_velocity;
		new_object->scale = placement_data->scale;

		if (placement_data->object_placement_flags & 1 != 0)
			new_object->object_flags |= 0x400u;
		else
			new_object->object_flags &= ~0x400u;

		if (new_object_model_tag && new_object_model_tag->collision_model.TagIndex != DATUM_INDEX_NONE)
			new_object->object_flags |= 0x200u;
		else
			new_object->object_flags &= ~0x200u;

		size_t out_1;
		size_t out_2;

		if (p_sub_531B0E(new_object_header_datum, &out_1, &out_2))
			new_object->object_flags |= 0x80000000;
		else
			new_object->object_flags &= ~0x80000000;

		new_object_header->unk__ = -1;

		//This is some sort of internal struct but i can't be bothered to map it out right now

		p_sub_4276B9(new_object->location);

		new_object->field_60 = -1;
		new_object->parent_datum = DATUM_INDEX_NONE;
		new_object->next_index = DATUM_INDEX_NONE;
		new_object->current_weapon_datum = DATUM_INDEX_NONE;
		new_object->name_list_index = DATUM_INDEX_NONE;
		new_object->netgame_equipment_index = -1;
		new_object->byte_108 = -1;
		new_object->byte_109 = -1;
		new_object->placement_policy = placement_data->placement_policy;
		if (new_object_tag->object_flags & s_object_group_definition::e_object_flags::does_not_cast_shadow)
			new_object->object_flags |= 0x10000u;

		if (new_object->object_flags & 1 != 0)
		{
			new_object->object_flags &= ~1u;
			if (s_object_globals::object_is_connected_to_map(new_object_header_datum))
				s_object_globals::object_connect_lights_recursive(new_object_header_datum, 0, 1, 0, 0);
			s_object_globals::object_update_collision_culling(new_object_header_datum);
		}
		new_object->field_C2 = placement_data->field_70;
		new_object->field_C4 = placement_data->field_68;
		new_object->field_C8 = placement_data->field_6C;
		new_object->model_variant_id = -1;
		new_object->field_CC = -1;
		new_object->field_D0 = -1;
		new_object->field_C0 = 0;

		if (placement_data->unk_12 & 8 != 0)
			new_object->field_C0 |= 0x100u;
		else
			new_object->field_C0 &= ~0x100u;

		new_object->havok_datum = -1;
		new_object->simulation_entity_index = -1;
		new_object->field_D8 = 0;
		new_object->destroyed_constraints_flag = placement_data->destroyed_constraints_flag;
		new_object->loosened_constraints_flag = placement_data->loosened_constraints_flag;

		size_t nodes_count = 1;
		size_t collision_regions_count = 1;
		size_t damage_info_damage_sections_size = 0;
		char unk_animation_structure[145];

		bool allow_interpolation = false;
		bool valid_animation_maybe = false;

		if (new_object_model_tag)
		{
			if (new_object_model_tag->nodes.size >= 1)
				nodes_count = new_object_model_tag->nodes.size;
			if (new_object_model_tag->collision_regions.size >= 1)
				collision_regions_count = new_object_model_tag->collision_regions.size;

			if (new_object_model_tag->new_damage_info.size > 0 && new_object_model_tag->new_damage_info.data != -1)
				damage_info_damage_sections_size = new_object_model_tag->new_damage_info[0]->damage_sections.size;

			if (new_object_model_tag->animation.TagIndex != DATUM_INDEX_NONE)
			{

				unk_animation_structure[144] = 0;
				p_sub_4F3B64(unk_animation_structure);
				if (p_sub_4F59AD(unk_animation_structure, new_object_model_tag->animation.TagIndex, new_object_tag->model.TagIndex, 1))
				{
					valid_animation_maybe = true;
					allow_interpolation = ((1 << new_object_tag->object_type) & (_object_is_sound_scenery | _object_is_light_fixture | _object_is_control | _object_is_machine | _object_is_scenery | _object_is_projectile)) == 0;
					if (((1 << new_object_tag->object_type) & (_object_is_light_fixture | _object_is_control | _object_is_machine)) != 0)
					{
						if (((s_device_group_definition*)new_object_tag)->flags & s_device_group_definition::e_flags::allow_interpolation)
							allow_interpolation = true;
					}
				}
				unk_animation_structure[144] = -1;
				p_sub_4F3240(unk_animation_structure);
			}
		}

		auto new_object_absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(new_object_header_datum);

		long intperolation_node_size = (!allow_interpolation) ? 0 : nodes_count * 32;

		bool b_attachments = p_object_header_block_allocate(new_object_absolute_index, 284, 8 * new_object_tag->attachments.size, 0);
		bool b_damage_sections = p_object_header_block_allocate(new_object_absolute_index, 0x120, 8 * damage_info_damage_sections_size, 0);
		bool b_change_colors = p_object_header_block_allocate(new_object_absolute_index, 0x124, 24 * new_object_tag->change_colors.size, 0);
		bool b_nodes = p_object_header_block_allocate(new_object_absolute_index, 0x114, 52 * nodes_count, 0);
		bool b_collision = p_object_header_block_allocate(new_object_absolute_index, 0x118, 10 * collision_regions_count, 0);
		bool b_interpolation_nodes = p_object_header_block_allocate(new_object_absolute_index, 0x110, intperolation_node_size, 0);
		bool b_interpolation_nodes_2 = p_object_header_block_allocate(new_object_absolute_index, 0x10C, intperolation_node_size, 0);
		bool b_animation = p_object_header_block_allocate(new_object_absolute_index, 0x128, ((valid_animation_maybe) ? 0x90 : 0), 1);
		bool b_havok = p_havok_can_allocate_space_for_instance_of_object_definition(DATUM_INDEX_TO_ABSOLUTE_INDEX(placement_data->tag_index));

		auto b_object_creation_valid = false;
		if (b_attachments && b_damage_sections && b_change_colors && b_nodes && b_collision && b_interpolation_nodes && b_interpolation_nodes_2 && b_animation && b_havok)
			b_object_creation_valid = true;
		else
			unk_creation_bool = true;

		if (b_object_creation_valid)
		{
			if (valid_animation_maybe)
			{
				auto object_animations_block = ((char*)new_object + new_object->object_animations_block_offset);

				p_sub_4f31E7(object_animations_block);
				if (p_sub_4F59AD(object_animations_block, new_object_model_tag->animation.TagIndex, new_object_tag->model.TagIndex, 1))
					new_object->object_flags |= 0x800u;
				else
					new_object->object_flags &= ~0x800u;
			}
			if (new_object_tag->attachments.size > 0)
			{
				auto object_attachments_block = (char*)new_object + new_object->object_attachments_block_offset;
				_Shell::csmemset(object_attachments_block, -1, 8 * ((unsigned int)(__int16)new_object->field_11C >> 3));
			}
			auto b_object_type_new = object_type_new(new_object_header_datum, placement_data, &unk_creation_bool);
			if (b_object_type_new)
			{
				auto b_object_flag_check = (new_object->object_flags & 0x20000) != 0;
				if (placement_data->object_placement_flags & 6 != 0)
					new_object->object_flags &= ~0x20000u;

				object_evaluate_placement_variant(new_object_header_datum, placement_data->variant_name);
				p_sub_532F07(new_object_absolute_index, placement_data->unk_AC);
				p_sub_5310F9(new_object_absolute_index, placement_data->active_change_colors_mask, placement_data->change_colors);
				p_object_initialize_vitality(new_object_header_datum, 0, 0);
				p_object_compute_change_colors(new_object_header_datum);
				new_object->field_24 = placement_data->unk_A8;

				if (new_object->object_animations_block_offset != 0xFFFF)
					p_sub_52FE4D(new_object_header_datum);

				p_object_compute_node_matrices_with_children(new_object_header_datum);
				if (s_object_globals::get() && s_object_globals::get()->initialized)
				{
					byte unk_byte = 0;
					s_location* p_location = nullptr;
					if (placement_data->object_is_inside_cluster
						|| (unk_byte = set_object_position_if_in_cluster(&placement_data->location, new_object_header_datum),
							(placement_data->object_is_inside_cluster = unk_byte) != 0))
					{
						p_location = &placement_data->location;
					}

					object_reconnect_to_map(p_location, new_object_header_datum);
				}

				object_postprocess_node_matrices(new_object_header_datum);
				s_object_globals::object_wake(new_object_header_datum);

				if ((placement_data->unk_12 & 0x20) != 0)
					new_object->field_C0 |= 2u;
				else
					new_object->field_C0 &= ~2u;

				p_sub_5323B3(new_object_absolute_index);
				object_initialize_effects(new_object_header_datum);
				object_type_unknown38_evaluate(new_object_header_datum);

				if (new_object_tag->creation_effect.TagIndex != DATUM_INDEX_NONE)
					p_effect_new_from_object(new_object_tag->creation_effect.TagIndex, (int*)&placement_data->field_68, new_object_header_datum, 0, 0, 0);

				p_sub_66CFDD(new_object_header_datum);

				if (b_object_flag_check)
					new_object->object_flags |= 0x20000u;
				else
					new_object->object_flags &= ~0x20000u;

				p_sub_54BA87(new_object_header_datum);

				if ((new_object->object_flags & 0x20000) == 0 || (new_object_header->flags & _object_header_active_bit) != 0)
				{
					return new_object_header_datum;
				}
				if (!s_object_globals::get()->initialized)
					return new_object_header_datum;
				auto unk_12 = placement_data->unk_12;
				if ((unk_12 & 2) == 0 && ((unk_12 & 4) == 0 || new_object->location[1] != 0xFFFF))
					p_object_delete_t(new_object_header_datum);
				return new_object_header_datum;
			}
			object_type_unknown3C_evaluate(new_object_header_datum);
		}
		return -1;
	}










	void Initialize()
	{
		if (ENABLEKANTTEST) {
			object_types = Memory::GetAddress<c_object_type_definition**>(0x41F560, 0x0);
			for (int i = 0; i < 13; i++)
			{
				auto c = *object_types[i];
				auto a = std::string(c.name);
				LOG_ERROR_FUNC("{}", a);
			}

			DETOUR_BEGIN();
			//DETOUR_ATTACH(p_object_type_definition_object_new_evaluate, Memory::GetAddress<object_type_definition_object_new_evaluate_t>(0x185BAE, 0x0), object_type_definition_object_new_evaluate);
			//DETOUR_ATTACH(p_object_type_object_delete_evaluate, Memory::GetAddress<object_type_object_delete_evaluate_t>(0x185C30, 0x0), object_type_object_delete_evaluate);
			//DETOUR_ATTACH(p_object_type_definitions_adjust_placement_evaluate, Memory::GetAddress<object_type_definitions_adjust_placement_evaluate_t>(0x185ADC, 0x0), object_type_definitions_adjust_placement_evaluate);
			//DETOUR_ATTACH(p_object_type_definitions_disconnect_from_structure_bsp_evalulate, Memory::GetAddress<object_type_definitions_disconnect_from_structure_bsp_evalulate_t>(0x185B45, 0x0), object_type_definitions_disconnect_from_structure_bsp_evalulate);
			//DETOUR_ATTACH(p_object_type_unknown38, Memory::GetAddress<object_type_unknown38_t>(0x185C9D, 0x0), object_type_unknown38_evaluate);
			//DETOUR_ATTACH(p_object_assign_new_entity, Memory::GetAddress<object_type_assign_new_entity_t>(0x185F0A, 0x0), object_type_assign_new_entity_evaluate);
			//DETOUR_ATTACH(p_object_type_detach_gamestate_entity, Memory::GetAddress<object_type_detach_gamestate_entity_t>(0x185F73, 0x0), object_type_detach_gamestate_entity_evaluate);
			//DETOUR_ATTACH(p_object_type_unknown7C, Memory::GetAddress<object_type_unknown7C_t>(0x1862E3, 0x0), object_type_unknown7C_evaluate);
			//DETOUR_ATTACH(p_object_type_unknown3C, Memory::GetAddress<object_type_unknown3C_t>(0x185D06, 0x0), object_type_unknown3C_evaluate);
			//DETOUR_ATTACH(p_object_type_object_move, Memory::GetAddress<object_type_object_move_t>(0x185DD9, 0x0), object_type_object_move_evaluate);
			//DETOUR_ATTACH(p_object_type_object_can_activate, Memory::GetAddress<object_type_object_can_activate_t>(0x185E2C, 0x0), object_type_object_can_activate_evaluate);
			//DETOUR_ATTACH(p_object_type_process_node_matrices, Memory::GetAddress<object_type_process_node_matrices_t>(0x1B6208, 0x0), object_type_process_node_matrices);

			DETOUR_ATTACH(p_object_new, Memory::GetAddress<object_new_t>(0x136CA7), object_new);
			DETOUR_COMMIT();
		//	if (!Memory::isDedicatedServer())
			//{
			//tags::on_map_load(MapLoad);
		//	}
		}
	}
}
