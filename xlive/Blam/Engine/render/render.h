#pragma once
#include "render_cameras.h"

#include "camera/camera.h"
#include "effects/player_effects.h"
#include "scenario/scenario_fog.h"

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

/* public code */

void render_apply_patches(void);

s_frame* global_window_parameters_get(void);

s_frame_parameters* global_frame_parameters_get(void);

real64 get_current_render_time(void);

bool* hs_texture_camera_view_get(void);

s_scenario_fog_result* global_fog_result_get(void);

uint32* global_effect_flag_get(void);

// ### TODO: better function name
bool frame_parameters_type_is_above_or_equal_to_7(void);

// CLIENT ONLY
// Get cluster index and leaf index from render position provided
// Return true if out_cluster_index and out_leaf_index are valid
bool __cdecl structure_get_cluster_and_leaf_from_render_point(real_point3d* point, int32* out_cluster_index, int32* out_leaf_index);

// Render window
void __cdecl render_window(window_bound* window, bool is_texture_camera);
