#pragma once
#include "shader_definitions.h"

#include "math/color_math.h"
#include "memory/static_arrays.h"
#include "tag_files/data_reference.h"
#include "tag_files/tag_reference.h"
#include "tag_files/tag_block.h"

/* constants */

#define k_maximum_implementations_per_shader_pass 32
#define k_maximum_textures_per_shader_pass_implementation 8
#define k_number_of_shader_texture_state_constants 10
#define k_maximum_vertex_shader_constants_per_shader_pass_implementation 32

/* enums */

enum e_shader_pass_flags : uint16
{
    _shader_pass_no_bitmap_lod_bit = 0,
    _shader_pass_required_parameter_bit = 1,
    k_shader_pass_flag_count
};

enum e_source_extern : int16
{
    _source_extern_none = 0,
    
    _source_extern_global_eye_forward_vector_z = 1,
    _source_extern_global_eye_right_vector_x = 2,
    _source_extern_global_eye_up_vector_y = 3,

    _source_extern_object_primary_color = 4,
    _source_extern_object_secondary_color = 5,
    _source_extern_object_function_value = 6,

    _source_extern_light_diffuse_color = 7,
    _source_extern_light_specular_color = 8,
    _source_extern_light_forward_vector_z = 9,
    _source_extern_light_right_vector_x = 10,
    _source_extern_light_up_vector_y = 11,
    _source_extern_light_object_relative_forward_vector_z = 12,
    _source_extern_light_object_relative_right_vector_x = 13,
    _source_extern_light_object_relative_up_vector_y = 14,
    _source_extern_light_object_falloff_value = 15,
    _source_extern_light_object_gel_color = 16,

    _source_extern_lightmap_object_ambient_factor = 17,
    _source_extern_lightmap_object_direct_vector = 18,
    _source_extern_lightmap_object_direct_color = 19,
    _source_extern_lightmap_object_indirect_vector = 20,
    _source_extern_lightmap_object_indirect_color = 21,

    _source_extern_oldfog_atmospheric_color = 22,
    _source_extern_oldfog_atmospheric_max_density = 23,
    _source_extern_oldfog_planar_color = 24,
    _source_extern_oldfog_planar_max_density = 25,
    _source_extern_oldfog_atmospheric_planar_blend_value = 26,
    _source_extern_oldfog_object_atmospheric_density = 27,
    _source_extern_oldfog_object_planar_density = 28,
    _source_extern_oldfog_object_color = 29,
    _source_extern_oldfog_object_density = 30,

    _source_extern_object_model_alpha = 31,
    _source_extern_object_shadow_alpha = 32,

    _source_extern_light_overbrighten_diffuse_shift = 33,
    _source_extern_light_overbrighten_specular_shift = 34,
    _source_extern_light_diffuse_contrast = 35,
    _source_extern_light_specular_gel = 36,

    _source_extern_shader_specular_type = 37,
    _source_extern_pad3 = 38,
    _source_extern_pad3_scale = 39,
    _source_extern_pad_thai = 40,
    _source_extern_taco_salad = 41,
    _source_extern_anisotropic_binormal = 42,
    
    _source_extern_object_light_shadow_fade = 43,
    _source_extern_light_shadow_fade = 44,

    _source_extern_oldfog_atmospheric_density = 45,
    _source_extern_oldfog_planar_density = 46,
    _source_extern_oldfog_planar_density_invert = 47,
    
    _source_extern_object_change_color_tertiary = 48,
    _source_extern_object_change_color_quaternary = 49,
    
    _source_extern_lightmap_object_specular_color = 50,
    _source_extern_shader_lightmap_type = 51,
    _source_extern_lightmap_object_ambient_color = 52,
    _source_extern_shader_lightmap_specular_brightness = 53,
    _source_extern_global_lightmap_shadow_darkening = 54,
    _source_extern_lightmap_object_env_brightness = 55,
    
    _source_extern_fog_atmospheric_max_density = 56,
    _source_extern_fog_atmospheric_color = 57,
    _source_extern_fog_atmospheric_color_adj = 58,
    _source_extern_fog_atmospheric_planar_blend = 59,
    _source_extern_fog_atmospheric_planar_blend_adj_inv = 60,
    _source_extern_fog_atmospheric_planar_blend_adj = 61,
    _source_extern_fog_secondary_max_density = 62,
    _source_extern_fog_secondary_color = 63,
    _source_extern_fog_secondary_color_adj = 64,
    _source_extern_fog_atmospheric_secondary_blend = 65,
    _source_extern_fog_atmospheric_secondary_blend_adj_inv = 66,
    _source_extern_fog_atmospheric_secondary_blend_adj = 67,
    _source_extern_fog_sky_density = 68,
    _source_extern_fog_sky_color = 69,
    _source_extern_fog_sky_color_adj = 70,
    _source_extern_fog_planar_max_density = 71,
    _source_extern_fog_planar_color = 72,
    _source_extern_fog_planar_color_adj = 73,
    _source_extern_fog_planar_eye_density = 74,
    _source_extern_fog_planar_eye_density_adj_inv = 75,
    _source_extern_fog_planar_eye_density_adj = 76,

    _source_extern_hud_waypoint_primary_color = 77,
    _source_extern_hud_waypoint_secondary_color = 78,

    _source_extern_lightmap_object_specular_color_times_one_half = 79,

    _source_extern_light_specular_enabled = 80,
    _source_extern_light_definition_specular_enabled = 81,

    _source_extern_object_active_camo_amount = 82,
    _source_extern_object_super_camo_amount = 83,

    _source_extern_hud_custom_color1 = 84,
    _source_extern_hud_custom_color2 = 85,
    _source_extern_hud_custom_color3 = 86,
    _source_extern_hud_custom_color4 = 87,

    _source_extern_object_active_camo_rgb = 88,

    _source_extern_fog_patchy_plane_nxyz = 89,
    _source_extern_fog_patchy_plane_dw = 90,
    
    _source_extern_hud_global_fade = 91,
    
    _source_extern_screeneffect_primary = 92,
    _source_extern_screeneffect_secondary = 93
};

enum e_shader_pass_implementation_flags : uint16
{
    _shader_pass_implementation_delete_from_cache_file_bit = 0,
    _shader_pass_implementation_critical_bit = 1,
    k_shader_pass_implementation_flag_count
};

enum e_shader_pass_texture_source_extern : int16
{
    _shader_pass_texture_source_extern_none = 0,
    _shader_pass_texture_source_extern_global_vector_normalization = 1,

    _shader_pass_texture_source_extern_unused_2 = 2,

    _shader_pass_texture_source_extern_global_target_texaccum = 3,

    _shader_pass_texture_source_extern_utility_buffer = 4,

    _shader_pass_texture_source_extern_global_target_frame_buffer = 5,
    _shader_pass_texture_source_extern_global_target_z = 6,

    _shader_pass_texture_source_extern_unused_7 = 7,

    _shader_pass_texture_source_extern_global_target_shadow = 8,

    _shader_pass_texture_source_extern_light_falloff = 9,
    _shader_pass_texture_source_extern_light_gel = 10,
    _shader_pass_texture_source_extern_lightmap = 11,

    _shader_pass_texture_source_extern_unused_12 = 12,

    _shader_pass_texture_source_extern_global_shadow_buffer = 13,
    _shader_pass_texture_source_extern_global_gradient_separate = 14,
    _shader_pass_texture_source_extern_global_gradient_product = 15,

    _shader_pass_texture_source_extern_hud_bitmap = 16,

    _shader_pass_texture_source_extern_global_active_camo = 17,
    _shader_pass_texture_source_extern_global_texture_camera = 18,
    _shader_pass_texture_source_extern_global_water_reflection = 19,
    _shader_pass_texture_source_extern_global_water_refraction = 20,
    _shader_pass_texture_source_extern_global_aux1 = 21,
    _shader_pass_texture_source_extern_global_aux2 = 22,
    _shader_pass_texture_source_extern_global_particle_distortion = 23,
    _shader_pass_texture_source_extern_global_convolution1 = 24,
    _shader_pass_texture_source_extern_global_convolution2 = 25,

    _shader_pass_texture_source_extern_shader_active_camo_bump = 26,

    _shader_pass_texture_source_extern_firstperson_scope = 27
};

enum e_shader_pass_texture_mode : int16
{
    _shader_pass_texture_mode_2d = 0,
    _shader_pass_texture_mode_3d = 1,
    _shader_pass_texture_mode_cube_map = 2,
    _shader_pass_texture_mode_passthrough = 3,
    _shader_pass_texture_mode_texkill = 4,
    _shader_pass_texture_mode_2d_dependent_ar = 5,
    _shader_pass_texture_mode_2d_dependent_gb = 6,
    _shader_pass_texture_mode_2d_bumpenv = 7,
    _shader_pass_texture_mode_2d_bumpenv_luminance = 8,
    _shader_pass_texture_mode_3d_brdf = 9,
    _shader_pass_texture_mode_dot_product = 10,
    _shader_pass_texture_mode_dot_product_2d = 11,
    _shader_pass_texture_mode_dot_product_3d = 12,
    _shader_pass_texture_mode_dot_product_cube_map = 13,
    _shader_pass_texture_mode_dot_product_zw = 14,
    _shader_pass_texture_mode_dot_reflect_diffuse = 15,
    _shader_pass_texture_mode_dot_reflect_specular = 16,
    _shader_pass_texture_mode_dot_reflect_specular_const = 17,
    _shader_pass_texture_mode_none = 18
};

enum e_shader_pass_texture_dot_mapping : int16
{
    _shader_pass_texture_dot_mapping_0_to_1 = 0,
    _shader_pass_texture_dot_mapping_signed_d3d = 1,
    _shader_pass_texture_dot_mapping_signed_gl = 2,
    _shader_pass_texture_dot_mapping_signed_nv = 3,
    _shader_pass_texture_dot_mapping_hi_lo_0_to_1 = 4,
    _shader_pass_texture_dot_mapping_hi_lo_signed_hemisphere_d3d = 5,
    _shader_pass_texture_dot_mapping_hi_lo_signed_hemisphere_gl = 6,
    _shader_pass_texture_dot_mapping_hi_lo_signed_hemisphere_nv = 7
};

enum e_texture_address_mode : int16
{
    _texture_address_mode_wrap = 0,
    _texture_address_mode_mirror = 1,
    _texture_address_mode_clamp = 2,
    _texture_address_mode_border = 3,
    _texture_address_mode_clamp_to_edge = 4
};

enum e_texture_filter : int16
{
    _texture_filter_none = 0,
    _texture_filter_point_sampled = 1,
    _texture_filter_linear = 2,
    _texture_filter_anisotropic = 3,
    _texture_filter_quincunx = 4,
    _texture_filter_gaussian_cubic = 5
};

enum e_texture_anisotropy : int16
{
    _texture_anisotropy_non_anisotropic = 0,
    _texture_anisotropy_2tap = 1,
    _texture_anisotropy_3tap = 2,
    _texture_anisotropy_4tap = 3
};

enum e_texture_state_kill_flags : uint16
{
    _texture_state_kill_alpha_kill_bit = 0,
    k_texture_state_kill_flag_count
};

enum e_texture_colorkey_mode : int16
{
    _texture_colorkey_mode_disabled = 0,
    _texture_colorkey_mode_zero_alpha = 1,
    _texture_colorkey_mode_zero_argb = 2,
    _texture_colorkey_mode_kill = 3
};

enum e_texture_component_sign_flags : uint16
{
    _texture_component_sign_r_signed_bit = 0,
    _texture_component_sign_g_signed_bit = 1,
    _texture_component_sign_b_signed_bit = 2,
    _texture_component_sign_a_signed_bit = 3,
    k_texture_component_sign_flag_count
};

enum e_texture_state_constant : int16
{
    _texture_state_constant_mipmap_bias_value = 0,
    _texture_state_constant_colorkey_color = 1,
    _texture_state_constant_border_color = 2,
    _texture_state_constant_border_alpha_value = 3,
    _texture_state_constant_bumpenv_mat00 = 4,
    _texture_state_constant_bumpenv_mat01 = 5,
    _texture_state_constant_bumpenv_mat10 = 6,
    _texture_state_constant_bumpenv_mat11 = 7,
    _texture_state_constant_bumpenv_lum_scale_value = 8,
    _texture_state_constant_bumpenv_lum_offset_value = 9
};

enum e_vs_constant_scale_by_texture_stage : int16
{
    _vs_constant_scale_by_texture_stage_none = 0,
    _vs_constant_scale_by_texture_stage_0 = 1,
    _vs_constant_scale_by_texture_stage_1 = 2,
    _vs_constant_scale_by_texture_stage_2 = 3,
    _vs_constant_scale_by_texture_stage_3 = 4
};

enum e_register_bank : int16
{
    _register_bank_vn015 = 0,
    _register_bank_cn012 = 1
};

enum e_vs_component_mask : int16
{
    _vs_component_mask_x_value = 0,
    _vs_component_mask_y_value = 1,
    _vs_component_mask_z_value = 2,
    _vs_component_mask_w_value = 3,
    _vs_component_mask_x_yzrgbcolor = 4,
    _vs_component_mask_x_uniform_scale = 5,
    _vs_component_mask_y_uniform_scale = 6,
    _vs_component_mask_z_uniform_scale = 7,
    _vs_component_mask_w_uniform_scale = 8,
    _vs_component_mask_xy_2d_scale = 9,
    _vs_component_mask_zw_2d_scale = 10,
    _vs_component_mask_xy_2d_translation = 11,
    _vs_component_mask_zw_2d_translation = 12,
    _vs_component_mask_xyzw_2d_simple_xform = 13,
    _vs_component_mask_xyw_row1_2d_affine_xform = 14,
    _vs_component_mask_xyw_row2_2d_affine_xform = 15,
    _vs_component_mask_xyz_3d_scale = 16,
    _vs_component_mask_xyz_3d_translation = 17,
    _vs_component_mask_xyzw_row1_3d_affine_xform = 18,
    _vs_component_mask_xyzw_row2_3d_affine_xform = 19,
    _vs_component_mask_xyzw_row3_3d_affine_xform = 20,
};

enum e_shader_pass_implementation_channel : int16
{
    _shader_pass_implementation_channel_all = 0,
    _shader_pass_implementation_channel_color_only = 1,
    _shader_pass_implementation_channel_alpha_only = 2,
    _shader_pass_implementation_channel_custom = 3
};

enum e_shader_pass_implementation_alphablend : int16
{
    _shader_pass_implementation_alphablend_disabled = 0,
    _shader_pass_implementation_alphablend_add = 1,
    _shader_pass_implementation_alphablend_multiply = 2,
    _shader_pass_implementation_alphablend_add_src_times_dstalpha = 3,
    _shader_pass_implementation_alphablend_add_src_times_srcalpha = 4,
    _shader_pass_implementation_alphablend_add_dst_times_srcalpha_inverse = 5,
    _shader_pass_implementation_alphablend_alpha_blend = 6,
    _shader_pass_implementation_alphablend_custom = 7
};

enum e_shader_pass_implementation_depth : int16
{
    _shader_pass_implementation_depth_disabled = 0,
    _shader_pass_implementation_depth_default_opaque = 1,
    _shader_pass_implementation_depth_default_opaque_write = 2,
    _shader_pass_implementation_depth_default_transparent = 3,
    _shader_pass_implementation_depth_custom = 4
};

enum e_alpha_blend_function : int16
{
    _alpha_blend_function_add = 0,
    _alpha_blend_function_subtract = 1,
    _alpha_blend_function_reverse_subtract = 2,
    _alpha_blend_function_min = 3,
    _alpha_blend_function_max = 4,
    _alpha_blend_function_add_signed = 5,
    _alpha_blend_function_reverse_subtract_signed = 6,
    _alpha_blend_function_logicop = 7
};

enum e_alpha_blend_factor : int16
{
    _alpha_blend_factor_zero = 0,
    _alpha_blend_factor_one = 1,
    _alpha_blend_factor_srccolor = 2,
    _alpha_blend_factor_srccolor_inverse = 3,
    _alpha_blend_factor_srcalpha = 4,
    _alpha_blend_factor_srcalpha_inverse = 5,
    _alpha_blend_factor_dstcolor = 6,
    _alpha_blend_factor_dstcolor_inverse = 7,
    _alpha_blend_factor_dstalpha = 8,
    _alpha_blend_factor_dstalpha_inverse = 9,
    _alpha_blend_factor_srcalpha_saturate = 10,
    _alpha_blend_factor_constant_color = 11,
    _alpha_blend_factor_constant_color_inverse = 12,
    _alpha_blend_factor_constant_alpha = 13,
    _alpha_blend_factor_constant_alpha_inverse = 14,
};

enum e_alpha_blend_logic_op_flags : uint16
{
    _alpha_blend_logic_op_src_0_dst_0_bit = 0,
    _alpha_blend_logic_op_src_0_dst_1_bit = 1,
    _alpha_blend_logic_op_src_1_dst_0_bit = 2,
    _alpha_blend_logic_op_src_1_dst_1_bit = 3,
    k_alpha_blend_logic_op_flag_count
};

enum e_shader_state_alpha_test_flags : uint16
{
    _shader_state_alpha_test_alphatest_enabled_bit = 0,
    _shader_state_alpha_test_samplealpha_to_coverage = 1,
    _shader_state_alpha_test_samplealpha_to_one = 2,
    k_shader_state_alpha_test_flag_count
};

enum e_compare_function : int16
{
    _compare_function_never = 0,
    _compare_function_less = 1,
    _compare_function_equal = 2,
    _compare_function_less_or_equal = 3,
    _compare_function_greater = 4,
    _compare_function_not_equal = 5,
    _compare_function_greater_or_equal = 6,
    _compare_function_always = 7
};

enum e_depth_mode : int16
{
    _depth_mode_use_z = 0,
    _depth_mode_use_w = 1,
};

enum e_depth_state_flags : uint16
{
    _depth_state_depth_write_bit = 0,
    _depth_state_offset_points_bit = 1,
    _depth_state_offset_lines_bit = 2,
    _depth_state_offset_triangles_bit = 3,
    _depth_state_clip_control_dont_cull_primitive_bit = 4,
    _depth_state_clip_control_clamp_bit = 5,
    _depth_state_clip_control_ignore_w_sign_bit = 6,
    k_depth_state_flag_count
};

enum e_cull_state_mode : int16
{
    _cull_state_mode_none = 0,
    _cull_state_mode_cw = 1,
    _cull_state_mode_ccw = 2
};

enum e_cull_front_face : int16
{
    _cull_front_face_cw = 0,
    _cull_front_face_ccw = 1
};

enum e_fill_state_flags : uint16
{
    _fill_state_flat_shading_bit = 0x1,
    _fill_state_edge_antialiasing_bit = 0x2,
    k_fill_state_flag_count
};

enum e_fill_mode : int16
{
    _fill_mode_solid = 0,
    _fill_mode_wireframe = 1,
    _fill_mode_points = 2
};

enum e_misc_state_flags : uint16
{
    _misc_state_yuv_to_rgb_bit = 0,
    _misc_state_16bit_dither_bit = 1,
    _misc_state_32bit_dxt1_noise_bit = 2,
    k_misc_state_flag_count
};

/* structures */

// max count: k_maximum_parameters_per_shader
struct s_shader_pass_parameter
{
    string_id name;
    /****************************************
    * definition_name: shader_text_definition
    * flags: 6
    * alignment_bit: 0
    ****************************************/
    // DataSize(65535)
    data_reference explanation;

    e_shader_parameter_type type;
    c_flags_no_init<e_shader_pass_flags, uint16, k_shader_pass_flag_count> flags;

    tag_reference default_bitmap;   // bitm
    real32 default_const_value;
    real_rgb_color default_const_color;
    e_source_extern source_extern;
    int16 pad;
};
ASSERT_STRUCT_SIZE(s_shader_pass_parameter, 44)

// max count: 1
struct s_shader_texture_state_address_state
{
    e_texture_address_mode u_address_mode;
    e_texture_address_mode v_address_mode;
    e_texture_address_mode w_address_mode;
    int16 pad;
};
ASSERT_STRUCT_SIZE(s_shader_texture_state_address_state, 8)

// max count: 1
struct s_shader_texture_state_filter_state
{
    e_texture_filter mag_filter;
    e_texture_filter min_filter;
    e_texture_filter mip_filter;
    int16 pad;

    real32 mipmap_bias;
    int16 max_mipmap_index; // 0 means all mipmap levels are used
    e_texture_anisotropy anisotropy;
};
ASSERT_STRUCT_SIZE(s_shader_texture_state_address_state, 8)

// max count: 1
struct s_shader_texture_state_kill_state
{
    c_flags_no_init<e_texture_state_kill_flags, uint16, k_texture_state_kill_flag_count> flags;
    int16 pad;

    e_texture_colorkey_mode colorkey_mode;
    int16 pad1;
    pixel32 colorkey_color;
};
ASSERT_STRUCT_SIZE(s_shader_texture_state_kill_state, 12)

// max count: 1
struct s_shader_texture_state_misc_state
{
    c_flags_no_init<e_texture_component_sign_flags, uint16, k_texture_component_sign_flag_count> component_sign_flags;
    int16 pad;
    pixel32 border_color;
};
ASSERT_STRUCT_SIZE(s_shader_texture_state_misc_state, 8)

// max count: k_number_of_shader_texture_state_constants
struct s_shader_state_constant
{
    string_id source_parameter;
    int16 pad;
    e_texture_state_constant constant;
};
ASSERT_STRUCT_SIZE(s_shader_state_constant, 8)

// max count: k_maximum_textures_per_shader_pass_implementation
struct s_shader_pass_texture
{
    string_id source_parameter;
    e_shader_pass_texture_source_extern source_extern;
    int16 pad[2];
    
    e_shader_pass_texture_mode mode;
    int16 pad1;

    e_shader_pass_texture_dot_mapping dot_mapping;
    int16 input_stage;
    int16 pad2;

    tag_block<s_shader_texture_state_address_state> address_state;
    tag_block<s_shader_texture_state_filter_state> filter_state;
    tag_block<s_shader_texture_state_kill_state> kill_state;
    tag_block<s_shader_texture_state_misc_state> misc_state;
    tag_block<s_shader_state_constant> constants;
};
ASSERT_STRUCT_SIZE(s_shader_pass_texture, 60)

// max count: k_maximum_vertex_shader_constants_per_shader_pass_implementation
struct s_shader_pass_constant
{
    string_id source_parameter;
    e_vs_constant_scale_by_texture_stage scale_by_texture_stage;
    e_register_bank register_bank;
    int16 register_index;
    e_vs_component_mask component_mask;
};
ASSERT_STRUCT_SIZE(s_shader_pass_constant, 12)

// max count: 1
struct s_shader_state_channels_state
{
    c_flags_no_init<e_texture_component_sign_flags, uint16, k_texture_component_sign_flag_count> flags;
    int16 pad;
};
ASSERT_STRUCT_SIZE(s_shader_state_channels_state, 4)

// max count: 1
struct s_shader_state_alpha_blend_state
{
    e_alpha_blend_function blend_function;
    e_alpha_blend_factor blend_src_factor;
    e_alpha_blend_factor blend_dst_factor;
    int16 pad;
    pixel32 blend_color;
    c_flags_no_init<e_alpha_blend_logic_op_flags, uint16, k_alpha_blend_logic_op_flag_count> logic_op_flags;
    int16 pad1;
};
ASSERT_STRUCT_SIZE(s_shader_state_alpha_blend_state, 16)

// max count: 1
struct s_shader_state_alpha_test_state
{
    e_shader_state_alpha_test_flags flags;
    e_compare_function alpha_compare_function;
    int16 alpha_test_ref;   // 0 - 255
    int16 pad;
};
ASSERT_STRUCT_SIZE(s_shader_state_alpha_test_state, 8)

// max count: 1
struct s_shader_state_depth_state
{
    e_depth_mode mode;
    e_compare_function depth_compare_function;
    c_flags_no_init<e_depth_state_flags, uint16, k_depth_state_flag_count> flags;
    int16 pad;

    real32 depth_bias_slope_scale;
    real32 depth_bias;
};
ASSERT_STRUCT_SIZE(s_shader_state_depth_state, 16)

// max count: 1
struct s_shader_state_cull_state
{
    e_cull_state_mode mode;
    e_cull_front_face front_face;
};
ASSERT_STRUCT_SIZE(s_shader_state_cull_state, 4)

// max count: 1
struct s_shader_state_fill_state
{
    c_flags_no_init<e_fill_state_flags, uint16, k_fill_state_flag_count> flags;
    e_fill_mode fill_mode;
    e_fill_mode back_fill_mode;
    int16 pad;
    real32 line_width;
};
ASSERT_STRUCT_SIZE(s_shader_state_fill_state, 12)

// max count: 1
struct s_shader_state_misc_state
{
    c_flags_no_init<e_misc_state_flags, uint16, k_misc_state_flag_count> flags;
    int16 pad;
    pixel32 fog_color;
};
ASSERT_STRUCT_SIZE(s_shader_state_misc_state, 8)

// max count: k_maximum_implementations_per_shader_pass
struct s_shader_pass_implementation
{
    c_flags_no_init<e_shader_pass_implementation_flags, uint16, k_shader_pass_implementation_flag_count> flags;
    int16 pad;
    tag_block<s_shader_pass_texture> textures;

    // Explaination("VERTEX SHADER", "EMPTY STRING")
    
    tag_reference vertex_shader;    // vrtx
    tag_block<s_shader_pass_constant> vs_constants;

    /****************************************
    * definition_name: shader_text_definition
    * flags: 6
    * alignment_bit: 0
    ****************************************/
    // DataSize(65535)
    data_reference pixel_shader_code;

    // Explaination("STATE", "EMPTY STRING")

    e_shader_pass_implementation_channel channels;
    e_shader_pass_implementation_alphablend alphablend;
    e_shader_pass_implementation_depth depth;
    int16 pad1;

    tag_block<s_shader_state_channels_state> channel_state;
    tag_block<s_shader_state_alpha_blend_state> alphablend_state;
    tag_block<s_shader_state_alpha_test_state> alphatest_state;
    tag_block<s_shader_state_depth_state> depth_state;
    tag_block<s_shader_state_cull_state> cull_state;
    tag_block<s_shader_state_fill_state> fill_state;
    tag_block<s_shader_state_misc_state> misc_state;
    tag_block<s_shader_state_constant> constants;

    tag_reference pixel_shader; // pixl
};
ASSERT_STRUCT_SIZE(s_shader_pass_implementation, 116)


struct s_shader_pass_definition
{
    data_reference documentation;
    tag_block<s_shader_pass_parameter> parameters;
    int16 pad[2];
    tag_block<s_shader_pass_implementation> implementations;
    tag_block<s_shader_pass_postprocess_definition_new> postprocess_definition;
};
ASSERT_STRUCT_SIZE(s_shader_pass_definition, 36)
