#pragma once
#include "camera/camera.h"
#include "math/color_math.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

#define MAXIMUM_ATMOSPHERIC_FOG_PALETTE_ENTRIES_PER_SCENARIO 127
#define k_maximum_mixers_per_scenario_atmospheric_fog_palette_entry 2
#define MAXIMUM_PLANAR_FOG_PALETTE_ENTRIES_PER_SCENARIO 127

enum e_camera_immersion_flags : short
{
    camera_immersion_flag_disable_atmospheric_fog = FLAG(0),
    camera_immersion_flag_disable_secondary_fog = FLAG(1),
    camera_immersion_flag_disable_planar_fog = FLAG(2),
    camera_immersion_flag_invert_planar_fog_priorities = FLAG(3),
    camera_immersion_flag_disable_water = FLAG(4)
};

struct s_scenario_fog_result
{
    int32 field_0;
    real_rgb_color color;
    bool unk_bool_10;
    bool unk_bool_11;
    int8 pad_12[2];
    real32 z_far;
    bool unk_bool_18;
    int8 pad[3];
    int8 gap_18[260];
};
ASSERT_STRUCT_SIZE(s_scenario_fog_result, 288);

// max count: k_maximum_mixers_per_scenario_atmospheric_fog_palette_entry 2
struct scenario_atmospheric_fog_mixer_block
{
    int pad;
    string_id atmospheric_fog_source_from_scenario_atmospheric_fog_palette;
    string_id interpolator_from_scenario_interpolators;
    short pad1[2];
};
ASSERT_STRUCT_SIZE(scenario_atmospheric_fog_mixer_block, 16);

// max count: MAXIMUM_ATMOSPHERIC_FOG_PALETTE_ENTRIES_PER_SCENARIO 127
struct s_scenario_atmospheric_fog_palette_entry
{
    string_id name;

    // Explaination("ATMOSPHERIC FOG", "EMPTY STRING")
    real_rgb_color color;
    float spread_distance_world_units;  // How far fog spreads into adjacent clusters: 0 defaults to 1.
    int pad;
    float maximum_density;              // Fog density clamps to this value.
    float start_distance_world_units;   // Before this distance, there is no fog.
    float opaque_distance_world_units;  // Fog becomes opaque (maximum density) at this distance from viewer.

    // Explaination("SECONDARY FOG", "EMPTY STRING")
    real_rgb_color secondary_fog_color;
    int pad1;
    float secondary_fog_maximum_density;                // Fog density clamps to this value.
    float secondary_fog_start_distance_world_units;     // Before this distance, there is no fog.
    float secondary_fog_opaque_distance_world_units;    // Fog becomes opaque (maximum density) at this distance from viewer.
    int pad2;

    // Explaination("PLANAR FOG OVERRIDE", "Planar fog, if present, is interpolated toward these values.")
    real_rgb_color planar_color;
    float planar_max_density;
    float planar_override_amount;
    float planar_min_distance_bias_world_units;     // Don't ask.
    int pad3[11];

    // Explaination("PATCHY FOG", "EMPTY STRING")
    real_rgb_color patchyColor;
    int pad4[3];
    real_vector2d patchy_density;
    real_vector2d patchy_distance_world_units;
    int pad5[8];
    tag_reference patchy_fog;           // fpch
    tag_block<scenario_atmospheric_fog_mixer_block> mixers;

    // Explaination("BLOOM OVERRIDE", "EMPTY STRING")
    float amount;
    float threshold;
    float brightness;
    float gamma_power;

    // Explaination("CAMERA IMMERSION OVERRIDE", "EMPTY STRING")
    e_camera_immersion_flags camera_immersion_flags;
    short pad6;
};
ASSERT_STRUCT_SIZE(s_scenario_atmospheric_fog_palette_entry, 244);

// max count: MAXIMUM_PLANAR_FOG_PALETTE_ENTRIES_PER_SCENARIO 127
struct s_scenario_planar_fog_palette_entry
{
    string_id name;
    tag_reference planar_fog;
    short pad[2];
};
ASSERT_STRUCT_SIZE(s_scenario_planar_fog_palette_entry, 16);

// CLIENT ONLY
// Renders the fog defined in the scenario
bool __cdecl render_scenario_fog(int cluster_index, s_camera* camera_position, real_vector3d* camera_forward, bool a4, bool render_fog, s_scenario_fog_result* result);
