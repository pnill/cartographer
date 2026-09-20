#include "stdafx.h"
#include "antenna.h"

#include "bitmaps/bitmap_group.h"
#include "main/interpolator.h"
#include "math/color_math.h"
#include "objects/objects.h"
#include "objects/widgets/widgets.h"
#include "rasterizer/dx9/rasterizer_dx9_widgets.h"
#include "render/render.h"

/* constants */

enum
{
	k_antenna_render_catch_up_update_threshold = 5,
	k_antenna_render_catch_up_update_count = 3,

	k_antenna_updates_per_second = 30,
};

constexpr real32 k_antenna_render_catch_up_dt = 0.05f;

constexpr real32 k_antenna_render_projected_pixels = 100.f;

constexpr real32 k_antenna_update_dt = 1.f / (real32)k_antenna_updates_per_second;

constexpr real32 k_antenna_maximum_update_dt = 2.f * k_antenna_update_dt;

/* structures */

struct s_antenna_interpolation_data
{
	datum antenna_index;
	real32 time_accumulator;
	real_vector3d previous_offsets[k_antenna_maximum_vertex_datum_count];
	real_vector3d target_offsets[k_antenna_maximum_vertex_datum_count];
};

/* prototypes */

static s_antenna_interpolation_data* antenna_interpolation_data_get(datum antenna_index);

static void antenna_interpolation_get_offsets(const antenna_datum* antenna, const antenna_definition* definition, real_vector3d* offsets);

static void antenna_interpolation_snap(datum antenna_index, const antenna_datum* antenna, const antenna_definition* definition);

static void antenna_interpolation_step(datum antenna_index, antenna_datum* antenna, const antenna_definition* definition, real32 dt);

static void antenna_get_render_positions(datum antenna_index, const antenna_datum* antenna, const antenna_definition* definition, real_point3d* positions);

/* globals */

static s_antenna_interpolation_data g_antenna_interpolation_data[k_maximum_antennas_per_map];

/* public code */

void antenna_apply_patches(void)
{
	WritePointer((uintptr_t)&widget_type_definition_get(_widget_type_antenna)->update, antennas_update);

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

void __cdecl antennas_update(real32 dt)
{
	data_array* antenna_array = antenna_data_get();

	for (datum antenna_index = data_next_index(antenna_array, NONE); antenna_index != NONE; antenna_index = data_next_index(antenna_array, antenna_index))
	{
		antenna_datum* antenna = antenna_get(antenna_index);
		const antenna_definition* definition = antenna_definition_get(antenna->definition_index);

		if (antenna->invalid)
		{
			continue;
		}

		antenna->updates_since_last_render++;

		if (antenna->object_index == NONE || antenna->updates_since_last_render >= k_antenna_render_catch_up_update_threshold)
		{
			continue;
		}

		if (!halo_frame_interpolator_enabled())
		{
			antenna_update(antenna, definition, MIN(dt, k_antenna_maximum_update_dt));
			continue;
		}

		s_antenna_interpolation_data* interpolation = antenna_interpolation_data_get(antenna_index);
		if (interpolation->antenna_index != antenna_index)
		{
			antenna_interpolation_snap(antenna_index, antenna, definition);
		}

		interpolation->time_accumulator = MIN(interpolation->time_accumulator + dt, k_antenna_maximum_update_dt);

		while (interpolation->time_accumulator >= k_antenna_update_dt)
		{
			interpolation->time_accumulator -= k_antenna_update_dt;
			antenna_interpolation_step(antenna_index, antenna, definition, k_antenna_update_dt);
		}
	}

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

		antenna_interpolation_snap(antenna_index, antenna, definition);
	}
	antenna->updates_since_last_render = 0;

	rasterizer_widget_render_setup(_rasterizer_widget_vertex_shader_transformed, _rasterizer_widget_blend_mode_alpha_blend, k_rasterizer_widget_flags_antenna);

	if (rasterizer_widget_set_texture(0, definition->bitmap.index, 0, k_rasterizer_widget_sampler_flags_antenna))
	{
		return;
	}

	real_point3d positions[k_antenna_maximum_vertex_datum_count];
	antenna_get_render_positions(antenna_index, antenna, definition, positions);

	const real_vector3d* camera_forward = &render_get()->camera.forward;

	for (int32 vertex_index = 0; vertex_index < definition->vertices.count; vertex_index++)
	{
		const antenna_vertex* vertex_definition = definition->vertices[vertex_index];
		const antenna_vertex_datum* vertex = &antenna->vertices[vertex_index];
		const real_point3d* position = &positions[vertex_index];
		const real_point3d* next_position = &positions[vertex_index + 1];

		real_vector3d segment;
		vector_from_points3d(position, next_position, &segment);
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

			add_vectors3d((const real_vector3d*)position, &offset, (real_vector3d*)&points[corner]);
			colors[corner] = vertex_color;
		}

		rasterizer_widget_draw_quad(points, texcoords, colors);
	}

	return;
}

/* private code */

static s_antenna_interpolation_data* antenna_interpolation_data_get(datum antenna_index)
{
	const int32 absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(antenna_index);
	ASSERT(VALID_INDEX(absolute_index, k_maximum_antennas_per_map));
	return &g_antenna_interpolation_data[absolute_index];
}

static void antenna_interpolation_get_offsets(const antenna_datum* antenna, const antenna_definition* definition, real_vector3d* offsets)
{
	for (int32 vertex_index = 0; vertex_index <= definition->vertices.count; vertex_index++)
	{
		vector_from_points3d(&antenna->last_attachment_location, &antenna->vertices[vertex_index].position, &offsets[vertex_index]);
	}
}

static void antenna_interpolation_snap(datum antenna_index, const antenna_datum* antenna, const antenna_definition* definition)
{
	s_antenna_interpolation_data* interpolation = antenna_interpolation_data_get(antenna_index);

	interpolation->antenna_index = antenna_index;
	interpolation->time_accumulator = 0.f;
	antenna_interpolation_get_offsets(antenna, definition, interpolation->target_offsets);
	csmemcpy(interpolation->previous_offsets, interpolation->target_offsets, sizeof(interpolation->previous_offsets));
}

static void antenna_interpolation_step(datum antenna_index, antenna_datum* antenna, const antenna_definition* definition, real32 dt)
{
	s_antenna_interpolation_data* interpolation = antenna_interpolation_data_get(antenna_index);

	csmemcpy(interpolation->previous_offsets, interpolation->target_offsets, sizeof(interpolation->previous_offsets));

	antenna_update(antenna, definition, dt);

	antenna_interpolation_get_offsets(antenna, definition, interpolation->target_offsets);
}

static void antenna_get_render_positions(datum antenna_index, const antenna_datum* antenna, const antenna_definition* definition, real_point3d* positions)
{
	const s_antenna_interpolation_data* interpolation = antenna_interpolation_data_get(antenna_index);

	if (!halo_frame_interpolator_enabled() || interpolation->antenna_index != antenna_index)
	{
		for (int32 vertex_index = 0; vertex_index <= definition->vertices.count; vertex_index++)
		{
			positions[vertex_index] = antenna->vertices[vertex_index].position;
		}
	}
	else
	{
		real_point3d attachment_point = antenna->last_attachment_location;
		object_marker marker;
		if (object_get_markers_by_string_id(antenna->object_index, definition->marker_name, &marker, 1) > 0)
		{
			attachment_point = marker.matrix.position;
		}

		const real32 t = PIN(interpolation->time_accumulator / k_antenna_update_dt, 0.f, 1.f);

		for (int32 vertex_index = 0; vertex_index <= definition->vertices.count; vertex_index++)
		{
			real_vector3d offset;
			vectors_interpolate(&interpolation->previous_offsets[vertex_index], &interpolation->target_offsets[vertex_index], t, &offset);
			add_vectors3d((const real_vector3d*)&attachment_point, &offset, (real_vector3d*)&positions[vertex_index]);
		}
	}
}
