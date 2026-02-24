#pragma once
#include "tag_files/tag_reference.h"

#define k_maximum_cluster_sound_palette_entries_per_structure 64
#define k_maximum_machine_door_portal_associations 128

enum e_background_sound_scale_flags : int
{
	background_sound_scale_flag_override_default_scale = FLAG(0),
	background_sound_scale_flag_use_adjacent_cluster_as_portal_scale = FLAG(1),
	background_sound_scale_flag_use_adjacent_cluster_as_exterior_scale = FLAG(2),
	background_sound_scale_flag_scale_with_weather_intensity = FLAG(3),
};

// max count: k_maximum_cluster_sound_palette_entries_per_structure 64
struct structure_background_sound_palette_entry
{
	char name[k_tag_string_length];
	tag_reference background_sound;         // lsnd

	// Play only when player is inside cluster.
	// lsnd
	tag_reference inside_cluster_sound;     
	int pad[5];

	float cutoff_distance;
	e_background_sound_scale_flags scale_flags;
	float interior_scale;
	float portal_scale;
	float exterior_scale;
	float interpolation_speed;          // 1 sec
	int pad1[2];
};
ASSERT_STRUCT_SIZE(structure_background_sound_palette_entry, 100);

// max count: k_maximum_cluster_sound_palette_entries_per_structure 64
struct structure_sound_environment_palette_entry
{
	char name[k_tag_string_length];
	tag_reference sound_environment;    // snde
	float cutoff_distance;
	float interpolation_speed;          // 1 sec
	int pad[6];
};
ASSERT_STRUCT_SIZE(structure_sound_environment_palette_entry, 72);


struct s_structure_audibility
{
	int32 door_portal_count;
	real_bounds cluster_distance_bounds;

	// max: k_maximum_machine_door_portal_associations*2*((((MAXIMUM_CLUSTERS_PER_STRUCTURE)+(LONG_BITS-1))>>LONG_BITS_BITS))
	// 4096
	s_tag_block encoded_door_pas;	// struct: int32

	// max: MAXIMUM_CLUSTERS_PER_STRUCTURE*((((k_maximum_machine_door_portal_associations)+(LONG_BITS-1))>>LONG_BITS_BITS))
	// 2048
	s_tag_block cluster_door_portal_encoded_pas;	// struct: int32

	// max: (((((MAXIMUM_CLUSTERS_PER_STRUCTURE*(MAXIMUM_CLUSTERS_PER_STRUCTURE-1))/2)+(LONG_BITS-1))>>LONG_BITS_BITS))
	// 4088
	s_tag_block ai_deafening_pas;	// struct: int32

	// max: (MAXIMUM_CLUSTERS_PER_STRUCTURE*(MAXIMUM_CLUSTERS_PER_STRUCTURE-1))/2
	// 130816
	s_tag_block cluster_distances;	// struct: int8

	// max: k_maximum_machine_door_portal_associations
	s_tag_block machine_door_mapping;	// struct: int8
};
ASSERT_STRUCT_SIZE(s_structure_audibility, 52);