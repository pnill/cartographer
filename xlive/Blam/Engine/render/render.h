#pragma once

#include "render_cameras.h"
#include "render_layers.h"
#include "render_visibility_collection.h"

#include "camera/camera.h"
#include "effects/player_effects.h"
#include "scenario/scenario_fog.h"

/* enums */

enum e_screen_split_type : int8
{
	_screen_split_type_full = 0,
	_screen_split_type_half = 1,
	_screen_split_type_quarter = 2,
	k_split_screen_type_count
};

/* structures */

struct s_bloom_window_data
{
	int8 field_0[16];
	real32 field_10;
	int8 field_14[16];
};

struct s_frame
{
	int16 window_bound_index;
	int16 render_type;
	int16 field_4;
	bool field_6;
	bool is_texture_camera;
	int16 alpha;
	int16 pad;
	real_rgb_color color;
	s_camera camera;
	render_projection projection;
	s_scenario_fog_result fog_result;
	bool render_fog;
	int8 pad_26D[3];
	s_screen_flash screen_flash;
	int8 field_288[8];
	s_bloom_window_data* bloom_data;
	bool field_294_zero;
	int8 pad_295[3];
};
ASSERT_STRUCT_SIZE(s_frame, 664);

struct s_frame_parameters
{
	uint32 frame_type;
	int32 unk0;
	real64 time;
	int32 dword_10;
	int32 dword_14;
};
ASSERT_STRUCT_SIZE(s_frame_parameters, 24);

struct window_bound
{
	int32 single_view;
	int32 window_bound_index;
	int32 user_index;
	s_camera render_camera;
	s_camera rasterizer_camera;
	s_bloom_window_data bloom_data;
};
ASSERT_STRUCT_SIZE(window_bound, 280);

struct s_render_scene_parameters
{
	uint32 scene_rendered_count;
	uint32 effect_flags;
	real32 depth_range;
};
ASSERT_STRUCT_SIZE(s_render_scene_parameters, 12);

/* globals */

extern bool render_lens_flares_enabled;

/* public code */

bool __cdecl render_ingame_user_interface_hud_element_hook(
	real32 left,
	real32 top,
	int16 x,
	int16 y,
	real32 scale,
	real32 rotation_rad,
	datum bitmap_tag_index,
	datum bitmap,
	real_rectangle2d* bounds,
	datum shader_tag_index);

void render_apply_patches(void);

int32 get_global_render_window_count();

bool get_global_render_split_horizontally();

e_controller_index global_render_current_controller_index();

uint32 global_render_current_user_index();

s_frame* global_window_parameters_get(void);

uint32* global_frame_num_get(void);

int32* load_low_detail_textures_get(void);

int32* get_global_window_bound_index(void);

s_frame_parameters* global_frame_parameters_get(void);

int32* global_rasterizer_pixel_shader_index_get(void);

real64 get_current_render_time(void);

int32 get_player_window_count(void);

real32* hs_texture_camera_scale_get(void);

bool* hs_texture_camera_view_get(void);

s_scenario_fog_result* global_fog_result_get(void);

int32* global_user_render_index_get(void);

uint32* global_effect_flag_get(void);

// ### TODO: better function name
bool frame_parameters_type_is_above_or_equal_to_7(void);

void render_scene_geometry(e_collection_type collection_type, e_render_layer render_layer);

// CLIENT ONLY
// Get cluster index and leaf index from render position provided
// Return true if out_cluster_index and out_leaf_index are valid
bool __cdecl structure_get_cluster_and_leaf_from_render_point(real_point3d* point, int32* out_cluster_index, int32* out_leaf_index);

e_screen_split_type get_screen_split_type(uint32 render_user_index);

void __cdecl render_frame(
	uint32 frame_render_type,
	int32 window_count,
	int32 player_count,
	int32 screen_split_type,
	window_bound* window);

// Render window
void __cdecl render_window(window_bound* window, bool is_texture_camera);

void __cdecl render_scene(
	int32 render_layer_debug_view,
	bool render_transparent_geo,
	bool lens_flare_occlusion_test,
	bool render_layer_selfibloomination,
	int32 hologram_flag,
	int32 effect_flag,
	real32 depth_range);
