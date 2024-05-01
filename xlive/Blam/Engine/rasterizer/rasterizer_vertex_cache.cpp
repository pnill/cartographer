#include "stdafx.h"

#include "rasterizer_vertex_cache.h"

/* prototypes */

c_rasterizer_constant_4f_cache* rasterizer_get_main_vertex_shader_cache();

/* definitions */

c_rasterizer_constant_4f_cache* rasterizer_get_main_vertex_shader_cache()
{
	return Memory::GetAddress< c_rasterizer_constant_4f_cache*>(0xA3C7B0);
}

bool c_rasterizer_constant_4f_cache::test_cache(int32 constant_index, real32* vertex_constants, int32 vertex_count_4f)
{
	return INVOKE_TYPE(0x4FF5B, 0x0, bool(__thiscall*)(c_rasterizer_constant_4f_cache*, int32, real32*, int32), this, constant_index, vertex_constants, vertex_count_4f);
}