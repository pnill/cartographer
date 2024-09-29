#pragma once

/* defines */

//#define D3D9_ON_12_ENABLED

/* prototypes */

#ifdef D3D9_ON_12_ENABLED

int32 rasterizer_dx9_create_through_d3d9on12(IDirect3D9Ex** d3d);

#endif
