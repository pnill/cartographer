#include "stdafx.h"
#include "rasterizer_dx9_widgets.h"

#include "rasterizer/dx9/rasterizer_dx9_main.h"

/* structures */

struct s_rasterizer_widget_vertex
{
	real_point3d position;
	real_point2d texcoord;
	pixel32 color;
};
ASSERT_STRUCT_SIZE(s_rasterizer_widget_vertex, 24);

/* public code */

void __cdecl rasterizer_dx9_create_widget_pixel_shaders(void)
{
	INVOKE(0x27987F, 0x0, rasterizer_dx9_create_widget_pixel_shaders);
	return;
}

void __cdecl rasterizer_widget_render_setup(int32 vertex_shader, int32 blend_mode, uint16 flags)
{
	INVOKE(0x279C2C, 0x0, rasterizer_widget_render_setup, vertex_shader, blend_mode, flags);
	return;
}

bool __cdecl rasterizer_widget_set_texture(int16 stage, datum bitmap_tag_index, int16 frame_index, uint8 sampler_flags)
{
	return INVOKE(0x27994D, 0x0, rasterizer_widget_set_texture, stage, bitmap_tag_index, frame_index, sampler_flags);
}

void rasterizer_widget_draw_quad(const real_point3d* points, const real_point2d* texcoords, const pixel32* colors)
{
	ASSERT(points);
	ASSERT(texcoords);
	ASSERT(colors);

	s_rasterizer_widget_vertex vertices[4];
	for (int32 i = 0; i < NUMBEROF(vertices); i++)
	{
		vertices[i].position = points[i];
		vertices[i].texcoord = texcoords[i];
		vertices[i].color = colors[i];
	}

	rasterizer_dx9_draw_primitive_up(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(s_rasterizer_widget_vertex));
	return;
}