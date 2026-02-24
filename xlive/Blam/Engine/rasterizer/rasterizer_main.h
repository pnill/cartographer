#pragma once
#include "rasterizer_globals.h"

#include "math/color_math.h"

/* enums */

enum e_framebuffer_blend_function : int16
{
	_framebuffer_blend_function_alpha_blend = 0,
	_framebuffer_blend_function_multiply = 1,
	_framebuffer_blend_function_double_multiply = 2,
	_framebuffer_blend_function_add = 3,
	_framebuffer_blend_function_subtract = 4,
	_framebuffer_blend_function_component_min = 5,
	_framebuffer_blend_function_component_max = 6,
	_framebuffer_blend_function_alpha_multiply_add = 7,
	_framebuffer_blend_function_constant_color_blend = 8,
	_framebuffer_blend_function_inverse_constant_color_blend = 9,
	_framebuffer_blend_function_none = 10,
	k_shader_framebuffer_blend_function_count = 12
};

/* structures */

struct s_rasterizer_debug_options
{
	int16 field_0;
	int16 statistics_mode;
	int16 drawing_mode;
	int8 pad0[2];
	bool wireframe_enabled;
	int8 pad[3];
	real32 gamma_power;
	int16 pad1;
	int16 debug_model_lod;
	bool debug_transparent_geometry_enabled;
	bool debug_meter_shader_enabled;
	bool draw_models;
	bool draw_model_transparent_geometry;
	bool draw_first_person_weapon_first;
	bool stencil_mask_enabled;
	bool draw_environment;
	bool draw_decals;
	bool draw_water;
	bool draw_lens_flares;
	bool draw_lens_flares_dof_region;
	bool pad_1;
	bool draw_dynamic_unlit_geometry;
	bool draw_dynamic_lit_geometry;
	bool draw_dynamic_screen_geometry;
	bool draw_hud_motion_sensor;
	bool draw_detail_objects;
	bool draw_debug_geometry;
	bool debug_geometry_multipass;
	bool fog_atmospheric_enabled;
	bool fog_planar_enabled;
	bool bump_mapping_enabled;
	int16 pad_2;
	real32 lightmap_ambient;
	int16 lightmap_mode;
	int16 pad3;
	int16 taco_salad;
	int16 anisotropic_binormal;
	bool lightmap_incident_radiosity_enabled;
	bool lightmap_filtering_enabled;
	real32 model_lighting_ambient;
	bool environment_alpha_testing_enabled;
	bool environment_specular_mask_enabled;
	bool shadow_convolution_enabled;
	bool shadow_debug_enabled;
	bool water_mipmapping_enabled;
	bool active_camouflage_enabled;
	bool active_camouflage_multipass_enabled;
	bool plasma_energy_enabled;
	bool lens_flare_occlusion_enabled;
	bool lens_flare_occlusion_debug;
	bool lens_flare_sun_glow_enabled;
	bool lens_flares_occluded_by_active_camo;
	bool screen_flash_enabled;
	bool screen_effects_enabled;
	bool DXTC_noise_enabled;
	bool profile_log_enabled;
	real32 detail_object_screen_facing_offset_multiplier;
	bool decal_flush_on_create;
	int32 decal_z_bias;
	bool decal_z_offset_slopescale_enabled;
	real32 decal_z_offset;
	real32 decal_z_slopescale;
	real32 decal_z_geometry_offset;
	int32 decal_debug_draw_grid;
	bool safe_frame_bounds;
	bool zsprite_enabled;
	bool filthy_decal_fog_hack_enabled;
	bool smart_states_enabled;
	bool splitscreen_VB_optimization_enabled;
	bool rasterizer_counters_enabled;
	bool markers_enabled;
	bool profile_print_locks;
	real32 profile_objectlock_time;
	real32 pad3_scale;
	real32 pad_thai;
	real32 f[6];
	real32 g[32];
	bool frame_dump;
	bool field_119;
	bool transparent_pixel_counter;
	int32 render_layer_view_5;
	struct bitmap_data* rasterizer_debug_display_bitmap;
	int16 pad4;
	bool shaders_enabled;
	bool karma_events;
	bool karma_events_detailed;
	bool tiny;
	int8 field_12A[2];
	int32 busywork;
	bool wbuffer_clip_enable;
	bool rasterizer_far_clip_nudge;
	bool shader_submit_unified;
	bool shader_submit_unified_safe;
	bool shader_submit_unified_super_safe;
	bool shader_submit_force_extern_refresh;
	bool sort;
	int8 pad5;
	int16 sort_force_layer;
	int16 sort_force_order;
	bool shadow_buffer_use_offsets;
	int8 pad6[3];
	real32 shadow_buffer_z_offset;
	real32 shadow_buffer_z_slope;
	real32 shadow_buffer_env_z_offset;
	real32 shadow_buffer_env_z_slope;
	real32 shadow_buffer_blur;
	bool shadow_buffer_cinematic_debug;
	int8 pad7[3];
	real32 shadow_buffer_lod_resolution_area_scale;
	real32 shadow_buffer_lod_resolution_light_bias_scale;
	real32 shadow_buffer_lod_resolution_minimum;
	real32 shadow_buffer_lod_samples_area_scale;
	real32 shadow_buffer_lod_samples_light_bias_scale;
	bool shadow_buffer_debug;
	int8 pad8;
	int16 dynamic_gamma_mode;
	real32 dynamic_gamma_bias;
	real32 dynamic_gamma_dark_clamp;
	real32 dynamic_gamma_light_clamp;
	real32 dynamic_gamma_dark_adjust_dt;
	real32 dynamic_gamma_light_adjust_dt;
	real32 dynamic_gamma_contrast_adjust_dt;
	real32 dynamic_gamma_dark_threshold;
	real32 dynamic_gamma_light_threshold;
	real32 dynamic_gamma_bloom_scale;
	bool dynamic_gamma_debug;
	int8 pad9[3];
	real32 vision_distance;
	real32 vision_jitter_scale;
	real32 vision_noise_scale;
	real32 vision_scan_scale;
	real32 vision_scan_speed;
	real32 bloom_default_threshold;
	real32 selfibloomination_scale;
	real32 selfibloomination_to_bloom_amount;
	bool selfibloomination_is_fogged;
	int8 pad10[3];
	real32 depth_of_field_near_filter_box_factor;
	real32 depth_of_field_near_filter_max_factor;
	real32 depth_of_field_far_filter_box_factor;
	real32 depth_of_field_far_filter_max_factor;
	bool colorbars;
	int8 pad11[3];
	real32 LMO_ambient;
	real32 LMO_direct;
	real32 LMO_indirect;
	real32 LMO_specular;
	real32 lightmap_shadow_falloff_scale;
	real32 lightmap_shadow_cutoff_scale;
	bool new_submission_interface;
	int8 pad12[3];
	real32 line_width;
	bool linear_gradients;
	bool fog_atmospheric_auto_blend;
	int16 pad13;
	int32 test_screen;
	real32 test_screen_val_1;
	real32 test_screen_val_2;
	real32 test_screen_supablur[5];
	bool solid_color_mode;
	int8 pad14[3];
	real_argb_color solid_model_color;
	real_argb_color solid_structure_color;
	real_argb_color solid_structure_instanced_geometry_color;
	real_argb_color solid_sky_color;
	real_argb_color solid_decorator_color;
	real_argb_color solid_decorator_model_color;
	real_argb_color solid_decorator_decal_color;
	real_argb_color solid_decorator_sprite_color;
	real32 decorator_accuracy_min;
	real32 decorator_accuracy_max;
	real32 first_person_w_scale;
	int32 tessellation_scale;
	bool patchy_fog_alpha_modulate;
	bool patchy_fog_force_100_percent;
	bool fog_planar_separate_enabled;
	bool force_specular;
	bool fog_planar_care;
	int8 pad15[3];
	int32 rz_mode;
	int32 rz_gradient_index;
	real32 rz_amount;
	real32 rz_0;
	int32 pad16;
	real32 rz_slope;
	bool light_volume_debug;
	bool light_volume_perspective_midpoints;
	int16 pad17;
	real32 light_offset;
	bool debug_fog_geometry;
	int8 pad18[3];
	real32 decal_depth_bias;
	real32 decal_depth_bias_slope_scale;
};
ASSERT_STRUCT_SIZE(s_rasterizer_debug_options, 736);

/* prototypes */

void rasterizer_main_apply_patches(void);

void rasterizer_sapien_apply_patches(void);

struct s_rasterizer_debug_options* rasterizer_debug_options_get(void);

void rasterizer_present_frame_screenshot_wrapper(struct bitmap_data* bitmap);

e_display_type rasterizer_get_display_type(void);

uint32 rasterizer_get_adapter_count(void);

bool rasterizer_is_initialized(void);

bool rasterizer_initialize(void);

void rasterizer_reset(bool create_window);

bool rasterizer_initialize_screenshot_render_target(uint32 screen_width, uint32 screen_height);

void rasterizer_cleanup_screenshot_render_target(void);

bool rasterizer_device_is_lost(void);

void rasterizer_main_render_pregame(void);
