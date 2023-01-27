#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"
#include "Blam/Math/real_math.h"

#pragma pack(push ,1)
struct collision_bsp_test_sphere_result
{
	WORD unk0;
	byte gap[4110];
};
CHECK_STRUCT_SIZE(collision_bsp_test_sphere_result, 4112);
#pragma pack(pop)

struct test_sphere_result_buffer
{
	s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp;
	short flags;
	short pad;
	void* breakable_surface_set;
	real_point3d* point;
	float scale;
	collision_bsp_test_sphere_result* test_sphere_result;
	DWORD unk0;
	byte gap1[515];
	byte unk_bool;
	byte gap2[8];
};
CHECK_STRUCT_SIZE(test_sphere_result_buffer, 552);

bool __cdecl collision_bsp_test_sphere(s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp,
	__int16 flags,
	void* breakable_surface_set,
	real_point3d* point,
	float scale,
	collision_bsp_test_sphere_result* test_result);