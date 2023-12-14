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

