#include "stdafx.h"

#include "KantTesting.h"

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

			//DETOUR_ATTACH(p_object_new, Memory::GetAddress<object_new_t>(0x136CA7), object_new);
			DETOUR_COMMIT();
		//	if (!Memory::isDedicatedServer())
			//{
			//tags::on_map_load(MapLoad);
		//	}
		}
	}
}
