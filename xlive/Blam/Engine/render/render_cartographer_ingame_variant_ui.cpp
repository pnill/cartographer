#include "stdafx.h"
#include "render_cartographer_ingame_variant_ui.h"

#include "cache/cache_files.h"
#include "interface/cartographer_variant_settings.h"
#include "interface/hud.h"
#include "interface/multiplayer_variant_settings_interface_definition.h"
#include "main/game_preferences.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_text.h"
#include "rasterizer/dx9/rasterizer_dx9_primitives.h"
#include "text/draw_string.h"
#include "text/font_cache.h"
#include "text/text_group.h"

/* constants */

enum : uint32
{
	k_variant_ui_body_font = 0,
	k_variant_ui_header_font = 10,
	k_variant_ui_text_padding = 5,

	k_variant_ui_header_color = PIXEL32_ARGB(128, 0, 0, 0),
	k_variant_ui_body_color = PIXEL32_ARGB(64, 0, 0, 0),

	k_variant_ui_body_column_count = 3,
	k_variant_ui_body_row_count = 2,
	k_variant_ui_body_cell_count = k_variant_ui_body_column_count * k_variant_ui_body_row_count
};

static const wchar_t* g_variant_ui_headers[k_language_count][k_variant_ui_body_cell_count] = 
{
	{
			L"Match",
			L"Team",
			L"Equipment/Vehicle",
			L"Game Type",
			L"Cartographer",
			L"Players",
	},
	{
			L"試合",
			L"チーム",
			L"装備/車両",
			L"ゲームタイプ",
			L"Cartographer",
			L"プレイヤー",
	},
	{
			L"Match",
			L"Team",
			L"Ausrüstung/Fahrzeug",
			L"Spieltyp",
			L"Cartographer",
			L"Spieler",
	},
	{
			L"Partie",
			L"Équipe",
			L"Équipement/Véhicule",
			L"Type de partie",
			L"Cartographer",
			L"Joueurs",
	},
	{
			L"Partida",
			L"Equipo",
			L"Equipamiento/Vehículo",
			L"Tipo de juego",
			L"Cartographer",
			L"Jugadores",
	},
	{
			L"Partita",
			L"Squadra",
			L"Equipaggiamento/Veicolo",
			L"Tipo di gioco",
			L"Cartographer",
			L"Giocatori",
	},
	{
			L"경기",
			L"팀",
			L"장비/차량",
			L"게임 유형",
			L"Cartographer",
			L"플레이어",
	},
	{
			L"比赛",
			L"团队",
			L"装备/载具",
			L"游戏类型",
			L"Cartographer",
			L"玩家",
	},
	{
			L"Partida",
			L"Equipe",
			L"Equipamento/Veículo",
			L"Tipo de jogo",
			L"Cartographer",
			L"Jogadores",
	},
};

/* prototypes */

static real32 cartographer_ingame_variant_ui_fade_in_scale();
static pixel32 cartographer_ingame_variant_ui_body_fade_in(pixel32 pixel);
static const real_argb_color* cartographer_ingame_variant_ui_text_fade_in(real_argb_color* out_color, const real_argb_color* base_color);
static void cartographer_ingame_variant_ui_draw_setting_string(rectangle2d* draw_bounds, int16 line_height, wchar_t const* string);

/* globals */

static real_argb_color g_cartographer_variant_ui_text_color;
static real_argb_color g_cartographer_variant_ui_text_shadow_color;

/* public code */

void cartographer_ingame_variant_ui_render()
{
	if (g_cartographer_variant_settings_render_globals.render)
	{
		s_game_variant* variant = get_game_variant();

		ASSERT(variant);

		wchar_t string_buffer[256];

		const int16 header_line_height = get_text_size_from_font_cache(k_variant_ui_header_font);
		const int16 body_line_height = get_text_size_from_font_cache(k_variant_ui_body_font);

		cartographer_ingame_variant_ui_text_fade_in(&g_cartographer_variant_ui_text_color, global_real_argb_white);
		cartographer_ingame_variant_ui_text_fade_in(&g_cartographer_variant_ui_text_shadow_color, global_real_argb_black);

		rectangle2d frame_bounds;

		rasterizer_get_frame_bounds(&frame_bounds);

		// maybe add safe area respecting padding?
		const rectangle2d header_rect = {
			75,
			20,
			(int16)(80 + header_line_height),
			(int16)(frame_bounds.right - 20)
		};
		const rectangle2d body_rect =
		{
			header_rect.bottom,
			header_rect.left,
			(int16)(frame_bounds.bottom - 20),
			header_rect.right
		};

		rasterizer_dx9_draw_primitive_quad(&header_rect, cartographer_ingame_variant_ui_body_fade_in(k_variant_ui_header_color));

		draw_string_reset();
		draw_string_set_draw_mode(k_variant_ui_header_font, 0, 2, 1, &g_cartographer_variant_ui_text_color, &g_cartographer_variant_ui_text_shadow_color, false);

		rectangle2d header_text_bounds = header_rect;
		header_text_bounds.top += k_variant_ui_text_padding;
		header_text_bounds.bottom += header_line_height + k_variant_ui_text_padding;

		swprintf_s(string_buffer, L"%s Settings", variant->variant_name);

		rasterizer_draw_unicode_string(&header_text_bounds, string_buffer);

		const uint16 body_column_width = (body_rect.right - body_rect.left) / k_variant_ui_body_column_count;
		const uint16 body_row_height = (body_rect.bottom - body_rect.top) / k_variant_ui_body_row_count;

		int16 current_cell_index = 0;

		const e_variant_setting_category_type variant_categories[k_variant_ui_body_cell_count]
		{
			(e_variant_setting_category_type)(_variant_setting_category_type_match_ctf + variant->variant_game_engine_index - 1),
			(e_variant_setting_category_type)(_variant_setting_category_type_team_ctf + variant->variant_game_engine_index - 1),
			_variant_setting_category_type_equipment,
			(e_variant_setting_category_type)(_variant_setting_category_type_game_ctf + variant->variant_game_engine_index - 1),
			_variant_setting_category_type_cartographer_settings,
			_variant_setting_category_type_players
		};

		for (uint16 column = 0; column < k_variant_ui_body_column_count; ++column)
		{
			int16 current_column_left = body_rect.left + (body_column_width * column);
			int16 current_column_right = current_column_left + body_column_width;

			for (uint16 row = 0; row < k_variant_ui_body_row_count; ++row)
			{
				int16 current_row_top = body_rect.top + (body_row_height * row);
				int16 current_row_bottom = current_row_top + body_row_height;

				rectangle2d cell_rect =
				{
				current_row_top,
				current_column_left,
				current_row_bottom,
				current_column_right
				};

				rectangle2d cell_header_rect =
				{
					cell_rect.top,
					cell_rect.left,
					(int16)(cell_rect.top + header_line_height),
					cell_rect.right
				};

				rectangle2d cell_body_rect =
				{
					cell_header_rect.bottom,
					cell_rect.left,
					cell_rect.bottom,
					cell_rect.right
				};

				rasterizer_dx9_draw_primitive_quad(&cell_header_rect, cartographer_ingame_variant_ui_body_fade_in(k_variant_ui_header_color));
				rasterizer_dx9_draw_primitive_quad(&cell_body_rect, cartographer_ingame_variant_ui_body_fade_in(k_variant_ui_body_color));

				draw_string_reset();
				draw_string_set_draw_mode(k_variant_ui_header_font, 0, 2, 0, &g_cartographer_variant_ui_text_color, &g_cartographer_variant_ui_text_shadow_color, false);

				swprintf_s(string_buffer, L"%s", g_variant_ui_headers[get_current_language()][current_cell_index]);
				rasterizer_draw_unicode_string(&cell_header_rect, string_buffer);

				draw_string_reset();
				draw_string_set_draw_mode(k_variant_ui_body_font, 0, 0, FLAG(_draw_string_wrap_vertically), &g_cartographer_variant_ui_text_color, &g_cartographer_variant_ui_text_shadow_color, false);

				cell_body_rect.left += k_variant_ui_text_padding;

				// grab the category reference if the return is null it is a custom category.
				s_variant_setting_edit_reference* category_reference = multiplayer_variant_settings_interface_get_category_reference(variant_categories[current_cell_index]);

				if (category_reference)
				{
					for (int32 i = 0; i < category_reference->options.count; ++i)
					{
						s_text_value_pair_definition* text_pair = text_value_pair_definition_get(category_reference->options[i]->index);

						int32 setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, text_pair->parameter);
						s_text_value_pair_reference_new* setting_label = multiplayer_variant_settings_interface_get_variant_parameter_label(text_pair, setting_value);

						c_maximum_interface_text label_buffer;
						c_maximum_interface_text value_buffer;

						string_list_get_normal_string(text_pair->string_list.index, text_pair->title_text, &label_buffer);

						// format the variant parameter text, if the setting_label is nullptr that means it is a
						// integer parsing parameter just print the int
						if (setting_label)
						{
							string_list_get_normal_string(text_pair->string_list.index, setting_label->label_string, &value_buffer);
							swprintf_s(string_buffer, L"%s: %s", label_buffer.get_string(), value_buffer.get_string());
						}
						else
						{
							swprintf_s(string_buffer, L"%s: %d", label_buffer.get_string(), setting_value);
						}

						cartographer_ingame_variant_ui_draw_setting_string(&cell_body_rect, body_line_height, string_buffer);
					}

					// merge the equipment and vehicle categories so we can keep the 3x2 grid size
					if (variant_categories[current_cell_index] == _variant_setting_category_type_equipment)
					{
						category_reference = multiplayer_variant_settings_interface_get_category_reference(_variant_setting_category_type_vehicles);
						if (category_reference)
						{
							for (int32 i = 0; i < category_reference->options.count; ++i)
							{
								s_text_value_pair_definition* text_pair = text_value_pair_definition_get(category_reference->options[i]->index);

								int32 setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, text_pair->parameter);
								s_text_value_pair_reference_new* setting_label = multiplayer_variant_settings_interface_get_variant_parameter_label(text_pair, setting_value);

								c_maximum_interface_text label_buffer;
								c_maximum_interface_text value_buffer;

								string_list_get_normal_string(text_pair->string_list.index, text_pair->title_text, &label_buffer);

								// format the variant parameter text, if the setting_label is nullptr that means it is a
								// integer parsing parameter just print the int
								if (setting_label)
								{
									string_list_get_normal_string(text_pair->string_list.index, setting_label->label_string, &value_buffer);
									swprintf_s(string_buffer, L"%s: %s", label_buffer.get_string(), value_buffer.get_string());

								}
								else
								{
									swprintf_s(string_buffer, L"%s: %d", label_buffer.get_string(), setting_value);
								}

								cartographer_ingame_variant_ui_draw_setting_string(&cell_body_rect, body_line_height, string_buffer);
							}
						}
					}
				}
				else if (variant_categories[current_cell_index] == _variant_setting_category_type_cartographer_settings)
				{
					for (uint32 i = k_variant_setting_parameter_type_base_count + 1; i < k_variant_setting_parameter_type_base_count + 1 + k_variant_setting_parameter_type_cartographer_count; ++i)
					{
						e_variant_setting_parameter_type type = (e_variant_setting_parameter_type)i;

						c_maximum_interface_text title;
						c_maximum_interface_text value;

						int32 setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, type);

						multiplayer_variant_settings_interface_get_custom_variant_parameter_title(nullptr, type, title.get_buffer());
						multiplayer_variant_settings_interface_get_custom_variant_parameter_label(nullptr, type, setting_value, value.get_buffer());

						swprintf_s(string_buffer, NUMBEROF(string_buffer), L"%s: %s", title.get_string(), value.get_string());

						cartographer_ingame_variant_ui_draw_setting_string(&cell_body_rect, body_line_height, string_buffer);
					}
				}

				current_cell_index++;
			}
		}
	}
}

/* private code */

static real32 cartographer_ingame_variant_ui_fade_in_scale()
{
	real32 inv = 1.0f - g_cartographer_variant_settings_render_globals.progress;
	return 1.0f - (inv * inv);
}

static pixel32 cartographer_ingame_variant_ui_body_fade_in(pixel32 pixel)
{
	uint32 alpha = ((uint32)pixel >> 24) & 0xff;
	alpha = (uint32)(((real32)alpha * cartographer_ingame_variant_ui_fade_in_scale()) + 0.5f);
	return (pixel & 0x00ffffff) | (alpha << 24);
}

static const real_argb_color* cartographer_ingame_variant_ui_text_fade_in(real_argb_color* out_color, const real_argb_color* base_color)
{
	out_color->rgb = base_color->rgb;
	out_color->alpha = base_color->alpha * cartographer_ingame_variant_ui_fade_in_scale();
	return out_color;
}

void cartographer_ingame_variant_ui_draw_setting_string(rectangle2d* draw_bounds, int16 line_height, wchar_t const* string)
{
	rectangle2d text_bounds;
	rectangle2d cursor_bounds;

	draw_string_compute_bounds(draw_bounds, string, &text_bounds, &cursor_bounds, *get_secondary_hud_scale());

	const int32 rendered_height = rectangle2d_height(&text_bounds);
	const int16 line_count = (int16)(MAX(1, (rendered_height + line_height / 2) / line_height));

	rasterizer_draw_unicode_string(draw_bounds, string);

	draw_bounds->top += line_count * line_height + k_variant_ui_text_padding;
}
