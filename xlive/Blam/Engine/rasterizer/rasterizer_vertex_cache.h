#pragma once

class c_rasterizer_constant_4f_cache
{
public:
	bool test_cache(int32 constant_index, real32* vertex_constants, int32 vertex_count_4f);
};

c_rasterizer_constant_4f_cache* rasterizer_get_main_vertex_shader_cache();
