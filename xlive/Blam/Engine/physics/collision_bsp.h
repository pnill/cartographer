#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"
#include "Blam/Math/real_math.h"

struct collision_bsp_test_sphere_result
{
	int count_buffer0;
	DWORD buffer0[256];
	int count_buffer1;
	DWORD buffer1[256];
	int count_buffer2;
	DWORD buffer2[256];
	int stack_depth;
	DWORD buffer3[256];
};
CHECK_STRUCT_SIZE(collision_bsp_test_sphere_result, 4112);

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
	DWORD gap[128];
	WORD pad0;
	bool unk21E;
	bool unk21F;
	real_point2d plane;
};
CHECK_STRUCT_SIZE(test_sphere_result_buffer, 552);

bool __cdecl collision_bsp_test_sphere(
	s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp,
	__int16 flags,
	void* breakable_surface_set,
	real_point3d* point,
	float scale,
	collision_bsp_test_sphere_result* test_result);