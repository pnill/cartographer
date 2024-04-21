#include "stdafx.h"
#include "render_cameras.h"

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
