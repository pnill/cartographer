#include "stdafx.h"
#include "screen_settings.h"

#include "screen_audio_settings.h"
#include "screen_video_settings.h"
#include "screen_about_dialog.h"
#include "screen_network_adapter.h"
#include "screen_variant_game_engine_type.h"
#include "screen_single_player_profile_select.h"

#include "bitmaps/bitmap_group.h"
#include "cache/cache_files.h"
#include "cartographer/discord/discord_interface.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_widget_text.h"
#include "interface//user_interface_bitmap_block.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "interface/screens/screen_cartographer_menus.h"
#include "networking/network_event.h"
#include "networking/panorama/panorama_presence.h"


#include "tag_files/tag_loader/tag_injection.h"

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

	for (int32 i = 0; i < m_list_data->maximum_count; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);
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

void c_settings_list::handle_item_pressed_event(s_event_record* const& event, datum* pitem_index)
{
	//return INVOKE_TYPE(0x21F70A, 0x0, void(__thiscall*)(c_settings_list*, s_event_record**, long*), this, pevent, pitem_index);

	int16 item_absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index);

	switch (item_absolute_index)
	{
	case _item_player_profile:
		handle_item_player_profile(event);
		break;
	case _item_variant:
		handle_item_variant(event);
		break;
	case _item_video:
		handle_item_video(event);
		break;
	case _item_audio:
		handle_item_audio(event);
		break;
	case _item_network:
		handle_item_network(event);
		break;
	case _item_about:
		handle_item_about(event);
		break;
	case _item_guide:
		handle_item_guide(event);
		break;
	}
}

void c_settings_list::handle_item_player_profile(
	s_event_record* const& event)
{
	c_screen_parameters params;

	params.initialize_default_user(FLAG(event->controller), _user_interface_channel_type_gameshell_screen, _window_4, c_screen_single_player_profile_select_fancy::load);
	params.execute_load_function();
	return;
}

void c_settings_list::handle_item_variant(
	s_event_record* const& event)
{
	c_screen_parameters params;

	params.initialize_default_user(FLAG(event->controller), _user_interface_channel_type_gameshell_screen, _window_4, c_screen_game_engine_category::load_for_settings);
	params.execute_load_function();
	return;
}

void c_settings_list::handle_item_video(
	s_event_record* const& event)
{
	c_screen_parameters params;

	params.initialize_default_user(FLAG(event->controller), _user_interface_channel_type_gameshell_screen, _window_4, c_screen_video_settings::load);
	params.execute_load_function();
	return;
}

void c_settings_list::handle_item_audio(
	s_event_record* const& event)
{
	c_screen_parameters params;
	
	params.initialize_default_user(FLAG(event->controller), _user_interface_channel_type_gameshell_screen, _window_4, c_screen_audio_settings::load);
	params.execute_load_function();
	return;
}

void c_settings_list::handle_item_network(
	s_event_record* const& event)
{
	c_screen_parameters params;

	params.initialize_default_user(FLAG(event->controller), _user_interface_channel_type_gameshell_dialog, _window_4, c_screen_network_adapter_menu::load);
	params.execute_load_function();
	return;
}

void c_settings_list::handle_item_about(s_event_record* const& event)
{
	c_screen_parameters params;

	params.initialize_default_user(FLAG(event->controller), _user_interface_channel_type_gameshell_dialog, _window_4, c_screen_about_dialog_menu::load);
	params.execute_load_function();
	return;
}

void c_settings_list::handle_item_guide(s_event_record* const& event)
{
	ui_load_cartographer_guide_menu();
	return;
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

		if (cartographer_bitmap_datum != NONE && main_bitmap)
		{
			bitmap_data* bitmap_block = bitmap_group_get_bitmap(cartographer_bitmap_datum, _settings_cartographer_bitmap_type_default);
			main_bitmap->assign_new_bitmap_block(bitmap_block);
		}

	}

	// call orignal function now
	c_screen_widget::post_initialize_button_keys();
}


const void* c_screen_settings::load_proc(void) const
{
	return &c_screen_settings::load;
}

void* c_screen_settings::load(c_screen_parameters* parameters)
{
	//return INVOKE(0xB071, 0x0, c_screen_settings::load, parameters);

	c_screen_settings* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_settings), 0);
	if (pool)
	{
		screen = new (pool) c_screen_settings(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags()
		);

		screen->m_allocated = true;

		panorama_presence_set(_context_presence_settings);
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
	const char* k_main_widget_tag_path = "ui\\screens\\game_shell\\settings_screen\\settings";

	datum main_widget_datum_index = tag_loaded(_tag_group_user_interface_screen_widget_definition, k_main_widget_tag_path);

	if (main_widget_datum_index == NONE)
	{
		error(_error_log, "bad datum found");
		return;
	}

	s_user_interface_screen_widget_definition* main_widget_tag = user_interface_screen_widget_definition_get(main_widget_datum_index);

	// add window_pane for guide option 
	tag_injection_extend_block(&main_widget_tag->panes, sizeof(s_window_pane_reference), k_number_of_addition_panes_for_settings_screen);
	
	s_window_pane_reference* guide_pane = user_interface_widget_pane_get(&main_widget_tag->panes, _settings_pane_guide);
	s_window_pane_reference* about_pane = user_interface_widget_pane_get(&main_widget_tag->panes, _settings_pane_about);

	// copy data from about_pane
	*guide_pane = *about_pane;
	
	// updating the new number of visible items in each pane
	for (int32 i = 0; i < main_widget_tag->panes.count; ++i)
	{
		s_window_pane_reference const* pane_reference = TAG_BLOCK_GET_ELEMENT(
			&main_widget_tag->panes,
			i,
			s_window_pane_reference
		);
		s_list_reference* list_reference = TAG_BLOCK_GET_ELEMENT(
			&pane_reference->list_block,
			0,
			s_list_reference
		);


		list_reference->num_visible_items = k_no_of_visible_items_for_settings;
	}

	tag_injection_set_active_map(L"mainmenu_bitmaps");
	if (tag_injection_active_map_verified())
	{
		datum cartographer_bitmap_org_datum = tag_injection_load(_tag_group_bitmap, "ui\\screens\\game_shell\\settings_screen\\cartographer", true);

		if (cartographer_bitmap_org_datum != NONE)
		{
			tag_injection_inject();
			cartographer_bitmap_datum = cartographer_bitmap_org_datum;
			event(_event_verbose, "interface:screens: cartographer bitmap datum : 0x%08X", cartographer_bitmap_datum);
		}
		else
		{
			cartographer_bitmap_datum = NONE;
		}
	}
	else
	{
		cartographer_bitmap_datum = NONE;
	}

	

	//clone blocks into a seperate memory (so we dont end up affecting about_pane)
	tag_injection_extend_block(&guide_pane->text_blocks, sizeof(s_text_block_reference), k_number_of_addition_texts_for_guide_pane);

	//copy data from existing blocks

	s_text_block_reference* flavor_text33 = TAG_BLOCK_GET_ELEMENT(&guide_pane->text_blocks, _pane_guide_text_flavor_text33, s_text_block_reference);
	s_text_block_reference* flavor_text34 = TAG_BLOCK_GET_ELEMENT(&guide_pane->text_blocks, _pane_guide_text_flavor_text34, s_text_block_reference);
	s_text_block_reference* flavor_text1 = TAG_BLOCK_GET_ELEMENT(&guide_pane->text_blocks, _pane_guide_text_custom_flavor_text1, s_text_block_reference);
	s_text_block_reference* flavor_text2 = TAG_BLOCK_GET_ELEMENT(&guide_pane->text_blocks, _pane_guide_text_custom_flavor_text2, s_text_block_reference);

	csmemcpy(flavor_text1, flavor_text33, sizeof(s_text_block_reference));
	csmemcpy(flavor_text2, flavor_text33, sizeof(s_text_block_reference));

	//update new text_block to reposition callout texts
	flavor_text33->text_bounds = { -5 ,-5,-115 ,75 };
	flavor_text34->text_bounds = { 180 ,460,140 ,860 };
	flavor_text1->text_bounds = { 250 ,30,150 ,150 };
	flavor_text2->text_bounds = { -120 ,450,-220 ,530 };
	return;
}
