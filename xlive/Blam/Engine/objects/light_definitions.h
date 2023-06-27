#pragma once

enum e_light_type : short
{
    light_type_sphere = 0,
    light_type_orthogonal = 1,
    light_type_projective = 2,
    light_type_pyramid = 3
};

enum e_light_placement_flags : short
{
    light_placement_flag_custom_geometry = FLAG(0),
    light_placement_flag_unused = FLAG(1),
    light_placement_flag_cinematicOnly = FLAG(2)
};

enum e_lightmap_type : short
{
    lightmap_type_use_light_tag_setting = 0,
    lightmap_type_dynamic_only = 1,
    lightmap_type_dynamic_with_lightmaps = 2,
    lightmap_type_lightmaps_only = 3
};

enum e_lightmap_flags: short
{
    lightmap_placement_flag_unused = FLAG(0),
};