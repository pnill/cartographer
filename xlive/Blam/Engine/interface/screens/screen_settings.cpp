#include "stdafx.h"
#include "screen_settings.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_widget_text.h"
#include "interface//user_interface_bitmap_block.h"
#include "interface/user_interface_globals.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "networking/panorama/panorama_presence.h"
#include "tag_files/global_string_ids.h"
#include "H2MOD/Modules/CustomMenu/CustomMenu.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"

/* macro defines */

#define k_settings_list_name "settings list"

/* enums */

enum e_settings_list_items : uint16
{
	//h2x
	//_item_player_profile,
	//_item_variant,

	//h2v
	_item_player_profile,
	_item_variant,
	_item_video,
	_item_audio,
	_item_network,
	_item_about,

	//custom addition
	_item_guide,
	k_total_no_of_settings_list_items
};

enum e_default_list_skin_texts
{
	_default_list_skin_text_main = 0,
	k_number_of_default_list_skin_texts,
};

enum e_settings_screen_panes
{
	_settings_pane_player_profile,
	_settings_pane_variant,
	_settings_pane_video,
	_settings_pane_audio,
	_settings_pane_network,
	_settings_pane_about,

	//custom addition
	_settings_pane_guide,
	k_total_no_of_settings_pane,

	k_total_no_of_settings_pane_orignal = 6,
	k_number_of_addition_panes_for_settings_screen = k_total_no_of_settings_pane- k_total_no_of_settings_pane_orignal
};

enum e_screen_pane_texts
{
	_pane_about_text_description = 0,
	_pane_about_text_flavor_text33,
	_pane_about_text_flavor_text34,
	k_screen_pane_about_texts_count,


	_pane_guide_text_description = 0,
	_pane_guide_text_flavor_text33,
	_pane_guide_text_flavor_text34,

	//custom addition
	_pane_guide_text_custom_flavor_text1,
	_pane_guide_text_custom_flavor_text2,
	k_screen_pane_guide_texts_count,

	k_total_no_of_guide_texts_orignal = 3,
	k_number_of_addition_texts_for_guide_pane = k_screen_pane_guide_texts_count - k_total_no_of_guide_texts_orignal
};

enum e_screen_pane_bitmaps
{
	_pane_about_bitmap_settings_screen_bitmap = 0,
	_pane_about_bitmap_ul_06,
	_pane_about_bitmap_br_03,
	k_screen_pane_about_bitmaps_count,

	_pane_guide_bitmap_settings_screen_bitmap = 0,
	_pane_guide_bitmap_ul_06,
	_pane_guide_bitmap_br_03,
	k_screen_pane_guide_bitmaps_count,
};

enum e_settings_cartographer_bitmap_type
{
	_settings_cartographer_bitmap_type_default = 0,
	k_number_of_cartographer_variant_bitmap_types,
};

/* globals */

datum cartographer_bitmap_datum = NONE;

c_settings_list::c_settings_list(int16 user_flags):
	c_list_widget(user_flags),
	m_slot(this, &c_settings_list::handle_item_pressed_event)
{
	//never actually set true?
	field_464 = false;

	//we dont need s_list_item_datum here as no of list items remain same
	m_list_data = ui_list_data_new(k_settings_list_name, k_total_no_of_settings_list_items, sizeof(datum));
	data_make_valid(m_list_data);

	for (int32 i = 0; i < m_list_data->datum_max_elements; ++i)
	{
		datum_new(m_list_data);
	}

	signal2->link_signal_to_slot((_slot*)&signal2, &m_slot);
}


c_settings_list::~c_settings_list()
{
}

c_list_item_widget* c_settings_list::get_list_items()
{
	return m_list_items;
}

int32 c_settings_list::get_list_items_count()
{
	//return INVOKE_TYPE(0x21E9BF, 0x0, int(__thiscall*)(c_settings_list*), this);
	return (field_464 != false) + k_no_of_visible_items_for_settings;
}

void c_settings_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	//return INVOKE_TYPE(0x21E9CE, 0x0, void(__thiscall*)(c_settings_list*, c_list_widget*, int), this, item, skin_index);

	ASSERT(item);
	c_text_widget* item_text = item->try_find_text_widget(_default_list_skin_text_main);

	if (item_text)
	{
		switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()))
		{
		case _item_player_profile:
			item_text->set_text_from_string_id(_string_id_settings_player_profile);
			break;
		case _item_variant:
			item_text->set_text_from_string_id(_string_id_settings_variant);
			break;
		case _item_video:
			item_text->set_text_from_string_id(_string_id_settings_video);
			break;
		case _item_audio:
			item_text->set_text_from_string_id(_string_id_settings_audio);
			break;
		case _item_network:
			item_text->set_text_from_string_id(_string_id_settings_network);
			break;
		case _item_about:
			item_text->set_text_from_string_id(_string_id_settings_about);
			break;
		case _item_guide:
			item_text->set_text(L"Cartographer");
			break;
		}
	}
}

bool c_settings_list::handle_item_pressed_event(s_event_record** pevent, datum* pitem_index)
{
	//return INVOKE_TYPE(0x21F70A, 0x0, bool(__thiscall*)(c_settings_list*, s_event_record**, long*), this, pevent, pitem_index);

	int16 item_absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index);

	switch (item_absolute_index)
	{
	case _item_player_profile:
		handle_item_player_profile(pevent);
		break;
	case _item_variant:
		handle_item_variant(pevent);
		break;
	case _item_video:
		handle_item_video(pevent);
		break;
	case _item_audio:
		handle_item_audio(pevent);
		break;
	case _item_network:
		handle_item_network(pevent);
		break;
	case _item_about:
		handle_item_about(pevent);
		break;
	case _item_guide:
		handle_item_guide(pevent);
		break;
	
	}

	return true;
}

bool c_settings_list::handle_item_player_profile(s_event_record** pevent)
{
	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = FLAG((*pevent)->controller);
	params.m_channel_type = _user_interface_channel_type_gameshell;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = c_screen_single_player_profile_select_fancy_load;

	params.m_load_function(&params);
	return true;
}
bool c_settings_list::handle_item_variant(s_event_record** pevent)
{
	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = FLAG((*pevent)->controller);
	params.m_channel_type = _user_interface_channel_type_gameshell;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = c_screen_game_engine_category_load;

	params.m_load_function(&params);
	return true;
}
bool c_settings_list::handle_item_video(s_event_record** pevent)
{
	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = FLAG((*pevent)->controller);
	params.m_channel_type = _user_interface_channel_type_gameshell;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = c_screen_video_settings_load;

	params.m_load_function(&params);
	return true;
}
bool c_settings_list::handle_item_audio(s_event_record** pevent)
{
	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = FLAG((*pevent)->controller);
	params.m_channel_type = _user_interface_channel_type_gameshell;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = c_screen_audio_settings_load;

	params.m_load_function(&params);
	return true;
}
bool c_settings_list::handle_item_network(s_event_record** pevent)
{
	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = FLAG((*pevent)->controller);
	params.m_channel_type = _user_interface_channel_type_interface;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = c_screen_network_adapter_menu_load;

	params.m_load_function(&params);
	return true;
}
bool c_settings_list::handle_item_about(s_event_record** pevent)
{
	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = FLAG((*pevent)->controller);
	params.m_channel_type = _user_interface_channel_type_interface;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = c_screen_about_dialog_menu_load;

	params.m_load_function(&params);
	return true;
}
bool c_settings_list::handle_item_guide(s_event_record** pevent)
{
	GSCustomMenuCall_Guide();
	return true;
}



//
// c_screen_settings class starts here
// 


c_screen_settings::c_screen_settings(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags) :
	c_screen_with_menu(_screen_settings, channel_type, window_index, user_flags, &m_settings_list),
	m_settings_list(user_flags)
{
	user_interface_globals_commit_edit_profile_changes();
	user_interface_squad_clear_match_playlist();
}

c_screen_settings::~c_screen_settings()
{
}

void c_screen_settings::post_initialize_button_keys()
{
	// this function is executed once upon every pane creation
	// thus can be used as on_pane_switch hook

	if (this->m_pane_index == _settings_pane_guide
		|| DATUM_INDEX_TO_ABSOLUTE_INDEX(this->m_settings_list.get_old_data_index()) == _item_guide)
	{
		c_text_widget* description = this->try_find_screen_text(_pane_guide_text_description);
		c_text_widget* flavor_text1 = this->try_find_screen_text(_pane_guide_text_custom_flavor_text1);
		c_text_widget* flavor_text2 = this->try_find_screen_text(_pane_guide_text_custom_flavor_text2);
		c_bitmap_widget* main_bitmap = this->try_find_bitmap_widget(_pane_guide_bitmap_settings_screen_bitmap);
		if (description)
		{
			description->set_text(L"Go to Cartographer settings");
		}

		if (flavor_text1)
		{
			flavor_text1->set_text(L"Matchmaking is ready! and Coop works for real");
		}

		if (flavor_text2)
		{
			flavor_text2->set_text(L"Hop onto ONLINE and be ready to suffer from disappointment");
		}

		if (!DATUM_IS_NONE(cartographer_bitmap_datum) && main_bitmap)
		{
			bitmap_data* bitmap_block = bitmap_group_get_bitmap(cartographer_bitmap_datum, _settings_cartographer_bitmap_type_default);
			main_bitmap->assign_new_bitmap_block(bitmap_block);
		}

	}

	// call orignal function now
	c_screen_widget::post_initialize_button_keys();
}


void* c_screen_settings::load_proc()
{
	return &c_screen_settings::load;
}

void* c_screen_settings::load(s_screen_parameters* parameters)
{
	//return INVOKE(0xB071, 0x0, c_screen_settings::load, parameters);

	c_screen_settings* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_settings), 0);
	if (pool)
	{
		screen = new (pool) c_screen_settings(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->user_flags);

		screen->m_allocated = true;

		networking_panorama_presence_set_presence(_context_presence_settings);
		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		screen = 0;
	}

	return screen;
}

void c_screen_settings::apply_patches_on_map_load()
{
	const char* main_widget_tag_path = "ui\\screens\\game_shell\\settings_screen\\settings";

	datum main_widget_datum_index = tags::find_tag(_tag_group_user_interface_screen_widget_definition, main_widget_tag_path);

	if (DATUM_IS_NONE(main_widget_datum_index))
	{
		LOG_ERROR_FUNC("bad datum found");
		return;
	}

	s_user_interface_screen_widget_definition* main_widget_tag = tags::get_tag_fast<s_user_interface_screen_widget_definition>(main_widget_datum_index);

	//	add window_pane for guide option 
	MetaExtender::add_tag_block3<s_window_pane_reference>((uint32)&main_widget_tag->panes, k_number_of_addition_panes_for_settings_screen);
	//copy data from about_pane
	csmemcpy(main_widget_tag->panes[_settings_pane_guide], main_widget_tag->panes[_settings_pane_about], sizeof(s_window_pane_reference));
	//updating the new number of visible items in each pane
	for (s_window_pane_reference& pane : main_widget_tag->panes)
	{
		pane.list_block[0]->num_visible_items = k_no_of_visible_items_for_settings;
	}

	datum cartographer_bitmap_org_datum = tag_loader::Get_tag_datum("ui\\screens\\game_shell\\settings_screen\\cartographer", _tag_group_bitmap, "mainmenu_bitmaps");

	if (!DATUM_IS_NONE(cartographer_bitmap_org_datum))
	{
		tag_loader::Load_tag(cartographer_bitmap_org_datum, false, "mainmenu_bitmaps");
		tag_loader::Push_Back();
		cartographer_bitmap_datum = tag_loader::ResolveNewDatum(cartographer_bitmap_org_datum);
		LOG_DEBUG_FUNC("cartographer bitmap datum : 0x{:08X} ,", cartographer_bitmap_datum);
	}
	else
	{
		cartographer_bitmap_datum = NONE;
	}

	s_window_pane_reference* guide_pane = main_widget_tag->panes[_settings_pane_guide];
	//clone blocks into a seperate memory (so we dont end up affecting about_pane)
	MetaExtender::add_tag_block3<s_text_block_reference>((uint32)&main_widget_tag->panes[_settings_pane_guide]->text_blocks, k_number_of_addition_texts_for_guide_pane);
	//copy data from existing blocks
	csmemcpy(guide_pane->text_blocks[_pane_guide_text_custom_flavor_text1], guide_pane->text_blocks[_pane_guide_text_flavor_text33], sizeof(s_text_block_reference));
	csmemcpy(guide_pane->text_blocks[_pane_guide_text_custom_flavor_text2], guide_pane->text_blocks[_pane_guide_text_flavor_text33], sizeof(s_text_block_reference));

	
	//update new text_block to reposition callout texts
	guide_pane->text_blocks[_pane_guide_text_flavor_text33]->text_bounds = { -5 ,-5,-115 ,75 };
	guide_pane->text_blocks[_pane_guide_text_flavor_text34]->text_bounds = { 180 ,460,140 ,860 };
	guide_pane->text_blocks[_pane_guide_text_custom_flavor_text1]->text_bounds = { 250 ,30,150 ,150 };
	guide_pane->text_blocks[_pane_guide_text_custom_flavor_text2]->text_bounds = { -120 ,450,-220 ,530 };

}
