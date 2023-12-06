#include "stdafx.h"
#include "render_models.h"

int32 __cdecl render_model_find_marker_group_by_name(datum render_model_index, string_id name)
{
	return INVOKE(0x37A358, 0x3250C8, render_model_find_marker_group_by_name, render_model_index, name);
}

void __cdecl render_model_apply_two_bone_ik(datum render_model_index,
	int32 node_index,
	real_matrix4x3* matrix_1,
	real_matrix4x3* matrix_2,
	real32 ratio,
	int32 node_matrices_count,
	real_matrix4x3* out)
{
	INVOKE(0x37A561, 0x3252D1, render_model_apply_two_bone_ik, render_model_index, node_index, matrix_1, matrix_2, ratio, node_matrices_count, out);
	return;
}