#include "stdafx.h"
#include "render_models.h"

/* public code */

int32 __cdecl render_model_find_marker_group_by_name(datum render_model_index, string_id name)
{
	return INVOKE(0x37A358, 0x3250C8, render_model_find_marker_group_by_name, render_model_index, name);
}

void __cdecl render_model_apply_two_bone_ik(
	datum render_model_index,
	int32 node_index,
	real_matrix4x3* bone_1,
	real_matrix4x3* bone_2,
	real32 ratio,
	int32 node_matrices_count,
	real_matrix4x3* out)
{
	INVOKE(0x37A561, 0x3252D1, render_model_apply_two_bone_ik, render_model_index, node_index, bone_1, bone_2, ratio, node_matrices_count, out);
	return;
}

int32 __cdecl render_model_get_markers_by_name(
	datum render_model_index,
	string_id marker_string,
	void* collision_region_block,
	int32 a4,
	int32* node_remapping_table,
	int32 node_count,
	real_matrix4x3* node_matrices,
	bool object_is_mirrored,
	object_marker* markers,
	int32 count)
{
	return INVOKE(0x37A525, 0x325295, render_model_get_markers_by_name, render_model_index,
		marker_string,
		collision_region_block,
		a4,
		node_remapping_table,
		node_count,
		node_matrices,
		object_is_mirrored,
		markers,
		count);
}
