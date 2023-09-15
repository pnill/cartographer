#pragma once

#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Engine/cseries/cseries_strings.h"

#define k_maximum_cluster_sound_palette_entries_per_structure 64
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
    static_string32 name;
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
TAG_BLOCK_SIZE_ASSERT(structure_background_sound_palette_entry, 100);

// max count: k_maximum_cluster_sound_palette_entries_per_structure 64
struct structure_sound_environment_palette_entry
{
    static_string32 name;
    tag_reference sound_environment;    // snde
    float cutoff_distance;
    float interpolation_speed;          // 1 sec
    int pad[6];
};
TAG_BLOCK_SIZE_ASSERT(structure_sound_environment_palette_entry, 72);
