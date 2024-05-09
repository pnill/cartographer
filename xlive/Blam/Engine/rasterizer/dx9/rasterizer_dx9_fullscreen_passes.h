#pragma once

/* enums */

enum e_vertex_output_type : int32
{
    _vertex_output_type_position = 0,
    _vertex_output_type_texcoord = 1,
    _vertex_output_type_color = 2
};


/* public code */

void rasterizer_dx9_fullscreen_passes_apply_patches(void);

// ### TOOD rename
void __cdecl rasterizer_dx9_render_fullscreen_overlay_geometry(
    real32* a1,
    unsigned __int8(__cdecl* a2)(int32),
    bool(__cdecl* a3)(
        e_vertex_output_type input_type,
        real32* bounds,
        real32* location,
        void* output,
        void* ctx),
    int32(__cdecl* a4)(int32),
    int32 a5,
    int16 a6,
    bool a7);
