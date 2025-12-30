#pragma once

struct render_camera
{
	real_point3d point;
	real_vector3d forward;
	real_vector3d up;
	bool mirrored;
	int8 pad[3];
	real32 vertical_field_of_view;
	real32 scale;
	rectangle2d viewport_bounds;
	rectangle2d window_bounds;
	real32 z_near;
	real32 z_far;
	real_plane3d mirror_plane;
	bool enlarge_view;
	int8 pad1[3];
	real32 unk_floats[3];
	bool bool_2;
	int8 pad2[3];
	real32 frustum_multiplier_x;
	real32 frustum_multiplier_y;
};
ASSERT_STRUCT_SIZE(render_camera, 0x74);

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

/* public code */

void render_cameras_apply_patches(void);

void __cdecl render_camera_build_projection(render_camera* camera,
	real_rectangle2d* frustum_bounds,
	render_projection* projection);

// Builds frustum bounds from render camera
void __cdecl render_camera_build_viewport_frustum_bounds(const render_camera* camera, real_rectangle2d* frustum_bounds);

bool render_projection_point_to_screen(
	const real_point3d* camera_position,
	real32  occlusion_radius,
	real_vector4d* out_view_position,
	real_bounds* bounds);

bool render_camera_world_to_screen(
	const render_camera* camera,
	const render_projection* projection,
	const rectangle2d* viewport_bounds,
	const real_point3d* view_point,
	real_point2d* screen_point);

void render_camera_build(
	render_camera* camera,
	const struct s_observer_result* observer,
	const point2d* view_size,
	const point2d* view_array_size);
