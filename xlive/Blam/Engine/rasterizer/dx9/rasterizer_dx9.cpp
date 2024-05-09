#include "stdafx.h"
#include "rasterizer_dx9.h"

/* public code */

bool* rasterizer_dx9_disable_stencil_get(void)
{
    return Memory::GetAddress<bool*>(0xA3C63A);
}

c_rasterizer_constant_4f_cache<256>* rasterizer_get_main_vertex_shader_cache(void)
{
    return Memory::GetAddress<c_rasterizer_constant_4f_cache<256>*>(0xA3C7B0);
}

c_rasterizer_constant_4f_cache<32>* rasterizer_get_main_pixel_shader_cache(void)
{
    return Memory::GetAddress<c_rasterizer_constant_4f_cache<32>*>(0xA3D7B0);
}

