#include "stdafx.h"
#include "rasterizer_dx9.h"

#include "text/unicode.h"

/* globals */

bool g_rasterizer_d3d_perf = true;

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

void rasterizer_dx9_perf_event_begin(const char* string, real_argb_color* color)
{
    if (g_rasterizer_d3d_perf)
    {
        wchar_t wide_string[256];
        utf8_string_to_wchar_string(string, wide_string, NUMBEROF(wide_string));
        
        // If color is null set the color of the event to white
        pixel32 d3dcolor = (color ? real_argb_color_to_pixel32(color) : global_white_pixel32 );
        D3DPERF_BeginEvent(d3dcolor.color, (LPCWSTR)wide_string);
    }
    return;
}

void rasterizer_dx9_perf_event_end(void)
{
    if (g_rasterizer_d3d_perf)
    {
        D3DPERF_EndEvent();
    }
    return;
}