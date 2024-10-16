#include "stdafx.h"
#include "rasterizer_fog.h"

#include "render/render.h"
#include "render/render_layers.h"
#include "render/render_patchy_fog.h"
#include "rasterizer/dx9/rasterizer_dx9.h"
#include "rasterizer/dx9/rasterizer_dx9_fog.h"
#include "rasterizer/dx9/rasterizer_dx9_fullscreen_passes.h"

/* globals */


/* public code */

void render_atmospheric_fog(void)
{
	if (!render_layer_globals_get()->transparent_submit_in_progress)
	{
		rasterizer_dx9_perf_event_begin("atmospheric fog", NULL);

		const s_frame* global_window_parameters = global_window_parameters_get();
		const e_fog_mode fog_mode = global_window_parameters->fog_result.fog_mode;

		if (fog_mode == _fog_mode_atmospheric ||
			fog_mode == _fog_mode_planar_immersed ||
			fog_mode == _fog_mode_planar_immersed_secondary ||
			global_window_parameters->fog_result.planar_separate_mode)
		{
			rasterizer_dx9_render_fullscreen_overlay_geometry(
				0,
				rasterizer_dx9_atmospheric_fog_pipeline_setup,
				rasterizer_dx9_atmospheric_fog_build_vertex_buffer,
				0,
				0,
				4,
				true);
		}
		else if (fog_mode == _fog_mode_sky_only || fog_mode == _fog_mode_planar)
		{
			rasterizer_dx9_render_fullscreen_overlay_geometry(
				0,
				rasterizer_dx9_draw_fog_sky_only,
				rasterizer_dx9_sky_only_fog_build_vertex_buffer,
				0,
				0,
				0,
				true);	
		}

		rasterizer_dx9_perf_event_end("atmospheric fog");
	}
	return;
}

void render_patchy_fog(bool a1, bool a2)
{
	rasterizer_dx9_perf_event_begin("patchy fog", NULL);

	const s_frame* global_window_parameters = global_window_parameters_get();
	if (global_window_parameters->fog_result.patchy_fog_tag_index != NONE &&
		global_window_parameters->fog_result.patchy_fog_tag_index &&
		global_window_parameters->fog_result.field_88.upper > 0.f)
	{
		s_patchy_fog_globals* patchy_fog_globals = patchy_fog_globals_get();
		patchy_fog_globals->render_patchy_fog_parameters[0] = a1;
		patchy_fog_globals->render_patchy_fog_parameters[1] = a2;
		if (a1)
		{
			rasterizer_dx9_render_fullscreen_overlay_geometry(
				NULL,
				rasterizer_dx9_patchy_fog_composite_pipeline_setup,
				rasterizer_dx9_patchy_fog_composite_build_vertex_buffer,
				NULL,
				NULL,
				4,
				1);
			rasterizer_dx9_render_fullscreen_overlay_geometry(
				NULL,
				rasterizer_dx9_patchy_fog_apply_separate_layers_pipeline_setup,
				rasterizer_dx9_patchy_fog_apply_build_vertex_buffer,
				NULL,
				NULL,
				3,
				1);
		}
		else if (a2)
		{
			rasterizer_dx9_render_fullscreen_overlay_geometry(
				NULL,
				rasterizer_dx9_patchy_fog_apply_from_stencil_pipeline_setup,
				rasterizer_dx9_patchy_fog_apply_from_stencil_build_vertex_buffer,
				NULL,
				NULL,
				1,
				true);
		}
	}
	rasterizer_dx9_perf_event_end("patchy fog");
	return;
}
