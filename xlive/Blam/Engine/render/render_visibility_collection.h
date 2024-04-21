#pragma once
#include "objects/object_placement.h"

void __cdecl render_visibility_predict_resources_for_pvs(int32 cluster_index);

void __cdecl predicted_resources_precache(int32 cluster_index);

bool __cdecl render_visibility_check_location_cluster_active(s_location* location);

void render_scene_visibility_to_usercall(int32 user_index);
