#include "stdafx.h"
#include "render_cameras.h"

#include "render.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "saved_games/cartographer_player_profile.h"

/* prototypes */

void __cdecl render_camera_build_projection_static(s_camera* camera, real_rectangle2d* frustum_bounds, render_projection* out_projection);

/* public code */

void render_cameras_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x191440), render_camera_build_projection_static);
	return;
}

render_projection* global_projection_get(void)
{
	return Memory::GetAddress<render_projection*>(0x4E673C);
}

void __cdecl render_camera_build_projection(s_camera* camera,
	real_rectangle2d* frustum_bounds,
	render_projection* projection)
{
	INVOKE(0x1953f5, 0x0, render_camera_build_projection, camera, frustum_bounds, projection);
	return;
}

void __cdecl render_camera_build_projection_static(s_camera* camera, real_rectangle2d* frustum_bounds, render_projection* out_projection)
{
	real32 old_camera_field_of_view = camera->vertical_field_of_view;
	s_saved_game_cartographer_player_profile_v1* profile_settings = cartographer_player_profile_get_by_user_index(global_render_current_user_index());

	if (profile_settings)
	{
		if (profile_settings->static_first_person)
		{
			camera->vertical_field_of_view = DEGREES_TO_RADIANS(64.f) * 0.78500003f;
		}
	}
	
	render_camera_build_projection(camera, frustum_bounds, out_projection);
	camera->vertical_field_of_view = old_camera_field_of_view;
	return;
}

void __cdecl render_camera_build_viewport_frustum_bounds(const s_camera* camera, real_rectangle2d* frustum_bounds)
{
	INVOKE(0x194FBD, 0x180EB6, render_camera_build_viewport_frustum_bounds, camera, frustum_bounds);
	return;
}

bool render_projection_point_to_screen(
	const real_point3d* camera_position,
	real32  occlusion_radius,
	real_vector4d* out_view_position,
	real_bounds* bounds)
{
	bool result = false;
	if (occlusion_radius > 0.f)
	{
		const s_frame* global_window_parameters = global_window_parameters_get();

		real_point3d transformed_point;
		matrix4x3_transform_point(&global_window_parameters->projection.world_to_view, camera_position, &transformed_point);
		
		const s_oriented_bounding_box* obb = &global_window_parameters->projection.projection_matrix;

		const real32 v2 = 
			  obb->matrix[0][2] * transformed_point.x
			+ obb->matrix[1][2] * transformed_point.y
			+ obb->matrix[2][2] * transformed_point.z
			+ obb->matrix[3][2];

		if (v2 > 0.f)
		{
			const real32 divisor =
				  obb->matrix[0][3] * transformed_point.x
				+ obb->matrix[1][3] * transformed_point.y
				+ obb->matrix[2][3] * transformed_point.z
				+ obb->matrix[3][3];

			const real32 matrix_scale_x = obb->matrix[0][0] * occlusion_radius;
			const real32 matrix_scale_y = obb->matrix[1][1] * occlusion_radius;

			const real32 v1 =
				  obb->matrix[0][1] * transformed_point.x
				+ obb->matrix[1][1] * transformed_point.y
				+ obb->matrix[2][1] * transformed_point.z
				+ obb->matrix[3][1];
			const real32 v6 = 
				  obb->matrix[0][0] * transformed_point.x
				+ obb->matrix[1][0] * transformed_point.y
				+ obb->matrix[2][0] * transformed_point.z
				+ obb->matrix[3][0];			


			out_view_position->i = v6 / divisor;
			out_view_position->j = v1 / divisor;
			out_view_position->k = v2 / divisor;
			out_view_position->l = 1.f;

			bounds->lower = matrix_scale_x / divisor;
			bounds->upper = matrix_scale_y / divisor;
			result = true;
		}
	}

	return result;
	//return INVOKE(0x2799A3, 0x0, render_projection_point_to_screen, point, occlusion_radius, rect, bounds);
}

bool render_camera_world_to_screen(
	const s_camera* camera,
	const render_projection* projection,
	const rectangle2d* viewport_bounds,
	const real_point3d* view_point,
	real_point2d* screen_point)
{
	ASSERT(camera);
	ASSERT(projection);
	ASSERT(view_point);
	ASSERT(screen_point);

	const rectangle2d* bounds = (viewport_bounds ? viewport_bounds : &camera->viewport_bounds);

	bool within_screen_bounds;
	if (view_point->z >= 0.f)
	{
		screen_point->x = 0.f;
		screen_point->y = 0.f;
		within_screen_bounds = false;
	}
	else
	{
		int16 width = rectangle2d_width(bounds);
		int16 height = rectangle2d_height(bounds);

		real32 view_z = -1.f / view_point->z;
		screen_point->x = (
			  projection->projection_matrix.matrix[0][0] * view_point->x
			+ projection->projection_matrix.matrix[2][0] * view_point->z) * view_z;

		screen_point->y = (
			  projection->projection_matrix.matrix[1][1] * view_point->y
			+ projection->projection_matrix.matrix[2][1] * view_point->z) * -view_z;

		within_screen_bounds = screen_point->x >= -1.f && screen_point->x <= 1.f && screen_point->y >= -1.f && screen_point->y <= 1.f;

		screen_point->x = (screen_point->x * 0.5f + 0.5f) * width + bounds->left;
		screen_point->y = (screen_point->y * 0.5f + 0.5f) * height + bounds->top;
	}

	return within_screen_bounds;
}