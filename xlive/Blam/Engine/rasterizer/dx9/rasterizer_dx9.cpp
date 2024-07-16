#include "stdafx.h"
#include "rasterizer_dx9.h"

#ifndef NDEBUG
#include "text/unicode.h"
#endif



/* constants */

#ifndef NDEBUG
#define D3D9_PERF_EVENTS
#endif

/* globals */

// bool g_rasterizer_d3d_perf = true;

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

void rasterizer_dx9_perf_event_begin(const char* event_description, real_argb_color* color)
{
#ifdef D3D9_PERF_EVENTS
    wchar_t wide_string[256];
    utf8_string_to_wchar_string(event_description, wide_string, NUMBEROF(wide_string));
        
    // If color is null set the color of the event to white
    pixel32 d3dcolor = (color ? real_argb_color_to_pixel32(color) : global_white_pixel32 );
    D3DPERF_BeginEvent(d3dcolor.color, (LPCWSTR)wide_string);
#endif
    return;
}

void rasterizer_dx9_perf_event_end(const char* event_description)
{
#ifdef D3D9_PERF_EVENTS
    D3DPERF_EndEvent();
#endif
    return;
}