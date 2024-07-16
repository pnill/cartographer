#pragma once
#include "collision_bsp_definition.h"

#define MAXIMUM_EDGES_PER_COLLISION_BSP 262144
#define MAXIMUM_SURFACES_PER_COLLISION_BSP 131072

// max count: ((((MAXIMUM_SURFACES_PER_COLLISION_BSP)+(LONG_BITS-1))>>LONG_BITS_BITS)) = 4096
struct long_surface_flags
{
	int32 flags;
};

struct collision_bsp_test_vector_result
{
	real32 field_0;
	real_plane3d* plane;
	int32 field_8;
	int32 field_C;
	int32 flags;
	int32 field_14;
};
ASSERT_STRUCT_SIZE(collision_bsp_test_vector_result, 24);

struct collision_bsp_test_buffer
{
	int32 leaf_index;
	int32 unk[255];
};
ASSERT_STRUCT_SIZE(collision_bsp_test_buffer, 1024);

struct collision_bsp_test_sphere_result
{
	int32 count_buffer0;
	collision_bsp_test_buffer buffer0;
	int32 count_buffer1;
	collision_bsp_test_buffer buffer1;
	int32 count_buffer2;
	collision_bsp_test_buffer buffer2;
	int32 stack_depth;
	collision_bsp_test_buffer buffer3;
};
ASSERT_STRUCT_SIZE(collision_bsp_test_sphere_result, 4112);

struct test_sphere_result_buffer
{
	collision_bsp* collision_bsp;
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
ASSERT_STRUCT_SIZE(test_sphere_result_buffer, 552);

bool __cdecl collision_bsp_test_sphere(
	collision_bsp* collision_bsp,
	int16 flags,
	void* breakable_surface_set,
	real_point3d* point,
	real32 scale,
	collision_bsp_test_sphere_result* test_result);

