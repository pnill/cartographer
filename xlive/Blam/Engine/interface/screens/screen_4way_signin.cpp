#include "stdafx.h"

#include "screen_4way_signin.h"
#include "game/game.h"
#include "H2MOD/Tags/TagInterface.h"
#include "interface/user_interface.h"
#include "interface/user_interface_bitmap_block.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_globals.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_model_block.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_player_widget.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "interface/user_interface_utilities.h"
#include "Networking/online/online_account_xbox.h"
#include "tag_files/global_string_ids.h"

/* enums */

enum e_4way_signin_main_text_blocks
{

	_4way_signin_main_pane_0_text_player0_profile_name = 0,
	_4way_signin_main_pane_0_text_player0_insert_controller,
	_4way_signin_main_pane_0_text_player1_profile_name,
	_4way_signin_main_pane_0_text_player1_insert_controller,
	_4way_signin_main_pane_0_text_player2_profile_name,
	_4way_signin_main_pane_0_text_player2_insert_controller,
	_4way_signin_main_pane_0_text_player3_profile_name,
	_4way_signin_main_pane_0_text_player3_insert_controller,

	_4way_signin_main_pane_0_text_player0_profile_name_heading,
	_4way_signin_main_pane_0_text_player0_gamertag_heading,
	_4way_signin_main_pane_0_text_player0_gamertag,
	_4way_signin_main_pane_0_text_player1_profile_name_heading,
	_4way_signin_main_pane_0_text_player1_gamertag_heading,
	_4way_signin_main_pane_0_text_player1_gamertag,
	_4way_signin_main_pane_0_text_player2_profile_name_heading,
	_4way_signin_main_pane_0_text_player2_gamertag_heading,
	_4way_signin_main_pane_0_text_player2_gamertag,
	_4way_signin_main_pane_0_text_player3_profile_name_heading,
	_4way_signin_main_pane_0_text_player3_gamertag_heading,
	_4way_signin_main_pane_0_text_player3_gamertag,

	_4way_signin_main_pane_0_text_player0_press_a_to_join,
	_4way_signin_main_pane_0_text_player1_press_a_to_join,
	_4way_signin_main_pane_0_text_player2_press_a_to_join,
	_4way_signin_main_pane_0_text_player3_press_a_to_join,

};

enum e_4way_signin_main_bitmap_blocks
{
	_4way_signin_main_pane_0_bitmap_mp_signin = 0,
	_4way_signin_main_pane_0_bitmap_global_ul_09,
	_4way_signin_main_pane_0_bitmap_global_br_09,
	_4way_signin_main_pane_0_bitmap_3,
	_4way_signin_main_pane_0_bitmap_4,
	_4way_signin_main_pane_0_bitmap_5,
	_4way_signin_main_pane_0_bitmap_6,
	_4way_signin_main_pane_0_bitmap_player_0_controller_signin,
	_4way_signin_main_pane_0_bitmap_player_1_controller_signin,
	_4way_signin_main_pane_0_bitmap_player_2_controller_signin,
	_4way_signin_main_pane_0_bitmap_player_3_controller_signin,
};

enum e_4way_signin_main_model_blocks
{
	_4way_signin_main_pane_0_model_ui_player1 = 0,
	_4way_signin_main_pane_0_model_ui_player2,
	_4way_signin_main_pane_0_model_ui_player3,
	_4way_signin_main_pane_0_model_ui_player4,
	_4way_signin_main_pane_0_model_ui_player1b,
	_4way_signin_main_pane_0_model_ui_player2b,
	_4way_signin_main_pane_0_model_ui_player3b,
	_4way_signin_main_pane_0_model_ui_player4b,
};

enum e_controller_signin_bitmaps
{
	_controller_signin_bitmap_missing = 0,
	_controller_signin_bitmap_akebono,
	_controller_signin_bitmap_duke,
	k_number_of_controller_signin_bitmap_types
};

/* structures */

struct s_screen_4way_items
{
	uint8 text_profile_name;
	uint8 text_press_a_to_join;
	uint8 text_insert_controller;
	uint8 text_profile_name_heading;
	uint8 text_gamertag_heading;
	uint8 text_gamertag_text;
	uint8 model_ui_player_a;
	uint8 model_ui_player_b;
	uint8 bitmap_unknown;
	uint8 bitmap_controller_signin;
};

/* globals */

uint32 ui_recover_from_disconnection_return_address = NULL;

/* forward declarations*/

void initialize_elements(s_screen_4way_items& elements, e_controller_index controller);

/* public code */

c_screen_4way_signin::c_screen_4way_signin(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags) :
	c_screen_widget(_screen_4way_join_screen, channel_type, window_index, user_flags)
{
	m_call_context = _4_way_signin_type_splitscreen;
}

c_screen_4way_signin::~c_screen_4way_signin()
{
}

void c_screen_4way_signin::update()
{

	const uint32 all_users_mask = NONE;
	const bool has_live_privileges = true;

	this->set_controller_mask(all_users_mask);
	c_player_widget_representation representations[k_number_of_controllers];
	s_screen_4way_items items;

	for (e_controller_index controller = _controller_index_0;
		controller != k_no_controller;
		controller = user_interface_controller_get_next_valid_index(controller))
	{
		bool gamepad_connected = user_interface_controller_has_gamepad(controller);
		bool controller_has_joined = has_live_privileges && user_interface_controller_is_player_profile_valid(controller);
		bool show_gamertag_text = false;

		s_saved_game_file_player_profile profile;
		if (controller_has_joined)
		{
			uint32 profile_index;
			user_interface_controller_get_profile_data(controller, &profile, &profile_index);

			c_player_widget_representation* current_player = &representations[controller];
			current_player->set_appearance(&profile.profile);
			current_player->set_player_name_from_configuration((s_player_properties*)profile.player_name.get_buffer()); //hacky
			show_gamertag_text = online_connected_to_xbox_live();
		}

		initialize_elements(items, controller);

		this->set_child_visible(_widget_type_text, TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(items.text_profile_name), controller_has_joined);
		c_text_widget* press_a_to_join_text = this->try_find_screen_text(items.text_press_a_to_join);
		if (press_a_to_join_text)
		{
			press_a_to_join_text->set_visible(false);

			if (!controller_has_joined)
			{
				string_id join_text_string;
				if (gamepad_connected)
				{
					join_text_string = _string_id_press_a_to_join;
				}
				else
				{
					join_text_string = _string_id_insert_controller;
				}
				press_a_to_join_text->set_visible(true);
				press_a_to_join_text->set_text_from_string_id(join_text_string);
			}
		}

		c_text_widget* insert_controller_text = this->try_find_screen_text(items.text_insert_controller);
		if (insert_controller_text)
		{
			if (controller_has_joined)
			{
				insert_controller_text->set_visible(true);
				insert_controller_text->set_text_from_string_id(_string_id_press_a_to_continue);
			}
			else
			{
				insert_controller_text->set_visible(false);
			}
		}
	
		this->set_child_visible(_widget_type_text, TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(items.text_gamertag_text), show_gamertag_text);
		this->set_child_visible(_widget_type_text, TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(items.text_gamertag_heading), show_gamertag_text);
		this->set_child_visible(_widget_type_text, TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(items.text_profile_name_heading), controller_has_joined);

		//for some reason game saves player characters as masterchief and dervish in saved profiles
		c_model_widget* ui_player_model_a = this->try_find_model_widget(items.model_ui_player_a);
		if (ui_player_model_a)
		{
			if (controller_has_joined)
				ui_player_model_a->apply_appearance_and_character(&profile.profile, _character_type_masterchief);
			else
				ui_player_model_a->set_visible(false);
		}
		c_model_widget* ui_player_model_b = this->try_find_model_widget(items.model_ui_player_b);
		if (ui_player_model_b)
		{
			if (controller_has_joined)
				ui_player_model_b->apply_appearance_and_character(&profile.profile, _character_type_dervish);
			else
				ui_player_model_b->set_visible(false);

		}


		c_bitmap_widget* unknown_bitmap = this->try_find_bitmap_widget(items.bitmap_unknown);
		if (unknown_bitmap)
			unknown_bitmap->set_visible(controller_has_joined);

		c_bitmap_widget* controller_signin_bitmap = this->try_find_bitmap_widget(items.bitmap_controller_signin);
		if (controller_signin_bitmap)
		{
			bool should_show_bitmap = controller_has_joined == false;
			controller_signin_bitmap->set_visible(should_show_bitmap);
			if (should_show_bitmap)
			{
				// we dont really have a way to determine controller_types between akebono and duke , using either is fine.
				int16 bitmap_update_idx = gamepad_connected ? _controller_signin_bitmap_akebono : _controller_signin_bitmap_missing;
				controller_signin_bitmap->verify_and_update_bitmap_index(bitmap_update_idx);
			}
		}
	}


	this->apply_new_representations_to_players(representations, k_number_of_controllers);
	c_user_interface_widget::update();
}

bool c_screen_4way_signin::handle_event(s_event_record* event)
{

	bool result = false;

	if (user_interface_channel_is_busy(_user_interface_channel_type_game_error))
		return true;

	if (event->type == _user_interface_event_type_gamepad_button_pressed)
	{
		if (!user_interface_controller_is_player_profile_valid(event->controller))
		{
			// if the event owner is not yet signed in
			result = this->handle_default_events(event);
		}
		else
		{

			result = this->handle_main_events(event);

		}

		if (result)
			return result;
	}


	return c_screen_widget::handle_event(event);
}

void c_screen_4way_signin::initialize(s_screen_parameters* parameters)
{
	s_interface_expected_screen_layout layout;
	csmemset(&layout, 0, sizeof(layout));
	layout.panes_count = 1;


	datum widget_tag_datum = user_interface_get_widget_tag_index_from_screen_id(this->m_screen_id);
	if (!DATUM_IS_NONE(widget_tag_datum))
	{
		this->verify_and_load_from_layout(NONE, &layout);
	}
	this->setup_children();
	user_interface_squad_clear_match_playlist();
}

void* c_screen_4way_signin::load_proc()
{
	switch (this->m_call_context)
	{
	case _4_way_signin_type_crossgame_invite:
		return  &c_screen_4way_signin::load_for_crossgame_invite;
		break;
	case _4_way_signin_type_xbox_live:
		return  &c_screen_4way_signin::load_for_xbox_live;
		break;

	case _4_way_signin_type_system_link:
		return  &c_screen_4way_signin::load_for_system_link;
		break;

	case _4_way_signin_type_splitscreen:
		return  &c_screen_4way_signin::load_for_splitscreen;
		break;

	case _4_way_signin_type_campaign:
		return  &c_screen_4way_signin::load_for_campaign;
		break;

	default:
		LOG_ERROR_FUNC("unreachable type");
	}
	return nullptr;
}

bool __cdecl user_interface_mainmenu_sign_out_controller_callback(e_controller_index controller_index)
{
	//return INVOKE(0xA421, 0x0, user_interface_mainmenu_sign_out_controller_callback, controller_index);
	user_interface_controller_sign_out(controller_index);
	user_interface_enter_game_shell(2);
	return true;

}
bool __cdecl user_interface_sign_out_controller_default_callback(e_controller_index controller_index)
{
	user_interface_controller_sign_out(controller_index);
	return true;
}
bool __cdecl user_interface_decline_invite_callback(e_controller_index controller_index)
{
	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = FLAG(controller_index);
	params.m_channel_type = _user_interface_channel_type_gameshell;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.field_4 = 0xFFFFFFFF;
	params.m_screen_state.field_8 = 0xFFFFFFFF;
	params.m_load_function = c_screen_press_start_introduction_load;

	params.m_load_function(&params);
	return true;
}

bool c_screen_4way_signin::handle_main_events(s_event_record* event)
{
	char sucess = true;
	if (event->component == _user_interface_controller_component_button_a ||
		event->component == _user_interface_controller_component_button_start)
	{
		s_screen_parameters params;
		params.m_flags = 0;
		params.m_window_index = _window_4;
		params.field_C = 0;
		params.user_flags = FLAG(event->controller);
		params.m_channel_type = _user_interface_channel_type_gameshell;
		params.m_screen_state.field_0 = 0xFFFFFFFF;
		params.m_screen_state.field_4 = 0xFFFFFFFF;
		params.m_screen_state.field_8 = 0xFFFFFFFF;
		params.m_load_function = nullptr;

		switch (this->m_call_context)
		{
		case _4_way_signin_type_campaign:

			if (user_interface_create_new_squad(true, online_connected_to_xbox_live()))
			{
				user_interface_squad_clear_game_settings();
				user_interface_set_desired_multiplayer_mode(0);
				int32 difficulty = user_interface_globals_get_game_difficulty();

				user_interface_globals_set_game_difficulty_real(difficulty);
				user_interface_globals_set_loading_from_persistent_storage(false);
				user_interface_squad_set_campaign_difficulty(difficulty);
			}
			break;
		case _4_way_signin_type_splitscreen:
			if (user_interface_create_new_squad(true, false))
			{
				user_interface_set_desired_multiplayer_mode(2);
			}
			break;
		case _4_way_signin_type_system_link:
			params.m_load_function = c_screen_network_squad_browser_load;
			break;
		case _4_way_signin_type_xbox_live:
			params.m_load_function = c_screen_bungie_news_load;
			break;
		case _4_way_signin_type_crossgame_invite:

			//this needs further research

			screen_error_ok_dialog_show(_user_interface_channel_type_interface, _ui_error_cant_join_gameinvite_without_signon, _window_4, FLAG(event->controller), 0, 0);

			break;
		}

		if (params.m_load_function != nullptr)
			params.m_load_function(&params);

	}
	else if (event->component == _user_interface_controller_component_button_b ||
		event->component == _user_interface_controller_component_button_back)
	{
		if (user_interface_controller_get_signed_in_controller_count() == 1)
		{
			// _screen_xbox_live_main_menu is replaced by _screen_bungie_news in h2v
			//if (!user_interface_back_out_from_channel_by_id(_user_interface_channel_type_gameshell, _window_4, _screen_xbox_live_main_menu)
			if (!user_interface_back_out_from_channel_by_id(_user_interface_channel_type_gameshell, _window_4, _screen_bungie_news)
				&& !user_interface_back_out_from_channel_by_id(_user_interface_channel_type_gameshell, _window_4, _screen_main_menu))
			{
				user_interface_enter_game_shell(1);
			}
		}
		else
		{

			if (user_interface_controller_get_guest_controllers_count_for_master(event->controller) <= 0)
			{
				if (this->m_call_context == _4_way_signin_type_crossgame_invite)
				{
					user_interface_error_display_ok_cancle_dialog_with_ok_callback(_user_interface_channel_type_interface,
						_window_4,
						FLAG(event->controller),
						user_interface_mainmenu_sign_out_controller_callback,
						_ui_error_confirm_controller_sign_out);
				}
				else
				{
					user_interface_error_display_ok_cancle_dialog_with_ok_callback(_user_interface_channel_type_interface,
						_window_4,
						FLAG(event->controller),
						user_interface_sign_out_controller_default_callback,
						_ui_error_confirm_controller_sign_out);
				}
			}
			else
			{
				screen_error_ok_dialog_show(_user_interface_channel_type_interface, _ui_error_cant_sign_out_master_with_guests, _window_4, FLAG(event->controller), NULL, NULL);

			}
			this->m_controllers_mask |= FLAG(event->controller);
		}

	}
	else
	{
		sucess = false;
	}

	return sucess;
}

bool c_screen_4way_signin::handle_default_events(s_event_record* event)
{
	if (event->component == _user_interface_controller_component_button_a
		|| event->component == _user_interface_controller_component_button_start)
	{
		bool online = false;
		if (online_connected_to_xbox_live() && this->m_call_context == _4_way_signin_type_crossgame_invite)
		{
			online = true;
		}
		user_interface_controller_pick_profile_dialog(event->controller, online);
	}
	else if ((event->component == _user_interface_controller_component_button_b 
		|| event->component == _user_interface_controller_component_button_back)
		&& !user_interface_controller_get_signed_in_controller_count())
	{
		user_interface_error_display_ok_cancle_dialog_with_ok_callback(
			_user_interface_channel_type_interface,
			_window_4,
			FLAG(event->controller),
			user_interface_decline_invite_callback,
			_ui_confirm_decline_crossgame_invite);
	}
	return true;
}

void* c_screen_4way_signin::load(s_screen_parameters * parameters)
{
	c_screen_4way_signin* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_4way_signin), 0);
	if (pool)
	{
		screen = new (pool) c_screen_4way_signin(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->user_flags);

		screen->m_allocated = true;
	}
	else
	{
		screen = 0;
	}

	return screen;
}

void* c_screen_4way_signin::load_for_crossgame_invite(s_screen_parameters* parameters)
{
	c_screen_4way_signin* screen;
	screen = (c_screen_4way_signin*)c_screen_4way_signin::load(parameters);
	screen->m_call_context = _4_way_signin_type_crossgame_invite;
	user_interface_register_screen_to_channel(screen, parameters);
	return screen;
}
void* c_screen_4way_signin::load_for_xbox_live(s_screen_parameters* parameters)
{
	c_screen_4way_signin* screen;
	screen = (c_screen_4way_signin*)c_screen_4way_signin::load(parameters);
	screen->m_call_context = _4_way_signin_type_xbox_live;
	user_interface_register_screen_to_channel(screen, parameters);
	return screen;
}
void* c_screen_4way_signin::load_for_system_link(s_screen_parameters* parameters)
{
	c_screen_4way_signin* screen;
	screen = (c_screen_4way_signin*)c_screen_4way_signin::load(parameters);
	screen->m_call_context = _4_way_signin_type_system_link;
	user_interface_register_screen_to_channel(screen, parameters);
	return screen;
}
void* c_screen_4way_signin::load_for_splitscreen(s_screen_parameters* parameters)
{
	c_screen_4way_signin* screen;
	screen = (c_screen_4way_signin*)c_screen_4way_signin::load(parameters);
	screen->m_call_context = _4_way_signin_type_splitscreen;
	user_interface_register_screen_to_channel(screen, parameters);
	return screen;
}
void* c_screen_4way_signin::load_for_campaign(s_screen_parameters* parameters)
{
	c_screen_4way_signin* screen;
	screen = (c_screen_4way_signin*)c_screen_4way_signin::load(parameters);
	screen->m_call_context = _4_way_signin_type_campaign;
	user_interface_register_screen_to_channel(screen, parameters);
	return screen;
}

void c_screen_4way_signin::apply_patches_on_map_load()
{

	const char* main_widget_tag_path = "ui\\screens\\game_shell\\4way_multiplayer_signin\\4way_signin_main";
	const char* player_skins_tag_path = "ui\\player_skins\\player_skin_signin";
	const int16 scale_factor = 2;

	datum main_widget_datum_index = tags::find_tag(_tag_group_user_interface_screen_widget_definition, main_widget_tag_path);
	datum player_skins_datum_index = tags::find_tag(_tag_group_user_interface_list_skin_definition, player_skins_tag_path);

	if (DATUM_IS_NONE(main_widget_datum_index) || DATUM_IS_NONE(player_skins_datum_index))
	{
		LOG_ERROR_FUNC("bad datum found");
		return;
	}

	s_user_interface_screen_widget_definition* main_widget_tag = tags::get_tag_fast<s_user_interface_screen_widget_definition>(main_widget_datum_index);
	s_window_pane_reference* base_pane = main_widget_tag->panes[0];

	const point2d bitmap_positions[] = {
		{ -227   ,   198   },
		{ -280   ,   180   },
		{   45   ,    31   },
		{ -107   ,   123   },
		{  172   ,   123   },
		{ -107   ,   -54   },
		{  172   ,   -54   },
		{ -181   ,   145   },
		{   94   ,   145   },
		{ -181   ,   -32   },
		{   94   ,   -32   }
	};

	if (base_pane->bitmap_blocks.count > 0)
	{
		for (uint8 itr = 0; itr < base_pane->bitmap_blocks.count; itr++)
		{
			point2d bitmap_pos = bitmap_positions[itr];
			point2d_scale(&bitmap_pos, scale_factor);
			base_pane->bitmap_blocks[itr]->topleft = bitmap_pos;
		}
	}

	const rectangle2d text_bounds[] = {
		{ 68    ,  -179    ,  46     ,  -4   },
		{166    ,  -179    ,  118    ,  -4   },
		{ 68    ,   100    ,  46     ,  275  },
		{166    ,   100    ,  118    ,  275  },
		{-108   ,  -179    , -130    ,  -4   },
		{-5     ,  -179    ,  -59    ,  -4   },
		{-108   ,   100    ,  -130   ,  275  },
		{-5     ,   100    ,  -59    ,  275  },
		{ 86    ,  -179    ,   68    ,  -4   },
		{ 47    ,  -179    ,   29    ,  -4   },
		{ 29    ,  -179    ,   7     ,  -4   },
		{ 86    ,   100    ,   68    ,  275  },
		{ 47    ,   100    ,   29    ,  275  },
		{ 29    ,   100    ,   7     ,  275  },
		{-90    ,  -179    ,  -108   ,  -4   },
		{-130   ,  -179    ,  -148   ,  -4   },
		{-148   ,  -179    ,  -170   ,  -4   },
		{-90    ,   100    ,  -108   ,  275  },
		{-130   ,   100    ,  -148   ,  275  },
		{-148   ,   100    ,  -170   ,  275  },
		{ 68    ,  -229    ,   14    ,  -47  },
		{ 68    ,    46    ,   14    , 	228  },
		{-108   ,  -229    ,  -162   ,  -47  },
		{-108   ,    46    ,  -162   ,  228  }
	};

	if (base_pane->text_blocks.count > 0)
	{
		for (uint8 itr = 0; itr < base_pane->text_blocks.count; itr++)
		{
			rectangle2d og = text_bounds[itr];
			rectangle2d_scale(&og, scale_factor);
			base_pane->text_blocks[itr]->text_bounds = og;
		}
	}

	const rectangle2d model_viewports[] = {
		{240 , -320,  0		, 320 }   ,
		{240 , -320,  0		, 320 }   ,
		{0   , -320,-240	, 320 }   ,
		{0   , -320,-240	, 320 }   ,
		{240 , -320,  0		, 320 }   ,
		{240 , -320,  0		, 320 }   ,
		{0   , -320,-240	, 320 }   ,
		{0   , -320,-240	, 320 }   ,
	};
	if (base_pane->model_scene_blocks.count > 0)
	{
		for (uint8 itr = 0; itr < base_pane->model_scene_blocks.count; itr++)
		{
			rectangle2d og = model_viewports[itr];
			rectangle2d_scale(&og, scale_factor);
			base_pane->model_scene_blocks[itr]->ui_viewport = og;
		}
	}
	if (base_pane->player_blocks.count > 0)
	{
		s_player_block_reference* players = base_pane->player_blocks[0];

		point2d og_pos = { -107 , 59 };
		point2d_scale(&og_pos, scale_factor);
		players->bottomleft = og_pos;
		players->row_height = -177 * scale_factor;
		players->column_width = 279 * scale_factor;

	}

	//hacky wacky until skin tag definitions are added
	void* player_skins_signin_tag = tags::get_tag_fast(player_skins_datum_index);
	tag_block<s_hud_block_reference> hud_block = *reinterpret_cast<tag_block<s_hud_block_reference>*>((char*)(player_skins_signin_tag)+0x2C);


	s_hud_block_reference* base_hud_block = hud_block[0];
	rectangle2d og_bounds = { 6   , -6, -58	, 58 };
	rectangle2d_scale(&og_bounds, scale_factor);
	base_hud_block->bounds = og_bounds;

	const char* shader_tag_path = "ui\\hud\\shaders\\ui_small_emblem";
	datum shader_datum_index = tags::find_tag(_tag_group_shader, shader_tag_path);

	// ui_medium_emblem is bugged for guest profiles so we use ui_small_emblem shader
	// but the bitmap quality is lowered
	// fixme
	if (!DATUM_IS_NONE(shader_datum_index))
	{
		base_hud_block->shader.index = shader_datum_index;
	}
}

void initialize_elements(s_screen_4way_items& elements, e_controller_index controller)
{
	switch (controller)
	{
	case _controller_index_0:

		elements.text_profile_name = _4way_signin_main_pane_0_text_player0_profile_name;
		elements.text_press_a_to_join = _4way_signin_main_pane_0_text_player0_press_a_to_join;
		elements.text_insert_controller = _4way_signin_main_pane_0_text_player0_insert_controller;
		elements.text_profile_name_heading = _4way_signin_main_pane_0_text_player0_profile_name_heading;
		elements.text_gamertag_heading = _4way_signin_main_pane_0_text_player0_gamertag_heading;
		elements.text_gamertag_text = _4way_signin_main_pane_0_text_player0_gamertag;
		elements.model_ui_player_a = _4way_signin_main_pane_0_model_ui_player1;
		elements.model_ui_player_b = _4way_signin_main_pane_0_model_ui_player1b;
		elements.bitmap_unknown = _4way_signin_main_pane_0_bitmap_3;
		elements.bitmap_controller_signin = _4way_signin_main_pane_0_bitmap_player_0_controller_signin;

		break;

	case _controller_index_1:

		elements.text_profile_name = _4way_signin_main_pane_0_text_player1_profile_name;
		elements.text_press_a_to_join = _4way_signin_main_pane_0_text_player1_press_a_to_join;
		elements.text_insert_controller = _4way_signin_main_pane_0_text_player1_insert_controller;
		elements.text_profile_name_heading = _4way_signin_main_pane_0_text_player1_profile_name_heading;
		elements.text_gamertag_heading = _4way_signin_main_pane_0_text_player1_gamertag_heading;
		elements.text_gamertag_text = _4way_signin_main_pane_0_text_player1_gamertag;
		elements.model_ui_player_a = _4way_signin_main_pane_0_model_ui_player2;
		elements.model_ui_player_b = _4way_signin_main_pane_0_model_ui_player2b;
		elements.bitmap_unknown = _4way_signin_main_pane_0_bitmap_4;
		elements.bitmap_controller_signin = _4way_signin_main_pane_0_bitmap_player_1_controller_signin;

		break;

	case _controller_index_2:

		elements.text_profile_name = _4way_signin_main_pane_0_text_player2_profile_name;
		elements.text_press_a_to_join = _4way_signin_main_pane_0_text_player2_press_a_to_join;
		elements.text_insert_controller = _4way_signin_main_pane_0_text_player2_insert_controller;
		elements.text_profile_name_heading = _4way_signin_main_pane_0_text_player2_profile_name_heading;
		elements.text_gamertag_heading = _4way_signin_main_pane_0_text_player2_gamertag_heading;
		elements.text_gamertag_text = _4way_signin_main_pane_0_text_player2_gamertag;
		elements.model_ui_player_a = _4way_signin_main_pane_0_model_ui_player3;
		elements.model_ui_player_b = _4way_signin_main_pane_0_model_ui_player3b;
		elements.bitmap_unknown = _4way_signin_main_pane_0_bitmap_5;
		elements.bitmap_controller_signin = _4way_signin_main_pane_0_bitmap_player_2_controller_signin;

		break;

	case _controller_index_3:

		elements.text_profile_name = _4way_signin_main_pane_0_text_player3_profile_name;
		elements.text_press_a_to_join = _4way_signin_main_pane_0_text_player3_press_a_to_join;
		elements.text_insert_controller = _4way_signin_main_pane_0_text_player3_insert_controller;
		elements.text_profile_name_heading = _4way_signin_main_pane_0_text_player3_profile_name_heading;
		elements.text_gamertag_heading = _4way_signin_main_pane_0_text_player3_gamertag_heading;
		elements.text_gamertag_text = _4way_signin_main_pane_0_text_player3_gamertag;
		elements.model_ui_player_a = _4way_signin_main_pane_0_model_ui_player4;
		elements.model_ui_player_b = _4way_signin_main_pane_0_model_ui_player4b;
		elements.bitmap_unknown = _4way_signin_main_pane_0_bitmap_6;
		elements.bitmap_controller_signin = _4way_signin_main_pane_0_bitmap_player_3_controller_signin;

		break;
	}
}

void user_interface_recover_4way_screen(e_session_protocol protocol)
{
	s_screen_parameters params;
	params.m_flags = 7; //retreating or recovering?
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = user_interface_controller_get_signed_in_controllers_mask();
	params.m_channel_type = _user_interface_channel_type_gameshell;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.field_4 = 0xFFFFFFFF;
	params.m_screen_state.field_8 = 0xFFFFFFFF;
	params.m_load_function = &c_screen_4way_signin::load_for_splitscreen;

	if (session_protocol_has_coop(protocol))
	{
		params.m_load_function = &c_screen_4way_signin::load_for_campaign;
	}

	params.m_load_function(&params);
}

void user_interface_recover_4way_screen_pregame(e_session_protocol protocol, e_user_interface_screen_id screen_id)
{
	if (screen_id == _screen_4way_join_screen)
	{
		//simply return if we are already in the 4way screen
		return;
	}
	else if (screen_id == _screen_multiplayer_pregame_lobby)
	{
		user_interface_recover_4way_screen(protocol);
	}

}

__declspec(naked) void jmp_to_recover_4way_for_splitscreen_for_network_pregame()
{
	__asm
	{
		push	edi //screen_id
		push	eax //protocol
		call	user_interface_recover_4way_screen_pregame

		pop		eax
		pop		edi

		//remaining function epilog can handle rest
		jmp		ui_recover_from_disconnection_return_address
	}
}

__declspec(naked) void jmp_to_recover_4way_for_splitscreen_for_network_postgame()
{
	__asm
	{
		push	eax //protocol
		call	user_interface_recover_4way_screen
		pop		eax

		//remaining function epilog can handle rest
		jmp		ui_recover_from_disconnection_return_address
	}
}

void user_interface_recover_from_disconnection_patches()
{
	// Use _screen_4way_join_screen instead of _screen_main_menu when backing out from splitscreen lobby
	WriteValue<uint8>(Memory::GetAddress(0x217583) + 1, _screen_4way_join_screen); //inside user_interface_recover_from_disconnection

	//patch jmptables
	WritePointer(Memory::GetAddress(0x217678), jmp_to_recover_4way_for_splitscreen_for_network_pregame);//case _session_protocol_splitscreen_coop
	WritePointer(Memory::GetAddress(0x21767C), jmp_to_recover_4way_for_splitscreen_for_network_pregame);//case _session_protocol_splitscreen_custom

	WritePointer(Memory::GetAddress(0x217694), jmp_to_recover_4way_for_splitscreen_for_network_postgame);//case _session_protocol_splitscreen_coop
	WritePointer(Memory::GetAddress(0x217698), jmp_to_recover_4way_for_splitscreen_for_network_postgame);//case _session_protocol_splitscreen_custom

	ui_recover_from_disconnection_return_address = Memory::GetAddress(0x2174DB);

	// fixes broken main_menu when leaving cooperative game
	WriteValue<uint8>(Memory::GetAddress(0x9112) + 1, 3);

}

//context is unused
void screen_network_squad_browser_backout_to_4way_screen(uint8 context)
{
	s_screen_parameters params;
	params.m_flags = 7; //retreating or recovering?
	params.m_window_index = _window_4;
	params.field_C = 0;
	params.user_flags = user_interface_controller_get_signed_in_controllers_mask(); // orignally h2x uses the flags
	params.m_channel_type = _user_interface_channel_type_gameshell;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.field_4 = 0xFFFFFFFF;
	params.m_screen_state.field_8 = 0xFFFFFFFF;
	params.m_load_function = &c_screen_4way_signin::load_for_system_link;

	params.m_load_function(&params);
}

void user_interface_backout_to_mainmenu_patches()
{
	// use _screen_4way_join_screen instead of _screen_main_menu
	WriteValue<uint8>(Memory::GetAddress(0x219E91) + 1, _screen_4way_join_screen); // inside c_screen_network_squad_browser::handle_event
	WriteValue<uint8>(Memory::GetAddress(0x21FD22) + 1, _screen_4way_join_screen); // inside c_screen_bungie_news::handle_event
	PatchCall(Memory::GetAddress(0x219EA7), screen_network_squad_browser_backout_to_4way_screen);// replace call to user_interface_enter_game_shell
	WriteValue(Memory::GetAddress(0x21FD3D) + 1, c_screen_4way_signin::load_for_xbox_live);// replace call to c_screen_main_menu::load
}

void c_screen_4way_signin::apply_instance_patches()
{
	if (Memory::IsDedicatedServer())
		return;

	user_interface_recover_from_disconnection_patches();
	user_interface_backout_to_mainmenu_patches();
}
	
