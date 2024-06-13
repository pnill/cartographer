#pragma once
#include "math/color_math.h"
#include "math/function_definitions.h"
#include "math/periodic_functions.h"

#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

/* constants */
#define k_maximum_reflections_per_lens_flare 32

/* enums */

enum e_occlusion_offset_direction : int16
{
    _occlusion_offset_direction_toward_viewer = 0,
    _occlusion_offset_direction_marker_forward = 1,
    _occlusion_offset_direction_none = 2
};

enum e_occlusion_inner_radius_scale : int16
{
    _occlusion_inner_radius_scale_none = 0,
    _occlusion_inner_radius_scale_12 = 1,
    _occlusion_inner_radius_scale_14 = 2,
    _occlusion_inner_radius_scale_18 = 3,
    _occlusion_inner_radius_scale_116 = 4,
    _occlusion_inner_radius_scale_132 = 5,
    _occlusion_inner_radius_scale_164 = 6
};

enum e_lens_flare_flags : int16
{
    _lens_flare_flag_sun = FLAG(0),
    _lens_flare_flag_no_occlusion_test = FLAG(1),
    _lens_flare_flag_only_render_in_first_person = FLAG(2),
    _lens_flare_flag_only_render_in_third_person = FLAG(3),
    _lens_flare_flag_fade_in_more_quickly = FLAG(4),
    _lens_flare_flag_fade_out_more_quickly = FLAG(5),
    _lens_flare_flag_scale_by_marker = FLAG(6)
};

enum e_rotation_function : int16
{
    _rotation_function_none = 0,
    _rotation_function_rotation_a = 1,
    _rotation_function_rotation_b = 2,
    _rotation_function_rotation_translation = 3,
    _rotation_function_translation = 4
};

enum e_lens_falloff_function : int16
{
    _lens_falloff_function_linear = 0,
    _lens_falloff_function_late = 1,
    _lens_falloff_function_very_late = 2,
    _lens_falloff_function_early = 3,
    _lens_falloff_function_very_early = 4,
    _lens_falloff_function_cosine = 5,
    _lens_falloff_function_zero = 6,
    _lens_falloff_function_one = 7
};

enum e_lens_flare_animation_flags : int16
{
    _lens_flare_animation_flag_synchronized = FLAG(0)
};

enum e_lens_flare_reflection_flags : int16
{
    _lens_flare_reflection_flag_align_rotation_with_screen_center = FLAG(0),
    _lens_flare_reflection_flag_radius_not_scaled_by_distance = FLAG(1),
    _lens_flare_reflection_flag_radius_scaled_by_occlusion_factor = FLAG(2),
    _lens_flare_reflection_flag_occluded_by_solid_objects = FLAG(3),
    _lens_flare_reflection_flag_ignore_light_color = FLAG(4),
    _lens_flare_reflection_flag_not_affected_by_inner_occlusion = FLAG(5)
};

/* structures */

struct s_lens_flare_reflection
{
    e_lens_flare_reflection_flags flags;
    int16 pad;
    int16 bitmap_index;
    int16 pad1;
    
    real32 position_along_flare_axis;
    real_angle rotation_offset;     // 0 is on top of light, 1 is opposite light, 0.5 is the center of the screen, etc.
    real_vector2d radius;           // interpolated by external input
    real_vector2d brightness;       // interpolated by external input

    // Explaination("TINT COLOR", "Tinting and modulating are not the same; 'tinting' a reflection will color the darker regions but leave the highlights, while 'modulating' will color everything uniformly. The modulation factor controls how much the reflection is modulated as opposed to tinted (0 is tinted, 1 is modulated).")
    
    real32 modulation_factor;
    real_rgb_color color;
};
ASSERT_STRUCT_SIZE(s_lens_flare_reflection, 48)

struct s_lens_flare_definition
{
	// Explaination("LENS FLARE", "")

    real_angle falloff_angle;;
    real_angle cutoff_angle;
    real32 cos_falloff_angle;
    real32 cos_cutoff_angle;

    // Explaination("OCCLUSION", "Occlusion factor affects overall lens flare brightness and can also affect scale. Occlusion never affects rotation.")
    
    real32 occlusion_radius;    // radius of the square used to test occlusion
    e_occlusion_offset_direction occlusion_offset_direction;
    e_occlusion_inner_radius_scale occlusion_inner_radius_scale;
    
    real32 near_fade_distance;  // distance at which the lens flare brightness is maximum

    // distance at which the lens flare brightness is minimum; set to zero to disable distance fading
    real32 far_fade_distance;

    tag_reference bitmap;   // bitm
    e_lens_flare_flags flags;
    int16 pad;

    e_rotation_function rotation_function;
    int16 pad1;
    real_angle rotation_function_scale;
    real_vector2d corona_scale;         // amount to stretch the corona

    // Explaination("EFFECT PARAMETERS", "Only affects lens flares created by effects.")

    e_lens_falloff_function falloff;
    int16 pad2;
    tag_block<s_lens_flare_reflection> reflections;
    
    // Explaination("ANIMATION", "")

    e_lens_flare_animation_flags animation_flags;
    int16 pad3;

    tag_block<c_function_definition> brightness;
    tag_block<c_function_definition> color;
    tag_block<c_function_definition> rotation;  
};
ASSERT_STRUCT_SIZE(s_lens_flare_definition, 100);
