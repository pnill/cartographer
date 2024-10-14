#pragma once

/* defines */

//#define D3D9_ON_12_ENABLED

/* prototypes */

#ifdef D3D9_ON_12_ENABLED

#include <d3d9on12.h>

int32 rasterizer_dx9_create_through_d3d9on12(IDirect3D9Ex** d3d, ID3D12Device** d3d12, bool use_warp);

#endif
