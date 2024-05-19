#include "stdafx.h"
#include "render_cameras.h"

#include "render.h"

#include "H2MOD/Modules/Shell/Config.h"

void __cdecl render_camera_build_projection_static(s_camera* camera, real_rectangle2d* frustum_bounds, render_projection* out_projection);

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
	
	if (H2Config_static_first_person) 
	{
		camera->vertical_field_of_view = DEGREES_TO_RADIANS(64.f) * 0.78500003f;
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

bool __cdecl render_projection_point_to_screen(
	const real_point3d* camera_position,
	real32  occlusion_radius,
	real_rectangle2d* rect,
	real_bounds* bounds)
{
	bool result = false;
	if (occlusion_radius > 0.f)
	{
		s_frame* global_window_parameters = global_window_parameters_get();

		real_point3d transformed_point;
		matrix4x3_transform_point(&global_window_parameters->projection.world_to_view, camera_position, &transformed_point);
		
		s_oriented_bounding_box* obb = &global_window_parameters->projection.projection_matrix;

		real32 v2 = 
			  obb->matrix[0][2] * transformed_point.x
			+ obb->matrix[1][2] * transformed_point.y
			+ obb->matrix[2][2] * transformed_point.z
			+ obb->matrix[3][2];

		if (v2 > 0.f)
		{
			real32 divisor =
				  obb->matrix[0][3] * transformed_point.x
				+ obb->matrix[1][3] * transformed_point.y
				+ obb->matrix[2][3] * transformed_point.z
				+ obb->matrix[3][3];

			real32 matrix_scale_x = obb->matrix[0][0] * occlusion_radius;
			real32 matrix_scale_y = obb->matrix[1][1] * occlusion_radius;

			real32 v1 =
				  obb->matrix[0][1] * transformed_point.x
				+ obb->matrix[1][1] * transformed_point.y
				+ obb->matrix[2][1] * transformed_point.z
				+ obb->matrix[3][1];
			real32 v6 = 
				  obb->matrix[0][0] * transformed_point.x
				+ obb->matrix[1][0] * transformed_point.y
				+ obb->matrix[2][0] * transformed_point.z
				+ obb->matrix[3][0];			


			rect->x0 = v6 / divisor;
			rect->x1 = v1 / divisor;
			rect->y0 = v2 / divisor;
			rect->y1 = 1.f;
			bounds->lower = matrix_scale_x / divisor;
			bounds->upper = matrix_scale_y / divisor;
			result = true;
		}
	}

	return result;
	//return INVOKE(0x2799A3, 0x0, render_projection_point_to_screen, point, occlusion_radius, rect, bounds);
}
