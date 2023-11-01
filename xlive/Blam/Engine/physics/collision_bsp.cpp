#include "stdafx.h"
#include "collision_bsp.h"

void __cdecl test_sphere_results(test_sphere_result_buffer* buffer, int32 starting_index)
{
	INVOKE(0xE918C, 0xD7707, test_sphere_results, buffer, starting_index);
	return;
}

bool __cdecl collision_bsp_test_sphere(
	s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp,
	int16 flags,
	void* breakable_surface_set,
	real_point3d* point,
	real32 scale,
	collision_bsp_test_sphere_result* test_result)
{
	test_sphere_result_buffer buffer;
	buffer.flags = flags;
	buffer.collision_bsp = collision_bsp;
	buffer.breakable_surface_set = breakable_surface_set;
	buffer.point = point;
	buffer.scale = scale;
	buffer.test_sphere_result = test_result;
	buffer.unk0 = 0;
	buffer.field_21F = false;
	test_result->stack_depth = 0;
	test_result->count_buffer0 = 0;
	test_result->count_buffer1 = 0;
	test_result->count_buffer2 = 0;
	test_sphere_results(&buffer, 0);
	return test_result->count_buffer0 > 0 || test_result->count_buffer1 > 0 || test_result->count_buffer2 > 0;
}
