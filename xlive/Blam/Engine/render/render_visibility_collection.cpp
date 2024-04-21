#include "stdafx.h"
#include "render_visibility_collection.h"

void __cdecl render_visibility_predict_resources_for_pvs(int32 cluster_index)
{
	INVOKE(0x19460E, 0x1804B5, render_visibility_predict_resources_for_pvs, cluster_index);
	return;
}

void __cdecl predicted_resources_precache(int32 cluster_index)
{
	INVOKE(0x1946B9, 0x180560, predicted_resources_precache, cluster_index);
	return;
}

bool __cdecl render_visibility_check_location_cluster_active(s_location* location)
{
	return INVOKE(0x19447C, 0, render_visibility_check_location_cluster_active, location);
}

void render_scene_visibility_to_usercall(int32 user_index)
{
	void* render_scene_visibility = (void*)Memory::GetAddress(0x190755);
	__asm {
		mov edi, user_index
		call render_scene_visibility
	}
	return;
}