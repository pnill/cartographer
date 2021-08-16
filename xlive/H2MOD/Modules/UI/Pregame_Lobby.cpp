
#include "H2MOD.h"
#include "Pregame_Lobby.h"

#include "H2MOD/Tags/TagInterface.h"

void c_pregame_lobby_menu::ApplyPatches()
{
	if (h2mod->GetEngineType() != MainMenu) return;

	std::string pregame_lobby_tag_path("ui\\screens\\game_shell\\pregame_lobby\\pregame_lobby");

	// TODO: add wgit structure
	datum pregame_lobby_tag_datum_index = tags::find_tag(blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, pregame_lobby_tag_path);
	char* pregame_lobby_tag_data = tags::get_tag<blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, char>(pregame_lobby_tag_datum_index);

	if (pregame_lobby_tag_data != nullptr)
	{
		// get the menu content block
		tags::tag_data_block* menu_content_block = reinterpret_cast<tags::tag_data_block*>(pregame_lobby_tag_data + 0x20);

		if (menu_content_block->block_count > 0
			&& menu_content_block->block_data_offset != -1)
		{
			char* menu_content_data = tags::get_tag_data() + menu_content_block->block_data_offset + 0x4C;

			tags::tag_data_block* buttons_block = reinterpret_cast<tags::tag_data_block*>(menu_content_data + 0x4);
			tags::tag_data_block* text_strings_block = reinterpret_cast<tags::tag_data_block*>(menu_content_data + 0x1C);
			tags::tag_data_block* bitmaps_block = reinterpret_cast<tags::tag_data_block*>(menu_content_data + 0x24);
			tags::tag_data_block* players_block = reinterpret_cast<tags::tag_data_block*>(menu_content_data + 0x44);
			if (buttons_block->block_count == 2
				&& buttons_block->block_data_offset != -1)
			{
				char* buttons_data = tags::get_tag_data() + buttons_block->block_data_offset;
				INT16 fixed_button_delay[2] = { 0, 0 };
				rect2d fixed_button_bounds[2] = { {480, -690, 440, -360}, {480, -320, 440, 8} };
				point2d fixed_bitmap_offset_placement[2] = { {-15, -12}, { -15, -12} };
				INT16 fixed_depth_bias[2] = { 6, 6 };
				INT16 fixed_mouse_region[2] = { -12, -12 };

				for (int i = 0; i < buttons_block->block_count; i++)
				{
					// fix the ui delay
					INT16* ui_button_delay_placement = reinterpret_cast<INT16*>(buttons_data + (i * 0x3C) + 0x6);
					*ui_button_delay_placement = fixed_button_delay[i];

					// fix the ui bounds
					rect2d* ui_button_bounds_placement = reinterpret_cast<rect2d*>(buttons_data + (i * 0x3C) + 0x1C);
					*ui_button_bounds_placement = fixed_button_bounds[i];

					// fix the ui bitmap elements position
					point2d* ui_bitmap_offset_placement = reinterpret_cast<point2d*>(buttons_data + (i * 0x3C) + 0x2C);
					*ui_bitmap_offset_placement = fixed_bitmap_offset_placement[i];

					// fix the ui depth bias
					INT16* ui_depth_bias = reinterpret_cast<INT16*>(buttons_data + (i * 0x3C) + 0x34);
					*ui_depth_bias = fixed_depth_bias[i];

					// fix the ui mouse region
					INT16* ui_mouse_region = reinterpret_cast<INT16*>(buttons_data + (i * 0x3C) + 0x36);
					*ui_mouse_region = fixed_mouse_region[i];
				}
			}

			if (text_strings_block->block_count == 44
				&& text_strings_block->block_data_offset != -1)
			{
				char* text_strings_data = tags::get_tag_data() + text_strings_block->block_data_offset;
				INT16 fixed_text_delay_placemets[44] = { 0, 0, 0, 0, 0, 0, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250
													, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250
													, 250, 250, 250, 250 };

				rect2d fixed_text_bounds_placemets[44] = { {320, -650, 200, -30}, {-195, -675, -235, -300}, {-95, -580, -135, -150}, {-130, -580, -170, -150} , {-235, -675, -400, -325}
													  ,{90, -650, 50, -30}, {177, 118, 157, 276}, {155, 118, 135, 276}, {133, 118, 113, 276} , {111, 118, 91, 276}
													  ,{89, 118, 69, 276}, {67, 118, 47, 276}, {45, 118, 25, 276}, {23, 118, 3, 276} , {1, 118, -19, 276}
													  ,{-21, 118, -41, 276}, {-43, 118, -63, 276}, {-65, 118, -85, 276}, {-87, 118, -107, 276} , {-109, 118, -129, 276}
													  ,{-131, 118, -151, 276}, {-153, 118, -173, 276}, {9999, 118, 9999, 279}, {9999, 118, 9999, 279} , {9999, 118, 9999, 279}
													  ,{9999, 118, 9999, 279}, {9999, 118, 9999, 279}, {9999, 118, 9999, 279}, {9999, 118, 9999, 279} , {9999, 118, 9999, 279}
													  ,{9999, 118, 9999, 279}, {9999, 118, 9999, 279}, {9999, 118, 9999, 279}, {9999, 118, 9999, 279} , {9999, 118, 9999, 279}
													  ,{9999, 118, 9999, 279}, {9999, 118, 9999, 279}, {9999, 118, 9999, 279}, {440, -650, 360, -20} , {540, 220, 500, 570}
													  ,{50, -515, -30, -30}, {200, 200, -20, 600}, {-35, -635, -75, -46}, {0, 0, 0, 0} };

				for (int i = 0; i < text_strings_block->block_count; i++)
				{
					// fix the ui text delay
					INT16* ui_text_delay_placement = reinterpret_cast<INT16*>(text_strings_data + (i * 0x2C) + 0x6);
					*ui_text_delay_placement = fixed_text_delay_placemets[i];

					// fix the ui text elements position
					rect2d* ui_text_bounds_placement = reinterpret_cast<rect2d*>(text_strings_data + (i * 0x2C) + 0x1C);
					*ui_text_bounds_placement = fixed_text_bounds_placemets[i];
				}
			}

			if (bitmaps_block->block_count == 42
				&& bitmaps_block->block_data_offset != -1)
			{
				char* bitmap_data = tags::get_tag_data() + bitmaps_block->block_data_offset;
				std::string arrows_tag_path("ui\\screens\\game_shell\\network_squad_browser_screen\\arrows");
				datum arrows_tag_datum_index = tags::find_tag(blam_tag::tag_group_type::bitmap, arrows_tag_path);

				INT16 fixed_bitmap_delay[42] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250
											   , 250, 0, 0, 0, 0, 0, 0, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250
											   , 250, 250 };

				point2d fixed_top_left_placement[42] = { {-279, 174}, {-705, 350}, { -448, -30}, { -700, -30}, { -670, -90}
													   , { 9999, 9999}, { 9999, 9999}, { 9999, 9999}, { 9999, 9999}, { 9999, 9999} 
													   , { 9999, 9999}, { 9999, 9999}, { 9999, 9999}, { 9999, 9999}, { 9999, 9999}
													   , { 9999, 9999}, { 9999, 9999}, { 9999, 9999}, { 9999, 9999}, { 9999, 9999}
													   , { 9999, 9999}, { 100, 493}, { 100, 454}, { 100, 415}, { 100, 376}
													   , { 100, 337}, { 100, 298}, { 100, 259}, { 100, 220}, { 100, 181}
													   , { 100, 142}, { 100, 103}, { 100, 64}, { 100, 25}, { 100, -14}
													   , { 100, -53}, { 100, -92}, { -680, -185}, { -700, -30}, { 688, -160}
													   , { 688, -320}, { -654, 440} };

				for (int i = 0; i < bitmaps_block->block_count; i++)
				{
					if (i == 0)
					{
						datum* ui_bitmap_delay = reinterpret_cast<datum*>(bitmap_data + (i * 0x38) + 0x1C);
						*ui_bitmap_delay = 0xFFFFFFFF;
					}
					if (i == 39 || i == 40)
					{
						datum* ui_bitmap_delay = reinterpret_cast<datum*>(bitmap_data + (i * 0x38) + 0x1C);
						*ui_bitmap_delay = arrows_tag_datum_index;
					}
					// fix the ui bitmap delay
					INT16* ui_bitmap_delay = reinterpret_cast<INT16*>(bitmap_data + (i * 0x38) + 0x6);
					*ui_bitmap_delay = fixed_bitmap_delay[i];

					// fix the ui top left placement
					point2d* ui_top_left_placement = reinterpret_cast<point2d*>(bitmap_data + (i * 0x38) + 0xC);
					*ui_top_left_placement = fixed_top_left_placement[i];
				}
			}

			if (players_block->block_count > 0
				&& players_block->block_data_offset != -1)
			{
				char* players_data = tags::get_tag_data() + players_block->block_data_offset;
				std::string player_skin_lobby_tag_path("ui\\player_skins\\player_skin_lobby");
				datum player_skin_lobby_tag_datum_index = tags::find_tag(blam_tag::tag_group_type::userinterfacelistskindefinition, player_skin_lobby_tag_path);
				point2d fixed_bottom_left_placement = { 160, 430 };
				INT16 fixed_row_height = -39;

				// fix the ui tag ref
				datum* ui_tag_ref = reinterpret_cast<datum*>(players_data + 0x8);
				*ui_tag_ref = player_skin_lobby_tag_datum_index;

				// fix the ui bottom left position
				point2d* ui_bottom_left_placement = reinterpret_cast<point2d*>(players_data + 0xC);
				*ui_bottom_left_placement = fixed_bottom_left_placement;

				// fix the ui row height
				INT16* ui_row_height = reinterpret_cast<INT16*>(players_data + 0x14);
				*ui_row_height = fixed_row_height;
			}
		}
	}
}
