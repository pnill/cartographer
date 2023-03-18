#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"
#include "Blam/Engine/math/real_math.h"

struct collision_bsp_test_buffer
{
	DWORD leaf_index;
	DWORD unk[255];
};

struct collision_bsp_test_sphere_result
{
	int count_buffer0;
	collision_bsp_test_buffer buffer0;
	int count_buffer1;
	collision_bsp_test_buffer buffer1;
	int count_buffer2;
	collision_bsp_test_buffer buffer2;
	int stack_depth;
	collision_bsp_test_buffer buffer3;
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