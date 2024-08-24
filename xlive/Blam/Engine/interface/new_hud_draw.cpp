#include "stdafx.h"
#include "new_hud_draw.h"

#include "hud.h"
#include "new_hud.h"
#include "new_hud_definitions.h"
#include "bitmaps/bitmap_group.h"
#include "game/players.h"
#include "render/render.h"
#include "saved_games/cartographer_player_profile.h"


/* private code */

s_draw_hud_bitmap_widget_input_results* global_hud_draw_widget_function_results_get()
{
	return Memory::GetAddress<s_draw_hud_bitmap_widget_input_results*>(0x976680);
}

real_rgb_color* global_hud_draw_widget_special_hud_type_color_primary_get()
{
	return Memory::GetAddress<real_rgb_color*>(0x976690);
}
real_rgb_color* global_hud_draw_widget_special_hud_type_secondary_color_get()
{
	return Memory::GetAddress<real_rgb_color*>(0x97669C);
}
real_rgb_color* global_hud_draw_widget_special_hud_type_tertiary_color_get()
{
	return Memory::GetAddress<real_rgb_color*>(0x9766A8);
}

void draw_hud_get_bitmap_data(uint32 local_render_user_index, s_hud_bitmap_widget_definition* bitmap_widget, real_rectangle2d* bounds, uint32* out_bitmap_index, uint32* out_width_pixels, uint32* out_height_pixels)
{
	ASSERT(bitmap_widget);
	ASSERT(bounds);
	ASSERT(out_bitmap_index);
	ASSERT(out_width_pixels);
	ASSERT(out_height_pixels);

	// If returned bitmap index is NONE the draw call will exit early.
	*out_bitmap_index = NONE;

	int32 sequence_index = 0;

	switch (get_screen_split_type(local_render_user_index))
	{
	case _screen_split_type_full:
		sequence_index = bitmap_widget->fullscreen_sequence_index;
		break;
	case _screen_split_type_half:
		sequence_index = bitmap_widget->halfscreen_sequence_index;
		break;
	case _screen_split_type_quarter:
		sequence_index = bitmap_widget->quarterscreen_sequence_index;
		break;
	default:
		sequence_index = bitmap_widget->fullscreen_sequence_index;
	}

	if (bitmap_widget->shader.index == NONE || bitmap_widget->bitmap.index == NONE || sequence_index < 0)
		return;

	bitmap_group* bitmap = (bitmap_group*)tag_get_fast(bitmap_widget->bitmap.index);

	if (sequence_index >= bitmap->sequences.count)
	{
		if (sequence_index || bitmap->sequences.count)
			return;

		bitmap_data* bitmap_data = bitmap->bitmaps[0];

		*out_width_pixels = bitmap_data->width_pixels;
		*out_height_pixels = bitmap_data->height_pixels;
		*out_bitmap_index = 0;

		bounds->y1 = 1.f;
		bounds->x1 = 1.f;
		bounds->y0 = 0;
		bounds->x0 = 0;

		return;
	}

	bitmap_group_sequence* bitmap_sequence = bitmap->sequences[sequence_index];

	if (bitmap_sequence->sprites.count <= 0)
	{
		if (bitmap_sequence->bitmap_count <= 0)
			return;

		int32 bitmap_index = bitmap_sequence->first_bitmap_index;
		if (bitmap_sequence->bitmap_count > 1 && player_unk_84_from_user_index(local_render_user_index))
			bitmap_index += 1;

		*out_width_pixels = bitmap->bitmaps[bitmap_index]->width_pixels;
		*out_height_pixels = bitmap->bitmaps[bitmap_index]->height_pixels;
		*out_bitmap_index = bitmap_index;
		bounds->y1 = 1.f;
		bounds->x1 = 1.f;
		bounds->y0 = 0;
		bounds->x0 = 0;

		return;
	}

	int32 sprite_index = 0;
	if (bitmap_sequence->sprites.count > 1 && player_unk_84_from_user_index(local_render_user_index))
		sprite_index = 1;

	bitmap_group_sprite* bitmap_sprite = bitmap_sequence->sprites[sprite_index];
	bitmap_data* bitmap_data = bitmap->bitmaps[bitmap_sprite->bitmapIndex];

	*out_width_pixels = bitmap_data->width_pixels;
	*out_height_pixels = bitmap_data->height_pixels;
	*out_bitmap_index = bitmap_sprite->bitmapIndex;
	bounds->y0 = bitmap_sprite->bounds.y0;
	bounds->y1 = bitmap_sprite->bounds.y1;
	bounds->x0 = bitmap_sprite->bounds.x0;
	bounds->x1 = bitmap_sprite->bounds.x1;
}

real32 __cdecl hud_draw_widget_effect_get_value(uint32 unused, string_id input_name)
{
	return INVOKE(0x22211A, 0, hud_draw_widget_effect_get_value, unused, input_name);
}

void hud_widget_effect_evaluate(uint32 local_render_user_index, s_new_hud_temporary_user_state* user_state, s_hud_widget_effect_definition* widget_effect, real_point2d* out_offset, real_point2d* out_scale, real32* out_theta)
{
	if (out_theta && widget_effect->flags.test(hud_widget_effect_flag_apply_theta))
	{
		const real32 theta_value = hud_draw_widget_effect_get_value(NONE, widget_effect->theta.input_name);
		const real32 theta_result = widget_effect->theta.function.evaluate(theta_value, 1.f);
		*out_theta += widget_effect->theta.function.unknown_post_evaluate_function(theta_result);
	}
	if (out_offset && widget_effect->flags.test(hud_widget_effect_flag_apply_offset))
	{
		const real32 horizontal_value = hud_draw_widget_effect_get_value(NONE, widget_effect->horizontal_offset.input_name);
		const real32 vertical_value = hud_draw_widget_effect_get_value(NONE, widget_effect->vertical_offset.input_name);

		const real32 horizontal_result = widget_effect->horizontal_offset.function.evaluate(horizontal_value, 1.f);
		const real32 vertical_result = widget_effect->vertical_offset.function.evaluate(vertical_value, 1.f);

		out_offset->x += out_scale->x * widget_effect->horizontal_offset.function.unknown_post_evaluate_function(horizontal_result);
		out_offset->y += out_scale->y * widget_effect->vertical_offset.function.unknown_post_evaluate_function(vertical_result);
	}
	if (widget_effect->flags.test(hud_widget_effect_flag_apply_scale))
	{
		const real32 horizontal_value = hud_draw_widget_effect_get_value(NONE, widget_effect->horizontal_scale.input_name);
		const real32 vertical_value = hud_draw_widget_effect_get_value(NONE, widget_effect->vertical_scale.input_name);

		const real32 horizontal_result = widget_effect->horizontal_scale.function.evaluate(horizontal_value, 1.f);
		const real32 vertical_result = widget_effect->vertical_scale.function.evaluate(vertical_value, 1.f);

		out_scale->x *= widget_effect->horizontal_scale.function.unknown_post_evaluate_function(horizontal_result);
		out_scale->y *= widget_effect->vertical_scale.function.unknown_post_evaluate_function(vertical_result);
	}
}

void hud_widget_anchor_calculate_point(e_hud_anchor anchor, real_point2d* out_point)
{
	INVOKE(0x223969, 0, hud_widget_anchor_calculate_point, anchor, out_point);
}

static const pixel32 g_draw_hud_bitmap_widget_shield_pixel_colors[9]
{
	{0},
	{0xFF0000},
	{0xFF00},
	{0xFFFF00},
	{0x007F00},
	{0x45059A},
	{0x9C46C1},
	{0x55AA},
	{0x78F0}
};

void __cdecl draw_hud_bitmap_widget(uint32 local_render_user_index, s_new_hud_temporary_user_state* user_state, s_hud_bitmap_widget_definition* bitmap_widget, int32* widget_function_results)
{
	if (bitmap_widget->bitmap.index == NONE || bitmap_widget->shader.index == NONE)
		return;

	real_rectangle2d bitmap_bounds{};
	uint32 bitmap_index = -1;
	uint32 bitmap_width = 0;
	uint32 bitmap_height = 0;

	draw_hud_get_bitmap_data(local_render_user_index, bitmap_widget, &bitmap_bounds, &bitmap_index, &bitmap_width, &bitmap_height);

	if (bitmap_index == NONE)
		return;

	s_draw_hud_bitmap_widget_input_results* hud_input_results = global_hud_draw_widget_function_results_get();

	memcpy(hud_input_results, widget_function_results, sizeof(int32) * 4);

	real_point2d offset_result{ 0, 0 };
	real_point2d scale_result{ 1.f, 1.f };
	real32 theta_result = 0;

	if (bitmap_widget->effect.count > 0)
		hud_widget_effect_evaluate(local_render_user_index, user_state, bitmap_widget->effect[0], &offset_result, &scale_result, &theta_result);

	bitmap_width = (bitmap_width * scale_result.x);
	bitmap_height = (bitmap_height * scale_result.y);

	real32 hud_scale = *get_primary_hud_scale();

	if (bitmap_widget->anchor == _hud_anchor_crosshair)
	{
		s_saved_game_cartographer_player_profile_v1* profile_settings = cartographer_player_profile_get_by_user_index(local_render_user_index);
		if (bitmap_widget->widget_state.yes_weapon_flags.test(widget_state_weapon_flag_primary_weapon) ||
			bitmap_widget->widget_state.yes_weapon_flags.test(widget_state_weapon_flag_secondary_weapon))
		{
			hud_scale = *get_secondary_hud_scale() * profile_settings->crosshair_scale;
		}
		else
			hud_scale = *get_secondary_hud_scale();
	}

	real_point2d anchor_point{};

	hud_widget_anchor_calculate_point(bitmap_widget->anchor, &anchor_point);

	point2d screen_offset;
	real_point2d registration_point;

	switch (get_screen_split_type(local_render_user_index))
	{
	case _screen_split_type_half:
		screen_offset = bitmap_widget->halfscreen_offset;
		registration_point = bitmap_widget->halfscreen_registration_point;
		break;
	case _screen_split_type_quarter:
		screen_offset = bitmap_widget->quarterscreen_offset;
		registration_point = bitmap_widget->quarterscreen_registration_point;
		break;
	default:
	case _screen_split_type_full:
		screen_offset = bitmap_widget->fullscreen_offset;
		registration_point = bitmap_widget->fullscreen_registration_point;
		break;
	}

	real32 calc_registration_x = ((bitmap_bounds.x1 - bitmap_bounds.x0) * registration_point.x) * (float)bitmap_width;
	real32 calc_offset_x = screen_offset.x + offset_result.x;

	real32 calc_registration_y = ((bitmap_bounds.y1 - bitmap_bounds.y0) * registration_point.y) * (float)bitmap_height;
	real32 calc_offset_y = screen_offset.y + offset_result.y;

	real32 final_point_x = (((calc_offset_x - calc_registration_x) + bitmap_bounds.x0) * hud_scale) + anchor_point.x;
	real32 final_point_y = (((calc_offset_y - calc_registration_y) + bitmap_bounds.y0) * hud_scale) + anchor_point.y;

	real_point2d final_location{ final_point_x, final_point_y };
	real32 bitmap_size[2]{ (float)bitmap_width, (float)bitmap_height };

	if (bitmap_widget->flags.test(bitmap_widget_flag_flip_horizontally))
	{
		real32 bounds_left = bitmap_bounds.x0;
		bitmap_bounds.x0 = bitmap_bounds.x1;
		bitmap_bounds.x1 = bounds_left;
	}

	if (bitmap_widget->flags.test(bitmap_widget_flag_flip_vertically))
	{
		real32 bounds_top = bitmap_bounds.y0;
		bitmap_bounds.y0 = bitmap_bounds.y1;
		bitmap_bounds.y1 = bounds_top;
	}

	bool special_draw_case = false;

	switch (bitmap_widget->special_hud_type)
	{
	case special_hud_type_sb_player_emblem:
		new_hud_engine_globals_set_drawing_player_index(user_state->local_player_datum);
		*global_hud_draw_widget_special_hud_type_color_primary_get() = user_state->local_player_color;
		break;
	case special_hud_type_sb_other_player_emblem:
		new_hud_engine_globals_set_drawing_player_index(user_state->other_player_datum);
		*global_hud_draw_widget_special_hud_type_color_primary_get() = user_state->other_player_color;
		break;
	case special_hud_type_sb_player_score_meter:
		*global_hud_draw_widget_special_hud_type_color_primary_get() = user_state->local_player_color;
		*global_hud_draw_widget_special_hud_type_secondary_color_get() = user_state->local_player_color;
		break;
	case special_hud_type_sb_other_player_score_meter:
		*global_hud_draw_widget_special_hud_type_color_primary_get() = user_state->other_player_color;
		*global_hud_draw_widget_special_hud_type_secondary_color_get() = user_state->other_player_color;
		break;
	case special_hud_type_unit_shield_meter:
	{
		special_draw_case = true;

		s_new_hud_globals_player_info* player_info = new_hud_engine_globals_get_player_data(local_render_user_index);
		int32 shield_layer_level = 0;

		while (true)
		{
			real32 shield_vitality = user_state->unit_current_shield_vitality - (float)shield_layer_level;

			if (shield_vitality >= 0.f)
				shield_vitality = shield_vitality <= 1.f ? shield_vitality : 1.f;
			else
				shield_vitality = 0;

			real32 player_unk_0 = player_info->unk_0 - (float)shield_layer_level;

			if (shield_vitality >= 0.f)
				player_unk_0 = player_unk_0 <= 1.f ? player_unk_0 : 1.f;
			else
				player_unk_0 = 0.f;

			bool unk_bool = false;
			real32 color_scale_something = player_info->unk_4;

			if (player_unk_0 <= shield_vitality)
			{
				unk_bool = false;
				color_scale_something = 0.f;
			}
			else
			{
				unk_bool = true;
				if (color_scale_something >= 0.f)
					color_scale_something = color_scale_something <= 1.f ? color_scale_something : 1.f;
				else
					color_scale_something = 0.f;
			}

			real_rgb_color shield_color;
			shield_color.red = 1.f * color_scale_something;
			shield_color.green = 1.f * color_scale_something;
			shield_color.blue = 1.f * color_scale_something;

			if (!unk_bool)
				player_unk_0 = shield_vitality;

			if (player_unk_0 <= 0 && shield_vitality <= 0)
				break;

			global_hud_draw_widget_function_results_get()->result_1 = player_unk_0;
			global_hud_draw_widget_function_results_get()->result_2 = shield_vitality;
			*global_hud_draw_widget_special_hud_type_color_primary_get() = shield_color;

			if (shield_layer_level)
			{
				pixel32_to_real_rgb_color(g_draw_hud_bitmap_widget_shield_pixel_colors[shield_layer_level], global_hud_draw_widget_special_hud_type_secondary_color_get());
				pixel32_to_real_rgb_color(g_draw_hud_bitmap_widget_shield_pixel_colors[shield_layer_level], global_hud_draw_widget_special_hud_type_tertiary_color_get());
			}
			else if (player_unk_84_from_user_index(local_render_user_index))
			{
				pixel32_to_real_rgb_color(g_draw_hud_bitmap_widget_shield_pixel_colors[5], global_hud_draw_widget_special_hud_type_secondary_color_get());
				pixel32_to_real_rgb_color(g_draw_hud_bitmap_widget_shield_pixel_colors[6], global_hud_draw_widget_special_hud_type_tertiary_color_get());
			}
			else
			{
				pixel32_to_real_rgb_color(g_draw_hud_bitmap_widget_shield_pixel_colors[7], global_hud_draw_widget_special_hud_type_secondary_color_get());
				pixel32_to_real_rgb_color(g_draw_hud_bitmap_widget_shield_pixel_colors[8], global_hud_draw_widget_special_hud_type_tertiary_color_get());
			}

			draw_ingame_user_interface_element_hook(
				final_location.x,
				final_location.y,
				bitmap_width,
				bitmap_height,
				hud_scale,
				theta_result,
				bitmap_widget->bitmap.index,
				bitmap_index,
				&bitmap_bounds,
				bitmap_widget->shader.index);

			if (++shield_layer_level > 4)
				break;
		}
		break;
	}
	case special_hud_type_territory_meter:
	{
		special_draw_case = true;

		if (user_state->territories_count == 0)
			return;

		*global_hud_draw_widget_special_hud_type_secondary_color_get() = *global_real_rgb_white;

		real32 distance_per_territory = ((float)bitmap_width + 1) * hud_scale;
		real32 base_location = final_location.x - ((user_state->territories_count - 1) * (distance_per_territory * 0.5f));

		for (int32 index = 0; index < user_state->territories_count; ++index)
		{
			pixel32 territory_color = user_state->territory_pixel_color[index];
			pixel32_to_real_rgb_color(territory_color, global_hud_draw_widget_special_hud_type_color_primary_get());

			real32 territory_progress = user_state->territory_control_progress[index];
			global_hud_draw_widget_function_results_get()->result_1 = territory_progress;

			real32 adjusted_location_x = base_location + distance_per_territory * index;

			draw_ingame_user_interface_element_hook(
				adjusted_location_x,
				final_location.y,
				bitmap_width,
				bitmap_height,
				hud_scale,
				theta_result,
				bitmap_widget->bitmap.index,
				bitmap_index,
				&bitmap_bounds,
				bitmap_widget->shader.index);
		}

		break;
	}
	default:
		break;
	}


	if (!special_draw_case)
	{
		if (!bitmap_widget->flags.test(bitmap_widget_flag_scope_stretch)) {
			draw_ingame_user_interface_element_hook(
				final_location.x,
				final_location.y,
				bitmap_width,
				bitmap_height,
				hud_scale,
				theta_result,
				bitmap_widget->bitmap.index,
				bitmap_index,
				&bitmap_bounds,
				bitmap_widget->shader.index);
		}
		else
		{
			real_point2d stretched_location;
			final_location.x = final_location.x - ((bitmap_size[0] * hud_scale) * 9.f);
			final_location.y = final_location.y - ((bitmap_size[1] * hud_scale) * 9.f);

			real_rectangle2d stretched_bounds;
			bitmap_bounds.y0 = bitmap_bounds.y0 - ((bitmap_bounds.y1 - bitmap_bounds.y0) * 9.f);
			bitmap_bounds.x0 = bitmap_bounds.x0 - ((bitmap_bounds.x1 - bitmap_bounds.x0) * 9.f);
			stretched_bounds.y1 = bitmap_bounds.y1;
			stretched_bounds.x1 = bitmap_bounds.x1;

			hud_scale = hud_scale * 10;

			draw_ingame_user_interface_element_hook(
				final_location.x,
				final_location.y,
				bitmap_width,
				bitmap_height,
				hud_scale,
				theta_result,
				bitmap_widget->bitmap.index,
				bitmap_index,
				&bitmap_bounds,
				bitmap_widget->shader.index);
		}
		if (bitmap_widget->flags.test(bitmap_widget_flag_scope_mirror_horizontally))
		{
			real_rectangle2d flipped_bounds{};
			flipped_bounds.y0 = bitmap_bounds.y0;
			flipped_bounds.y1 = bitmap_bounds.y1;
			flipped_bounds.x0 = bitmap_bounds.x1;
			flipped_bounds.x1 = bitmap_bounds.x0;

			draw_ingame_user_interface_element_hook(
				bitmap_size[0] * hud_scale + final_location.x,
				final_location.y,
				bitmap_width,
				bitmap_height,
				hud_scale,
				theta_result,
				bitmap_widget->bitmap.index,
				bitmap_index,
				&flipped_bounds,
				bitmap_widget->shader.index);
		}
		if (bitmap_widget->flags.test(bitmap_widget_flag_scope_mirror_vertically))
		{
			real_rectangle2d flipped_bounds{};

			flipped_bounds.y0 = bitmap_bounds.y1;
			flipped_bounds.y1 = bitmap_bounds.y0;
			flipped_bounds.x0 = bitmap_bounds.x0;
			flipped_bounds.x1 = bitmap_bounds.x1;


			draw_ingame_user_interface_element_hook(
				final_location.x,
				bitmap_size[1] * hud_scale + final_location.y,
				bitmap_width,
				bitmap_height,
				hud_scale,
				theta_result,
				bitmap_widget->bitmap.index,
				bitmap_index,
				&flipped_bounds,
				bitmap_widget->shader.index);
		}
		if (bitmap_widget->flags.test(bitmap_widget_flag_scope_mirror_horizontally) &&
			bitmap_widget->flags.test(bitmap_widget_flag_scope_mirror_vertically))
		{
			real_rectangle2d flipped_bounds{};
			flipped_bounds.y0 = bitmap_bounds.y1;
			flipped_bounds.y1 = bitmap_bounds.y0;
			flipped_bounds.x0 = bitmap_bounds.x1;
			flipped_bounds.x1 = bitmap_bounds.x0;
			draw_ingame_user_interface_element_hook(
				bitmap_size[0] * hud_scale + final_location.x,
				bitmap_size[1] * hud_scale + final_location.y,
				bitmap_width,
				bitmap_height,
				hud_scale,
				theta_result,
				bitmap_widget->bitmap.index,
				bitmap_index,
				&flipped_bounds,
				bitmap_widget->shader.index);
		}
	}
}

/* public code */

datum hud_bitmap_tag_index_get(void)
{
	return *Memory::GetAddress<datum*>(0x976678);
}

int32 hud_bitmap_data_index_get(void)
{
	return *Memory::GetAddress<int32*>(0x97667C);
}

void __cdecl draw_hud_layer(void)
{
	INVOKE(0x22657B, 0x0, draw_hud_layer);
	return;
}

//void __cdecl draw_hud_bitmap_widget(int player_render_state, s_new_hud_temporary_user_state* user_state, s_hud_bitmap_widget_definition* definition, int* a4)
//{
//	INVOKE(0x222E9B, 0, draw_hud_bitmap_widget, player_render_state, user_state, definition, a4);
//}

void new_hud_draw_apply_patches()
{
	//PatchCall(Memory::GetAddress(0x222EE2), draw_hud_get_bitmap_data_usercall_to_rewritten);
	PatchCall(Memory::GetAddress(0x224F46), draw_hud_bitmap_widget);
}
