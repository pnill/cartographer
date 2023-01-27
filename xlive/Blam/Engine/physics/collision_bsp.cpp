#include "stdafx.h"
#include "collision_bsp.h"

bool __cdecl collision_bsp_test_sphere(s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp,
	__int16 flags, 
	void* breakable_surface_set, 
	real_point3d* point,
	float scale, 
	collision_bsp_test_sphere_result* test_result)
{
	// I will reverse you later my sweet prince
	typedef void(__cdecl* test_sphere_results_t)(test_sphere_result_buffer* buffer, int starting_index);
	auto p_test_sphere_results = Memory::GetAddress<test_sphere_results_t>(0xE9890);


	test_sphere_result_buffer buffer; // [esp+0h] [ebp-228h] BYREF

	buffer.flags = flags;
	buffer.collision_bsp = collision_bsp;
	buffer.breakable_surface_set = breakable_surface_set;
	buffer.point = point;
	buffer.scale = scale;
	buffer.test_sphere_result = test_result;
	buffer.unk0 = 0;
	buffer.unk_bool = 0;
	*(DWORD*)&test_result->gap[3080] = 0;
	test_result->unk0 = 0;
	*(DWORD*)&test_result->gap[1024] = 0;
	*(DWORD*)&test_result->gap[2052] = 0;
	p_test_sphere_results(&buffer, 0);
	return test_result > 0 || *(int*)&test_result->gap[1024] > 0 || *(int*)&test_result->gap[2052] > 0;
}