#include "stdafx.h"
#include "screen_main_menu.h"
#include "screen_4way_signin.h"
#include "screen_settings.h"
#include "game/preferences.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_globals.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_widget_text.h"
#include "interface/user_interface_guide.h"
#include "interface/user_interface_utilities.h"
#include "Networking/online/online_account_xbox.h"
#include "Networking/transport/transport.h"
#include "Networking/panorama/panorama_friends.h"
#include "tag_files/global_string_ids.h"
#include "shell/shell.h"


/* macro defines */

#define k_main_menu_list_name "main menu list"

/* enums */

enum e_main_menu_list_items : uint16
{
	//h2v
	//_item_campaign = 0,
	//_item_xbox_live,
	//_item_system_link,
	//_item_settings,
	//_item_guide,
	//_item_quit,

	//h2x
	_item_campaign = 0,
	_item_xbox_live,
	_item_splitscreen,
	_item_system_link,
	_item_settings,
	_item_quit,

	k_total_no_of_main_menu_list_items,
};

enum e_main_menu_list_skin_texts
{
	_main_menu_list_skin_text_main = 0,
	_main_menu_list_skin_text_greyed,
	k_number_of_main_menu_list_skin_texts
};

/* forward declarations*/

bool __cdecl screen_show_screen_4way_signin_splitscreen_offline(e_controller_index controller_index);
bool __cdecl screen_show_screen_4way_signin_system_link_offline(e_controller_index controller_index);
bool __cdecl screen_show_screen_4way_signin_xbox_live_callback();


c_main_menu_list::c_main_menu_list(int16 user_flags) :
	c_list_widget(user_flags),
	m_slot(this, &c_main_menu_list::handle_item_pressed_event)
{
	//we dont need s_list_item_datum here as no of list items remain same
	m_list_data = ui_list_data_new(k_main_menu_list_name, k_total_no_of_main_menu_list_items, sizeof(datum));
	data_make_valid(m_list_data);

	for (int32 i = 0; i < m_list_data->datum_max_elements; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);
}

void c_main_menu_list::c_main_menu_list_ctor(int16 user_flags)
{
	new (this)c_main_menu_list(user_flags);
}

__declspec(naked) void jmp_c_main_menu_list() { __asm { jmp c_main_menu_list::c_main_menu_list_ctor } }

c_user_interface_widget* c_main_menu_list::destructor(uint32 flags)
{
	this->~c_main_menu_list();
	if (TEST_BIT(flags, 0))
	{
	}

	return this;
}

c_list_item_widget* c_main_menu_list::get_list_items()
{
	return m_list_items;
}

int32 c_main_menu_list::get_list_items_count()
{
	return k_no_of_visible_items_for_main_menu;
}

void c_main_menu_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	//return INVOKE_TYPE(0xA755, 0x0, void(__thiscall*)(c_list_widget*, c_list_widget*, int), this, item, skin_index);


	ASSERT(item);
	uint8 skin_text_type = _main_menu_list_skin_text_main;
	do
	{
		c_text_widget* skin_text = item->try_find_text_widget(skin_text_type);
		if (skin_text)
		{
			skin_text->set_visible(false);
		}

	} while (skin_text_type++ < k_number_of_main_menu_list_skin_texts);


	// this code responsible for checking to switch between main/greyed text
	//bool is_valid = unlock_main_menu_items(item->get_last_data_index());
	//item_text = item->try_find_text_widget(!is_valid);

	c_text_widget* item_text = item->try_find_text_widget(_main_menu_list_skin_text_main);
	item->set_item_transitioning();

	if (item_text)
	{
		switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()))
		{
		case _item_campaign:
			item_text->set_text_from_string_id(_string_id_campaign);
			break;
		case _item_xbox_live:
			item_text->set_text_from_string_id(_string_id_xbox_live);
			break;
		case _item_splitscreen:
			item_text->set_text_from_string_id(_string_id_splitscreen);
			break;
		case _item_system_link:
			item_text->set_text_from_string_id(_string_id_systemlink);
			break;
		case _item_settings:
			item_text->set_text_from_string_id(_string_id_settings);
			break;
		case _item_quit:
			item_text->set_text_from_string_id(_string_id_quit);
			break;
		default:
			item_text->set_text_from_string_id(_string_id_invalid);
		}


		item_text->set_visible(true);
	}
}

void c_main_menu_list::handle_item_pressed_event(s_event_record** pevent, datum* pitem_index)
{
	//return INVOKE_TYPE(0xB396, 0x0, bool(__thiscall*)(c_main_menu_list*, s_event_record**, long*), this, pevent, pitem_index);

	int16 item_absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index);

	// orignal h2v code , dont need this anymore
	//if (!unlock_main_menu_items(item_absolute_index))
	//	return false;


	if (item_absolute_index != _item_quit)
		global_preferences_set_last_main_menu_item(item_absolute_index);

	if (shell_startup_flag_is_set(_startup_flag_ui_fast_test) || shell_startup_flag_is_set(_startup_flag_ui_fast_test_no_start))
	{
		item_absolute_index = _item_system_link;
		//byte_811031 = 0;
		WriteValue<bool>(Memory::GetAddress(0x411031), false);
	}

	switch (item_absolute_index)
	{
	case _item_campaign:
		handle_item_campaign(pevent);
		break;
	case _item_xbox_live:
		handle_item_xbox_live(pevent);
		break;
	case _item_splitscreen:
		handle_item_splitscreen(pevent);
		break;
	case _item_system_link:
		handle_item_system_link(pevent);
		break;
	case _item_settings:
		handle_item_settings(pevent);
		break;
		//case _item_guide:
		//	handle_item_guide(pevent);
	case _item_quit:
		handle_item_quit(pevent);
		break;
	}
}

void c_main_menu_list::handle_item_campaign(s_event_record** pevent)
{
	INVOKE_TYPE(0xB198, 0x0, void(__thiscall*)(c_main_menu_list*, s_event_record**), this, pevent);
}

void c_main_menu_list::handle_item_xbox_live(s_event_record** pevent)
{
	//return INVOKE_TYPE(0xB257, 0x0, bool(__thiscall*)(c_main_menu_list*, s_event_record**), this, pevent);

	//disabling activation check as we dont need it
	/*
	XLivePBufferGetByte(dword_879E78, 0, &game_activated);
	if (!game_activated)
		return true;
	*/

	c_networking_panorama_friends* friends = get_networking_panorama_friends();
	if (!friends->has_active_task())
	{
		friends->initialize_startup();
	}

	//xbox checks if there are any live-controllers signed in
	//h2v on the other hand checks for live-connection-status and the flag
	if (online_connected_to_xbox_live() || shell_startup_flag_is_set(_startup_flag_unlock_xbox_live_menus))
	{
		s_screen_parameters params;
		params.m_flags = 0;
		params.m_window_index = _window_4;
		params.m_context = 0;
		params.user_flags = FLAG((*pevent)->controller);
		params.m_channel_type = _user_interface_channel_type_gameshell_screen;
		params.m_screen_state.field_0 = 0xFFFFFFFF;
		params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
		params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
		//params.m_load_function = &c_screen_bungie_news::load; // replacing with 4way_screen
		params.m_load_function = &c_screen_4way_signin::load_for_xbox_live;

		params.m_load_function(&params);
	}
	else
	{
		if (transport_available())
		{
			user_interface_guide_state_manager_get()->add_user_signin_task(true, screen_show_screen_4way_signin_xbox_live_callback);
		}
		else
		{
			screen_error_ok_dialog_show(
				_user_interface_channel_type_game_error,
				_ui_error_xblive_cannot_access_service,
				_window_4,
				FLAG((*pevent)->controller),
				nullptr,
				nullptr);
		}
	}
}

void c_main_menu_list::handle_item_splitscreen(s_event_record** pevent)
{
	if (user_interface_globals_is_beta_build())
	{
		screen_error_ok_dialog_show(
			_user_interface_channel_type_game_error,
			_ui_error_beta_feature_disabled,
			_window_4,
			FLAG((*pevent)->controller),
			nullptr,
			nullptr);
	}
	else if (online_connected_to_xbox_live())
	{
		user_interface_error_display_ok_cancel_dialog_with_ok_callback(
			_user_interface_channel_type_dialog,
			_window_4,
			FLAG((*pevent)->controller),
			screen_show_screen_4way_signin_splitscreen_offline,
			_ui_error_confirm_xbox_live_sign_out);
	}
	else
	{
		screen_show_screen_4way_signin_splitscreen_offline((*pevent)->controller);
	}
}

void c_main_menu_list::handle_item_system_link(s_event_record** pevent)
{
	//return INVOKE_TYPE(0xA978, 0x0, bool(__thiscall*)(c_main_menu_list*, s_event_record**), this, pevent);

	if (user_interface_globals_is_beta_build())
	{
		screen_error_ok_dialog_show(
			_user_interface_channel_type_game_error,
			_ui_error_beta_feature_disabled,
			_window_4,
			FLAG((*pevent)->controller),
			nullptr,
			nullptr);
	}
	else if (online_connected_to_xbox_live())
	{
		user_interface_error_display_ok_cancel_dialog_with_ok_callback(
			_user_interface_channel_type_dialog,
			_window_4,
			FLAG((*pevent)->controller),
			screen_show_screen_4way_signin_system_link_offline,
			_ui_error_confirm_xbox_live_sign_out);
	}
	else
	{
		screen_show_screen_4way_signin_system_link_offline((*pevent)->controller);
	}
}

void c_main_menu_list::handle_item_settings(s_event_record** pevent)
{
	//return INVOKE_TYPE(0xB32B, 0x0, bool(__thiscall*)(c_main_menu_list*, s_event_record**), this, pevent);

	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.m_context = NULL;
	params.user_flags = FLAG((*pevent)->controller);
	params.m_channel_type = _user_interface_channel_type_gameshell_screen;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = &c_screen_settings::load;

	params.m_load_function(&params);
}

//void c_main_menu_list::handle_item_guide(s_event_record** pevent)
//{
//	return INVOKE_TYPE(0xB513, 0x0, void(__thiscall*)(c_main_menu_list*, s_event_record**), this, pevent);
//}

void c_main_menu_list::handle_item_quit(s_event_record** pevent)
{
	return INVOKE_TYPE(0xA307, 0x0, void(__thiscall*)(c_main_menu_list*, s_event_record**), this, pevent);
}

bool __cdecl screen_show_screen_4way_signin_splitscreen_offline(e_controller_index controller_index)
{
	online_account_transition_to_offline();
	user_interface_transition_to_offline();
	
	if (user_interface_controller_get_signed_in_controller_count() <= 0)
	{
		user_interface_enter_game_shell(0);
	}
	else
	{
		s_screen_parameters params;
		params.m_flags = 0;
		params.m_window_index = _window_4;
		params.m_context = 0;
		params.user_flags = user_interface_controller_get_signed_in_controllers_mask();
		params.m_channel_type = _user_interface_channel_type_gameshell_screen;
		params.m_screen_state.field_0 = 0xFFFFFFFF;
		params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
		params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
		params.m_load_function = &c_screen_4way_signin::load_for_splitscreen;

		params.m_load_function(&params);
	}

	return true;
}

bool __cdecl screen_show_screen_4way_signin_system_link_offline(e_controller_index controller_index)
{
	online_account_transition_to_offline();
	user_interface_transition_to_offline();

	if (user_interface_controller_get_signed_in_controller_count() <= 0)
	{
		user_interface_enter_game_shell(0);
	}
	else
	{
		if (transport_available())
		{
			s_screen_parameters params;
			params.m_flags = 0;
			params.m_window_index = _window_4;
			params.m_context = 0;
			params.user_flags = user_interface_controller_get_signed_in_controllers_mask();
			params.m_channel_type = _user_interface_channel_type_gameshell_screen;
			params.m_screen_state.field_0 = 0xFFFFFFFF;
			params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
			params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
			params.m_load_function = &c_screen_4way_signin::load_for_system_link;

			params.m_load_function(&params);
		}
		else
		{
			screen_error_ok_dialog_show(
				_user_interface_channel_type_game_error,
				_ui_error_network_link_required,
				_window_4,
				user_interface_controller_get_signed_in_controllers_mask(),
				nullptr,
				nullptr);
		}
	}

	return true;
}

bool __cdecl screen_show_screen_4way_signin_xbox_live_callback()
{
	if (!UserSignedOnline(_controller_index_0))
		return true;

	if (!user_interface_controller_is_player_profile_valid(_controller_index_0))
	{
		// dont allow transitioning into live if _controller_index_0 is inactive
		screen_error_ok_dialog_show(
			_user_interface_channel_type_game_error,
			_ui_error_xblive_user_not_authorized,
			_window_4,
			NONE,// allow all controllers
			user_interface_controller_sign_out_all_controllers,
			nullptr);
	}

	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.m_context = 0;
	params.user_flags = NONE;// allow all
	params.m_channel_type = _user_interface_channel_type_gameshell_screen;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = &c_screen_4way_signin::load_for_xbox_live;

	params.m_load_function(&params);
	return true;
}


void c_main_menu_list::apply_instance_patches()
{
	if (Memory::IsDedicatedServer())
		return;

	PatchCall(Memory::GetAddress(0xB724), jmp_c_main_menu_list); //replace our c_main_menu_list inside c_screen_main_menu ctor
}
