#include "stdafx.h"
#include "scenario_fog.h"

/* public code */

bool __cdecl render_scenario_fog(
	int32 cluster_index,
	render_camera* camera_position,
	real_vector3d* camera_forward,
	bool a4,
	bool render_fog,
	s_scenario_fog_result* result)
{
	return INVOKE(0xBA3EA, 0x0, render_scenario_fog, cluster_index, camera_position, camera_forward, a4, render_fog, result);
}

void __cdecl scenario_fog_bloom_override_get_parameters(real32* bloom_override_amount, real32* bloom_override_gamma_power)
{
	ASSERT(bloom_override_amount);
	ASSERT(bloom_override_gamma_power);

	INVOKE(0xB7336, 0x0, scenario_fog_bloom_override_get_parameters, bloom_override_amount, bloom_override_gamma_power);
	return;
}
