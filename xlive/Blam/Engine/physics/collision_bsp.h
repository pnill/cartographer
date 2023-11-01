#pragma once
#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"
#include "Blam/Math/real_math.h"
#include "H2MOD/Tags/TagInterface.h"

#define MAXIMUM_EDGES_PER_COLLISION_BSP 262144
#define MAXIMUM_SURFACES_PER_COLLISION_BSP 131072

// max count: ((((MAXIMUM_SURFACES_PER_COLLISION_BSP)+(LONG_BITS-1))>>LONG_BITS_BITS)) = 4096
struct long_surface_flags
{
	int32 flags;
};

struct collision_bsp_test_buffer
{
	int32 leaf_index;
	int32 unk[255];
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
	int16 flags;
	int16 pad;
	void* breakable_surface_set;
	real_point3d* point;
	real32 scale;
	collision_bsp_test_sphere_result* test_sphere_result;
	int32 unk0;
	int32 gap[128];
	int16 pad0;
	bool unk21E;
	bool field_21F;
	real_point2d plane;
};
CHECK_STRUCT_SIZE(test_sphere_result_buffer, 552);

bool __cdecl collision_bsp_test_sphere(
	s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp,
	int16 flags,
	void* breakable_surface_set,
	real_point3d* point,
	real32 scale,
	collision_bsp_test_sphere_result* test_result);
