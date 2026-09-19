#include "stdafx.h"
#include "antenna.h"

#include "bitmaps/bitmap_group.h"
#include "math/color_math.h"
#include "objects/objects.h"
#include "rasterizer/dx9/rasterizer_dx9_widgets.h"
#include "render/render.h"

/* public code */

void antenna_apply_patches(void)
{
	WritePointer(Memory::GetAddress(0x18F840, 0x0), antenna_render_proper);

	PatchCall(Memory::GetAddress(0x18F877, 0x0), object_get_markers_by_string_id);

	return;
}

data_array* antenna_data_get(void)
{
	return *Memory::GetAddress<data_array**>(0x4E66C4);
}

void __cdecl antenna_update(antenna_datum* antenna, const antenna_definition* definition, real32 dt)
{
	INVOKE(0x18FBFB, 0x0, antenna_update, antenna, definition, dt);
	return;
}

void __cdecl antenna_render_proper(datum object_index, datum antenna_index)
{
	antenna_datum* antenna = antenna_get(antenna_index);
	const antenna_definition* definition = antenna_definition_get(antenna->definition_index);

	ASSERT(antenna->object_index == object_index);

	if (definition->vertices.count <= 0)
	{
		return;
	}

	real32 alpha = (k_antenna_render_projected_pixels - definition->cutoff_pixels) / (definition->falloff_pixels - definition->cutoff_pixels);
	alpha = (alpha >= 0.f) ? MIN(1.f, alpha) : 0.f;

	if (antenna->updates_since_last_render > k_antenna_render_catch_up_update_threshold)
	{
		for (int32 i = 0; i < k_antenna_render_catch_up_update_count; i++)
		{
			antenna_update(antenna, definition, k_antenna_render_catch_up_dt);
		}
	}
	antenna->updates_since_last_render = 0;

	rasterizer_widget_render_setup(_rasterizer_widget_vertex_shader_transformed, _rasterizer_widget_blend_mode_alpha_blend, k_rasterizer_widget_flags_antenna);

	if (rasterizer_widget_set_texture(0, definition->bitmap.index, 0, k_rasterizer_widget_sampler_flags_antenna))
	{
		return;
	}

	const real_vector3d* camera_forward = &render_get()->camera.forward;

	for (int32 vertex_index = 0; vertex_index < definition->vertices.count; vertex_index++)
	{
		const antenna_vertex* vertex_definition = definition->vertices[vertex_index];
		const antenna_vertex_datum* vertex = &antenna->vertices[vertex_index];
		const antenna_vertex_datum* next_vertex = &antenna->vertices[vertex_index + 1];

		real_vector3d segment;
		vector_from_points3d(&vertex->position, &next_vertex->position, &segment);
		normalize3d(&segment);

		real_vector3d side;
		cross_product3d(camera_forward, &segment, &side);
		normalize3d(&side);

		real_argb_color color = vertex_definition->color;
		color.alpha = alpha;
		const pixel32 vertex_color = real_argb_color_to_pixel32(&color);

		const bitmap_group* bitmap_group = bitmap_group_get(definition->bitmap.index);
		ASSERT(bitmap_group);
		ASSERT(VALID_INDEX(vertex_definition->sequence_index, bitmap_group->sequences.count));
		const bitmap_group_sequence* sequence = bitmap_group->sequences[vertex_definition->sequence_index];
		ASSERT(sequence);

		const bitmap_group_sprite* sprite = sequence->sprites[0];
		const real32 bitmap_width = (real32)bitmap_group->bitmaps[0]->width;
		const real32 scale = bitmap_width * vertex->sprite_scale;

		const real32 left = sprite->bounds.x0;
		const real32 right = sprite->bounds.x1;
		const real32 top = sprite->bounds.y0;
		const real32 bottom = sprite->bounds.y1;

		const real32 along_start = -sprite->registrationPoint.x;
		const real32 along_end = (right - left) - sprite->registrationPoint.x;
		const real32 across_top = sprite->registrationPoint.y;
		const real32 across_bottom = (top - bottom) + sprite->registrationPoint.y;

		const real32 along[4] = { along_start, along_start, along_end, along_end };
		const real32 across[4] = { across_bottom, across_top, across_top, across_bottom };
		const real_point2d texcoords[4] = { { left, bottom }, { left, top }, { right, top }, { right, bottom } };

		real_point3d points[4];
		pixel32 colors[4];
		for (int32 corner = 0; corner < 4; corner++)
		{
			real_vector3d offset;
			scale_vector3d(&segment, along[corner], &offset);
			offset.i -= side.i * across[corner];
			offset.j -= side.j * across[corner];
			offset.k -= side.k * across[corner];
			scale_vector3d(&offset, scale, &offset);

			add_vectors3d((const real_vector3d*)&vertex->position, &offset, (real_vector3d*)&points[corner]);
			colors[corner] = vertex_color;
		}

		rasterizer_widget_draw_quad(points, texcoords, colors);
	}

	return;
}
