#include "stdafx.h"
#include "screen_xbox_live_task_progress_dialog.h"

#include "Blam/Engine/interface/user_interface_screen_widget_definition.h"
#include "H2MOD/Tags/TagInterface.h"

void c_xbox_live_task_progress_menu::open(void* update_function)
{
	typedef void(__cdecl* c_screen_xbox_live_task_progress_dialog_t)(int arg_0, signed int local_player_index, void* update_function, int a4, int a5);
	auto p_c_screen_xbox_live_task_progress_dialog2 = Memory::GetAddress<c_screen_xbox_live_task_progress_dialog_t>(0x20C776);

	p_c_screen_xbox_live_task_progress_dialog2(-1, 0, update_function, 0, 0);
}

void c_xbox_live_task_progress_menu::apply_patches()
{
	const char task_progress_dialog_tag_path[] = "ui\\screens\\game_shell\\xbox_live\\task_progress_dialog\\task_progress_dialog";

	datum task_progress_dialog_datum_index = tags::find_tag(blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, task_progress_dialog_tag_path);
	
	if (task_progress_dialog_datum_index == DATUM_INDEX_NONE) { return;	}

	s_user_interface_screen_widget_definition* task_progress_dialog_definition = tags::get_tag_fast<s_user_interface_screen_widget_definition>(task_progress_dialog_datum_index);
	
	// If data is null ABORT
	if (task_progress_dialog_definition == nullptr) { return; }

	// If there's no pane tagblock ABORT
	if (task_progress_dialog_definition->panes.size == 0) { return; }

	s_window_pane_reference* pane_definition = task_progress_dialog_definition->panes[0];
	
	// Fix the text placement
	if (pane_definition->text_blocks.size > 0)
	{
		pane_definition->text_blocks[0]->text_bounds = { 144, -244, -38, 244 };
	}

	// Fix the placement of the 4 UI bitmaps
	if (pane_definition->bitmap_blocks.size == 4)
	{
		point2d fixed_bitmaps_placemets[4] = { {-288, 218}, {148, -66}, {-278, 208}, {-234, 306} };

		for (int i = 0; i < 4; ++i)
		{
			// fix the ui bitmap elements position
			pane_definition->bitmap_blocks[i]->topleft = fixed_bitmaps_placemets[i];
		}
	}

	// Fix the model viewport bounds
	if (pane_definition->model_scene_blocks.size > 0)
	{
		pane_definition->model_scene_blocks[0]->ui_viewport = { 78, -110, -174, 110 };
	}
}
