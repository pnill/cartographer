#pragma once
#include "math/color_math.h"
#include "memory/hashtable.h"

/* structures */

#pragma pack(push, 1)
struct s_vertex_declaration
{
	int8 field_0;
	int8 field_1;
	int8 field_2;
	int8 field_3;
	int8 field_4[17];
};
#pragma pack(pop)

struct s_stream_input
{
	int32 offset_in_bytes;
	uint32 stride;
	IDirect3DVertexBuffer9* vb;
};

struct c_gpu_frontend
{
	int32 field_0;
	IDirect3DVertexShader9* m_vertex_shader;
	bool m_vertex_shader_assigned;
	int8 pad[3];
	s_vertex_declaration* m_vertex_declaration;
	int8 m_gap[28];
	int32 m_field_2C;
	D3DVERTEXELEMENT9 m_vertex_element;
	int8 m_gap1[4];
	s_stream_input stream_input[16];
	IDirect3DVertexDeclaration9* m_d3d_vertex_declaration;
	int8 field_104[8];
	int8 field_109[16];
	s_hash_table* m_hash_table;
	int32 field_11C;
};


/* prototypes */

void __cdecl rasterizer_dx9_gpu_frontend_initialize(void);

void __cdecl rasterizer_dx9_set_vertex_shader_permutation(int32 shader_index);

void __cdecl rasterizer_dx9_set_vertex_shader_permutation(int32 shader_index, int32 a2, int32 max_nodes_per_vertex);

void __cdecl rasterizer_dx9_draw_rect(real_rectangle2d* quad, real32 depth, pixel32 color);

bool __cdecl rasterizer_dx9_submit_vertex_declaration(uint32 vertex_declaration_index);

void __cdecl rasterizer_dx9_submit_resolve(void);

void __cdecl rasterizer_dx9_clear_staged_texture_cache(void);
