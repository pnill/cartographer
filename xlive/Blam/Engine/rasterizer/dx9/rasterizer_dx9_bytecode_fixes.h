#pragma once

/* prototypes */

// Hooks IDirect3DDevice9::CreatePixelShader so the stock pixel shaders of lightmap_tex_detail_alpha_test[_clamped] (per-vertex lighting
// implementation, used by foliage on scenery/objects) are repaired before they reach the driver: they declare texture stage 4 as a
// cube sampler (dcl_cube s4) although the shader templates bind a 2D bitmap there. Reading a 2D texture through a cube sampler is
// undefined in D3D9; Turing+ NVIDIA GPUs (RTX 20 series and newer, GTX 16 series) return zero alpha, so the alpha test discards the
// whole object and foliage disappears.
// The device must have been created. Safe to call more than once, only the first call installs the hook.
void rasterizer_dx9_bytecode_fixes_initialize(IDirect3DDevice9* device);
