#pragma once
#include "objects/object_placement.h"

/* enums */

enum e_collection_type : uint32
{
	_collection_type_0 = 0,
	_collection_type_1 = 1,
	_collection_type_2 = 2,
	_collection_type_3 = 3,
	k_number_collection_types
};

/* public code */

void __cdecl render_visibility_predict_resources_for_pvs(int32 cluster_index);

void __cdecl predicted_resources_precache(int32 cluster_index);

bool __cdecl render_visibility_check_location_cluster_active(s_location* location);

void render_view_visibility_compute_to_usercall(int32 user_index);
