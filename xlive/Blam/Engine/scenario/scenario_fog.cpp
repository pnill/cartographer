#include "stdafx.h"
#include "scenario_fog.h"

bool __cdecl render_scenario_fog(int cluster_index, s_camera* camera_position, real_vector3d* camera_forward, bool a4, bool render_fog, s_scenario_fog_result* result)
{
    return INVOKE(0xBA3EA, 0x0, render_scenario_fog, cluster_index, camera_position, camera_forward, a4, render_fog, result);
}
