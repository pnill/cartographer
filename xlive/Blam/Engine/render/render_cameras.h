#pragma once
#include "camera/camera.h"
#include "math/matrix_math.h"

struct s_oriented_bounding_box
{
	real32 matrix[4][4];
};
ASSERT_STRUCT_SIZE(s_oriented_bounding_box, 64);

struct render_projection
{
	real_matrix4x3 world_to_view;
	real_matrix4x3 view_to_world;
	real_rectangle2d projection_bounds;
	s_oriented_bounding_box projection_matrix;
	real_vector2d unknownB8;
};
ASSERT_STRUCT_SIZE(render_projection, 192);

void render_cameras_apply_patches(void);

render_projection* global_projection_get(void);

void __cdecl render_camera_build_projection(s_camera* camera,
	real_rectangle2d* frustum_bounds,
	render_projection* projection);

// Builds frustum bounds from render camera
void __cdecl render_camera_build_viewport_frustum_bounds(const s_camera* camera, real_rectangle2d* frustum_bounds);

bool __cdecl render_projection_point_to_screen(
	const real_point3d* point,
	real32  occlusion_radius,
	real_rectangle2d* rect,
	real_bounds* bounds);
