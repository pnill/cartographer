#include "stdafx.h"
#include "rasterizer_dx9_main.h"

#include "rasterizer_dx9_errors.h"

/* public code */

void rasterizer_dx9_bitmaps_apply_patches(void)
{
    return;
}

bool rasterizer_dx9_default_bitmaps_initialize(void)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
    
    IDirect3D9* d3d;
    const HRESULT hr = global_d3d_device->GetDirect3D(&d3d);

    const bool result = SUCCEEDED(hr);
    if (!result)
    {
        rasterizer_dx9_errors_log(hr, "global_d3d_device->GetDirect3D(&d3d)");
    }
    else
    {
        d3d->Release();
    }

    return result;
}
