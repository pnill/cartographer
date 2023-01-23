#include "stdafx.h"

#include "KantTesting.h"

#include "Blam/Engine/Objects/ObjectTypes.h"
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_lightmap_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_structure_bsp_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Engine\Game\GameEngineGlobals.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Players\Players.h"
#include "Blam\LazyBlam\LazyBlam.hpp"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "Blam\Engine\Memory\bitstream.h"
#include "Blam\Engine\Memory\bitstream.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "Util\Hooks\Hook.h"


namespace KantTesting
{
	void MapLoad()
	{
		if (h2mod->GetEngineType() == _multiplayer)
		{
		}
	}
	c_object_type_definition** object_types;

	typedef bool(__cdecl* object_type_definition_object_new_evaluate_t)(datum object_datum, int a2, int a3);
	object_type_definition_object_new_evaluate_t p_object_type_definition_object_new_evaluate;

	bool __cdecl object_type_definition_object_new_evaluate(datum object_datum, int a2, int a3)
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

	typedef void(__cdecl* object_type_definitions_adjust_placement_evaluate_t)(datum object_datum);
	object_type_definitions_adjust_placement_evaluate_t p_object_type_definitions_adjust_placement_evaluate;

	void __cdecl object_type_definitions_adjust_placement_evaluate(datum object_datum)
	{
		auto object_type = c_object_type_definition::get_game_object_type_definition(object_datum);
		for (byte i = 0; i < 3; i++)
		{
			auto object_base_type = object_type->base_object_types[i];
			if (object_base_type)
				if (object_base_type->adjust_placement)
					object_base_type->adjust_placement(object_datum);
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
			DETOUR_ATTACH(p_object_type_definition_object_new_evaluate, Memory::GetAddress<object_type_definition_object_new_evaluate_t>(0x185BAE, 0x0), object_type_definition_object_new_evaluate);
			DETOUR_ATTACH(p_object_type_object_delete_evaluate, Memory::GetAddress<object_type_object_delete_evaluate_t>(0x185C30, 0x0), object_type_object_delete_evaluate);
			DETOUR_ATTACH(p_object_type_definitions_adjust_placement_evaluate, Memory::GetAddress<object_type_definitions_adjust_placement_evaluate_t>(0x185ADC, 0x0), object_type_definitions_adjust_placement_evaluate);
			DETOUR_ATTACH(p_object_type_definitions_disconnect_from_structure_bsp_evalulate, Memory::GetAddress<object_type_definitions_disconnect_from_structure_bsp_evalulate_t>(0x185B45, 0x0), object_type_definitions_disconnect_from_structure_bsp_evalulate);
			DETOUR_ATTACH(p_object_type_unknown38, Memory::GetAddress<object_type_unknown38_t>(0x185C9D, 0x0), object_type_unknown38_evaluate);
			DETOUR_ATTACH(p_object_assign_new_entity, Memory::GetAddress<object_type_assign_new_entity_t>(0x185F0A, 0x0), object_type_assign_new_entity_evaluate);
			DETOUR_ATTACH(p_object_type_detach_gamestate_entity, Memory::GetAddress<object_type_detach_gamestate_entity_t>(0x185F73, 0x0), object_type_detach_gamestate_entity_evaluate);
			DETOUR_ATTACH(p_object_type_unknown7C, Memory::GetAddress<object_type_unknown7C_t>(0x1862E3, 0x0), object_type_unknown7C_evaluate);
			DETOUR_ATTACH(p_object_type_object_move, Memory::GetAddress<object_type_object_move_t>(0x185DD9, 0x0), object_type_object_move_evaluate);
			DETOUR_ATTACH(p_object_type_object_can_activate, Memory::GetAddress<object_type_object_can_activate_t>(0x185E2C, 0x0), object_type_object_can_activate_evaluate);
			DETOUR_COMMIT();
		//	if (!Memory::isDedicatedServer())
			//{
			//tags::on_map_load(MapLoad);
		//	}
		}
	}
}
