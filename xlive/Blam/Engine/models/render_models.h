#pragma once

/* constants */

enum
{
	MAXIMUM_REGIONS_PER_MODEL = 16,
	MAXIMUM_PERMUTATIONS_PER_MODEL_REGION = 32,
	MAXIMUM_NODES_PER_MODEL = 255,
	MAXIMUM_SECTIONS_PER_RENDER_MODEL = 255,
	NUMBER_OF_GEOMETRY_DETAIL_LEVELS = 6,
	MAXIMUM_NODES_PER_RENDER_MODEL_SECTION = 40,
	MAXIMUM_MARKER_GROUPS_PER_RENDER_MODEL = 4096,
	MAXIMUM_MARKERS_PER_RENDER_MODEL_MARKER_GROUP = 256,
	MAXIMUM_RENDER_MODELS_PER_OBJECT = 5,
};

/* prototypes */

int32 __cdecl render_model_find_marker_group_by_name(datum render_model_index, string_id name);

void __cdecl render_model_apply_two_bone_ik(
	datum render_model_index, 
	int32 node_index,
	real_matrix4x3* matrix_1,
	real_matrix4x3* matrix_2,
	real32 ratio,
	int32 node_matrices_count,
	real_matrix4x3* out);

int32 __cdecl render_model_get_markers_by_name(
	datum render_model_index,
	string_id name,
	const int8* region_permutation_indices,
	int32 node_remap_count,
	const int32* node_remapping_table,
	int32 node_count,
	const struct real_matrix4x3* node_matrices,
	bool mirrored_flag,
	struct object_marker* markers,
	int32 maximum_marker_count);