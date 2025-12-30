#include "stdafx.h"
#include "hud_draw.h"

#include "interface.h"

#include "bitmaps/bitmap_group.h"
#include "cache/pc_texture_cache.h"
#include "game/players.h"
#include "math/matrix_math.h"
#include "render/render_cameras.h"
#include "render/render.h"
#include "units/units.h"

/* public code */

void hud_render_player_indicator(datum player_index)
{
	const player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);
	
	// Get head position and offset it to get the indicator position
	real_point3d head_position;
	unit_get_head_position_interpolated(player->unit_index, &head_position);
	head_position.z += 0.3f;

	const s_render* render = render_get();

	real_point3d world_position;
	matrix4x3_transform_point(&render->projection.world_to_view, &head_position, &world_position);

	real_point2d screen_point;
	if (render_camera_world_to_screen(&render->camera, &render->projection, NULL, &world_position, &screen_point))
	{
		datum multiplayer_hud_bitmap_tag_index = interface_get_tag_index(_interface_multiplayer_hud_bitmap);
		bitmap_data* bitmap_from_sequence = bitmap_group_get_bitmap_from_sequence(multiplayer_hud_bitmap_tag_index, 0, 0);
		if (bitmap_from_sequence)
		{
			if (texture_cache_bitmap_get_hardware_format_add_to_texture_cache(bitmap_from_sequence, 0.f, 0))
			{
				screen_point.x -= render->camera.viewport_bounds.left;
				screen_point.y -= render->camera.viewport_bounds.top;

				real32 v4 = 1.f - (((-world_position.z - render->camera.z_near) * 100.f) / (render->camera.z_far - render->camera.z_near));
				v4 = PIN(v4, 0.075f, 1.f);

				real_rectangle2d rect;
				rect.x0 = 0.f;
				rect.x1 = 1.f;
				rect.y0 = (1.f - v4) * 0.5f;
				rect.y1 = 1.f;
				interface_draw_bitmap(bitmap_from_sequence, &screen_point, &rect, 1.f, 0.f, 1.f);
			}
		}
	}
	return;
}
