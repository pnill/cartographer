
#include "H2MOD.h"
#include "XboxLiveTaskProgress.h"

#include "H2MOD/Tags/TagInterface.h"

void c_xbox_live_task_progress_menu::Open(void* callback)
{
	typedef void(__cdecl* c_screen_xbox_live_task_progress_dialog2)(int arg_0, signed int local_player_index, void* callback, int a4, int a5);
	auto p_c_screen_xbox_live_task_progress_dialog2 = Memory::GetAddress<c_screen_xbox_live_task_progress_dialog2>(0x20C776);

	p_c_screen_xbox_live_task_progress_dialog2(-1, 0, callback, 0, 0);
}

void c_xbox_live_task_progress_menu::ApplyPatches()
{
	if (h2mod->GetEngineType() != MainMenu || Memory::isDedicatedServer()) return;

	std::string task_progress_dialog_tag_path("ui\\screens\\game_shell\\xbox_live\\task_progress_dialog\\task_progress_dialog");

	// TODO: add wgit structure
	datum task_progress_dialog_tag_datum_index = tags::find_tag(blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, task_progress_dialog_tag_path);
	char* task_progress_dialog_tag_data = tags::get_tag<blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, char>(task_progress_dialog_tag_datum_index);

	if (task_progress_dialog_tag_data != nullptr)
	{
		// get the menu content block
		tags::tag_data_block* menu_content_block = reinterpret_cast<tags::tag_data_block*>(task_progress_dialog_tag_data + 0x20);

		// here we deal only with 1 block of data
		if (menu_content_block->block_count > 0
			&& menu_content_block->block_data_offset != -1)
		{
			char* menu_content_data = tags::get_tag_data() + menu_content_block->block_data_offset;

			// here we deal only with 1 block of data as well
			tags::tag_data_block* text_strings_block = reinterpret_cast<tags::tag_data_block*>(menu_content_data + 0x1C);
			if (text_strings_block->block_count > 0
				&& text_strings_block->block_data_offset != -1)
			{
				char* text_strings_data = tags::get_tag_data() + text_strings_block->block_data_offset;
				rect2d* text_placement = reinterpret_cast<rect2d*>(text_strings_data + 0x1C);

				// fix the text placement
				rect2d fixed_text_placement = { 140, -250, -19, 250 };
				*text_placement = fixed_text_placement;
			}

			// here we deal with the UI bitmaps, 4 in total
			tags::tag_data_block* menu_ui_bitmaps = reinterpret_cast<tags::tag_data_block*>(menu_content_data + 0x24);
			if (menu_ui_bitmaps->block_count == 4
				&& menu_ui_bitmaps->block_data_offset != -1)
			{
				char* ui_bitmap = tags::get_tag_data() + menu_ui_bitmaps->block_data_offset;
				point2d fixed_bitmaps_placemets[4] = { {-292, 210}, {150, -130}, {-282, 198}, {-230, 242} };
				for (int i = 0; i < menu_ui_bitmaps->block_count; i++)
				{
					// fix the ui bitmap elements position
					point2d* ui_bitmap_placement = reinterpret_cast<point2d*>(ui_bitmap + (i * 0x38) + 0xC);
					*ui_bitmap_placement = fixed_bitmaps_placemets[i];
				}
			}

			// here we deal only with 1 block of data as well
			tags::tag_data_block* model_scene_block = reinterpret_cast<tags::tag_data_block*>(menu_content_data + 0x2C);
			if (model_scene_block->block_count > 0
				&& model_scene_block->block_data_offset != -1)
			{
				char* model_scene_data = tags::get_tag_data() + model_scene_block->block_data_offset;
				rect2d* model_scene_placement = reinterpret_cast<rect2d*>(model_scene_data + 0x38);

				// fix the model scene placement
				rect2d fixed_model_scene_placement = { 78, -110, -174, 110 };
				*model_scene_placement = fixed_model_scene_placement;
			}
		}
	}
}
