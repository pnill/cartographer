#include "stdafx.h"
#include "rasterizer_dx9_water.h"

#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_targets.h"

#include "camera/camera.h"
#include "structures/structures.h"

/* prototypes */

bool __cdecl render_water_refraction(void);

/* public code */

bool* water_reflection_rendered_get(void)
{
    return Memory::GetAddress<bool*>(0x4F5048);
}

void rasterizer_dx9_water_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0xA07B5, 0x0), render_water_refraction);
    PatchCall(Memory::GetAddress(0x28158F, 0x0), render_water_refraction);
    return;
}

/* private code */

bool __cdecl render_water_refraction(void)
{
    bool* water_reflection_rendered = water_reflection_rendered_get();

    if (!*water_reflection_rendered)
    {
        rasterizer_dx9_reset_depth_buffer();
        if (get_global_structure_bsp()->water_definitions.count > 0)
        {
            IDirect3DSurface9* surface = rasterizer_dx9_target_get_surface(_rasterizer_target_20);
            s_rasterizer_target* target_data = rasterizer_dx9_target_get(_rasterizer_target_20);

            e_rasterizer_target render_target = *rasterizer_dx9_main_render_target_get();
            IDirect3DSurface9* render_surface = rasterizer_dx9_get_render_target_surface((uint16)render_target, 0);

            s_camera* camera = get_global_camera();
            RECT rect = { camera->viewport_bounds.left, camera->viewport_bounds.top, camera->viewport_bounds.right, camera->viewport_bounds.bottom };
            rasterizer_dx9_device_get_interface()->StretchRect(render_surface, &rect, surface, NULL, D3DTEXF_LINEAR);
        }
        *water_reflection_rendered = true;
        rasterizer_dx9_reset_depth_buffer();
    }
    return true;
}
