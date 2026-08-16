#include "stdafx.h"
#include "scenario.h"

#include "scenario_definitions.h"

#include "cache/cache_files.h"
#include "creatures/creature_definitions.h"
#include "game/game_engine.h"
#include "game/game_globals.h"
#include "physics/bsp3d.h"
#include "physics/collision_model_definitions.h"
#include "physics/physics_model_definitions.h"
#include "rasterizer/dx9/rasterizer_dx9_vertex_shaders.h"
#include "structures/structure_bsp_definitions.h"
#include "units/biped_definitions.h"
#include "units/vehicle_definitions.h"

/* prototypes */

static void __cdecl scenario_tags_postprocess(void);

static void __cdecl scenario_apply_level_patches(void);

/* public code */

void scenario_apply_patches(
	void)
{
	PatchCall(Memory::GetAddress(0x28516), scenario_tags_postprocess);	// Default Maps
	PatchCall(Memory::GetAddress(0x284DE), scenario_tags_postprocess);	// Custom Maps
	
	return;
}

scenario* global_scenario_get(void) 
{
	scenario* global_scenario = *Memory::GetAddress<scenario**>(0x479E74, 0x4A6430);
	
	ASSERT(global_scenario);

	return global_scenario;
}

scenario* global_scenario_try_and_get(void)
{
	scenario* global_scenario = *Memory::GetAddress<scenario**>(0x479E74, 0x4A6430);

	return global_scenario;
}

void set_global_scenario(
	scenario* _scenario)
{
	*Memory::GetAddress<scenario**>(0x479E74, 0x4A6430) = _scenario;
}

bsp3d* global_bsp3d_get(void)
{
	return *Memory::GetAddress<bsp3d**>(0x479E60, 0x4A641C);
}

collision_bsp* global_collision_bsp_get(void)
{
	return *Memory::GetAddress<collision_bsp**>(0x479E64, 0x4A6420);
}

void global_scenario_index_set(
	datum scenario_index)
{
	datum* global_scenario_index = Memory::GetAddress<datum*>(0x4119A0, 0x3B528C);
	*global_scenario_index = scenario_index;
	return;
}

int32 global_scenario_index_get(void)
{
	return *Memory::GetAddress<int32*>(0x4119A0, 0x3B528C);
}

int32 scenario_netgame_equipment_size(void)
{
	return global_scenario_get()->netgame_equipment.count;
}

void location_invalidate(
	s_location* object_location)
{
	object_location->leaf_index = NONE;
	object_location->cluster_index = NONE;
	object_location->bsp_index = get_global_structure_bsp_index();

	return;
}

int32 scenario_leaf_index_from_point(
	real_point3d const* point)
{
	bsp3d* global_bsp3d = global_bsp3d_get();

	ASSERT(global_bsp3d);

	return bsp3d_test_point(global_bsp3d, 0, point);
}

void __cdecl scenario_location_from_point(
	s_location* location,
	real_point3d* point)
{
	INVOKE(0x281EE, 0x30CB1, scenario_location_from_point, location, point);
	return;
}

bool __cdecl scenario_location_underwater(
	s_location* location,
	real_point3d* point,
	int16* global_material_index)
{
	return INVOKE(0x27A03, 0x304C6, scenario_location_underwater, location, point, global_material_index);
}

void __cdecl scenario_location_from_leaf(
	s_location* location,
	int32 leaf_index)
{
	INVOKE(0x2819D, 0x30C60, scenario_location_from_leaf, location, leaf_index);
	return;
}


bool __cdecl scenario_switch_bsp(
	int16 bsp_index)
{
	return INVOKE(0x27CA6, 0x0, scenario_switch_bsp, bsp_index);
}

void scenario_language_pack_unload(void)
{
	s_game_globals* game_globals = scenario_get_game_globals();

	game_globals->language_pack[get_current_language()].unload_data();
	
	return;
}

/* private code */

static void __cdecl scenario_tags_postprocess(void)
{
	tag_iterator itr;
	tag_iterator_new(&itr, _tag_group_none);
	
	for (datum i = tag_iterator_next(&itr); i != NONE; i = tag_iterator_next(&itr))
	{
		tag_group group = tag_instance_get(i)->group_tag;
		switch (group)
		{
		case _tag_group_biped:
			biped_definitions_fixup(i);
			break;
		case _tag_group_creature:
			creature_definitions_fixup(i);
			break;
		case _tag_group_vehicle:
			vehicle_definitions_fixup(i);
			break;
		case _tag_group_collision_model:
			collision_model_definitions_fixup(i);
			break;
		case _tag_group_physics_model:
			physics_model_definitions_fixup(i, false);
			break;
		case _tag_group_vertex_shader:
			//rasterizer_dx9_vertex_shaders_replace_map_bytecode(i);
			break;
		default:
			break;
		}

	}
	scenario_apply_level_patches();
	game_engine_apply_map_patches();

	return;
}

static void __cdecl scenario_apply_level_patches(void)
{
	return INVOKE(0x27EDD, 0x0, scenario_apply_level_patches);
}
