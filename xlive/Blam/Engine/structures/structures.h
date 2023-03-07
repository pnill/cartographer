#pragma once
#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"

struct s_game_cluster_bit_vectors
{
	int cluster_bitvector;
};

__int16 get_global_structure_bsp_index();
s_scenario_structure_bsp_group_definition* get_global_structure_bsp();

__int16 structure_clusters_in_bitvector(const s_game_cluster_bit_vectors* cluster_bitvector, short maximum_clusters, short* intersected_indices, short* out_actual_cluster_count);
