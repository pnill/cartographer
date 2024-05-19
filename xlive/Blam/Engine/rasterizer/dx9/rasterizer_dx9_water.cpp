#include "stdafx.h"
#include "rasterizer_dx9_water.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_targets.h"

#include "camera/camera.h"
#include "structures/structures.h"

/* prototypes */

bool __cdecl rasterizer_dx9_water_update_refraction_render_surface_with_main_rendered_surface(void);

/* public code */

bool* rasterizer_water_refraction_surface_updated_get(void)
{
    return Memory::GetAddress<bool*>(0x4F5048);
}

void rasterizer_dx9_water_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x1A07B5, 0x0), rasterizer_dx9_water_update_refraction_render_surface_with_main_rendered_surface);
    PatchCall(Memory::GetAddress(0x28158F, 0x0), rasterizer_dx9_water_update_refraction_render_surface_with_main_rendered_surface);
    return;
}

/* private code */

bool __cdecl rasterizer_dx9_water_update_refraction_render_surface_with_main_rendered_surface(void)
{
    bool* rasterizer_water_refraction_surface_updated = rasterizer_water_refraction_surface_updated_get();

    if (!*rasterizer_water_refraction_surface_updated)
    {
        rasterizer_dx9_perf_event_begin("water refraction", NULL);
        rasterizer_dx9_reset_depth_buffer();
        if (get_global_structure_bsp()->water_definitions.count > 0)
        {
            IDirect3DSurface9* surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_water_refraction);

            e_rasterizer_target render_target = *rasterizer_dx9_main_render_target_get();
            IDirect3DSurface9* render_surface = rasterizer_dx9_get_render_target_surface(render_target, 0);

            const s_frame* global_window_parameters = global_window_parameters_get();
            int16 width = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
            int16 height = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);
            int16 left = global_window_parameters->camera.viewport_bounds.left;
            int16 top = global_window_parameters->camera.viewport_bounds.top;

            const RECT rect = { left, top, width + left, height + top };
            rasterizer_dx9_device_get_interface()->StretchRect(render_surface, &rect, surface, NULL, D3DTEXF_LINEAR);
        }
        *rasterizer_water_refraction_surface_updated = true;
        rasterizer_dx9_reset_depth_buffer();

        rasterizer_dx9_perf_event_end();
    }
    return true;
}
