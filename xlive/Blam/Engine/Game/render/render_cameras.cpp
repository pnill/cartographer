#include "stdafx.h"
#include "H2MOD\Modules\Shell\Config.h"

#include "render_cameras.h"

typedef void(__cdecl render_camera_build_projection_t)(s_camera*, float*, real_matrix4x3*);
render_camera_build_projection_t* p_render_camera_build_projection;

void __cdecl render_camera_build_projection_hook(s_camera* camera, float* frustum_bounds, real_matrix4x3* out_projection)
{
	p_render_camera_build_projection = Memory::GetAddress<render_camera_build_projection_t*>(0x1953f5);

	float old_camera_field_of_view = camera->vertical_field_of_view;
	
	if (H2Config_static_first_person) 
	{
		camera->vertical_field_of_view = ((64.f * M_PI) / 180.0f) * 0.78500003f;
	}
	
	p_render_camera_build_projection(camera, frustum_bounds, out_projection);
	
	camera->vertical_field_of_view = old_camera_field_of_view;
}
