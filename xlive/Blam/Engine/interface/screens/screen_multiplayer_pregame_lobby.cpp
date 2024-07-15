#include "stdafx.h"

#include "H2MOD/Tags/TagInterface.h"
#include "screen_multiplayer_pregame_lobby.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "interface/user_interface_widget_window.h"
#include "interface/user_interface_networking.h"
#include "H2MOD/Tags/MetaExtender.h"


enum e_pregame_lobby_button_blocks
{
	// pane 0 : custom_games
	_pregame_lobby_pane_0_button_start_game = 0,
	_pregame_lobby_pane_0_button_game_setup,// last h2x button

	_pregame_lobby_pane_0_button_find_game, // h2v starts from here
	_pregame_lobby_pane_0_button_game_details,
	_pregame_lobby_pane_0_button_chat_button,
	k_pregame_lobby_pane_0_button_count,

	// pane 1 : cooperative
	_pregame_lobby_pane_1_button_start_game = 0,
	_pregame_lobby_pane_1_button_game_setup,
	k_pregame_lobby_pane_1_button_count,

};

enum e_pregame_lobby_text_blocks
{
	// pane 0 : custom_games
	_pregame_lobby_pane_0_text_game_cant_start_no_session = 0,
	_pregame_lobby_pane_0_text_gametype_options_format,
	_pregame_lobby_pane_0_text_gametype_format,
	_pregame_lobby_pane_0_text_mapname_format,
	_pregame_lobby_pane_0_text_4,
	_pregame_lobby_pane_0_text_5,
	_pregame_lobby_pane_0_text_6,
	_pregame_lobby_pane_0_text_7,
	_pregame_lobby_pane_0_text_8,
	_pregame_lobby_pane_0_text_9,
	_pregame_lobby_pane_0_text_party_privacy,
	_pregame_lobby_pane_0_text_11,
	_pregame_lobby_pane_0_text_12,
	_pregame_lobby_pane_0_text_13,
	_pregame_lobby_pane_0_text_14,
	_pregame_lobby_pane_0_text_15,
	_pregame_lobby_pane_0_text_16,
	_pregame_lobby_pane_0_text_17,
	_pregame_lobby_pane_0_text_18,
	_pregame_lobby_pane_0_text_19,
	_pregame_lobby_pane_0_text_20,
	_pregame_lobby_pane_0_text_21,
	_pregame_lobby_pane_0_text_22,
	_pregame_lobby_pane_0_text_23,
	_pregame_lobby_pane_0_text_24,
	_pregame_lobby_pane_0_text_25,
	_pregame_lobby_pane_0_text_26,
	_pregame_lobby_pane_0_text_27,
	_pregame_lobby_pane_0_text_28,
	_pregame_lobby_pane_0_text_29,
	_pregame_lobby_pane_0_text_30,
	_pregame_lobby_pane_0_text_31,
	_pregame_lobby_pane_0_text_32,
	_pregame_lobby_pane_0_text_33,
	_pregame_lobby_pane_0_text_34,
	_pregame_lobby_pane_0_text_35,
	_pregame_lobby_pane_0_text_36,
	_pregame_lobby_pane_0_text_37,
	_pregame_lobby_pane_0_text_38,
	_pregame_lobby_pane_0_text_39,
	_pregame_lobby_pane_0_text_40,
	_pregame_lobby_pane_0_text_41,
	_pregame_lobby_pane_0_text_42,
	_pregame_lobby_pane_0_text_party_leader,
	_pregame_lobby_pane_0_text_player_count,
	_pregame_lobby_pane_0_text_view_postgame,
	_pregame_lobby_pane_0_text_online_single_xbox_pregame_lobby_help,
	_pregame_lobby_pane_0_text_active_protocol,
	_pregame_lobby_pane_0_text_chat_body,
	_pregame_lobby_pane_0_text_map_download_progress,
	_pregame_lobby_pane_0_text_vote_status,
	_pregame_lobby_pane_0_text_vote_tally,
	_pregame_lobby_pane_0_text_vote_countdown,
	_pregame_lobby_pane_0_text_server_party_leader,
	k_pregame_lobby_pane_0_text_count,


	// pane 1 : cooperative

	_pregame_lobby_pane_1_text_game_cant_start_no_session = 0,
	_pregame_lobby_pane_1_text_level_description_label,
	_pregame_lobby_pane_1_text_level_format,
	_pregame_lobby_pane_1_text_difficulty_format,
	_pregame_lobby_pane_1_text_level_description_placeholder,
	_pregame_lobby_pane_1_text_party_privacy,
	_pregame_lobby_pane_1_text_6,
	_pregame_lobby_pane_1_text_7,
	_pregame_lobby_pane_1_text_8,
	_pregame_lobby_pane_1_text_9,
	_pregame_lobby_pane_1_text_10,
	_pregame_lobby_pane_1_text_11,
	_pregame_lobby_pane_1_text_12,
	_pregame_lobby_pane_1_text_13,
	_pregame_lobby_pane_1_text_14,
	_pregame_lobby_pane_1_text_15,
	_pregame_lobby_pane_1_text_16,
	_pregame_lobby_pane_1_text_17,
	_pregame_lobby_pane_1_text_18,
	_pregame_lobby_pane_1_text_19,
	_pregame_lobby_pane_1_text_20,
	_pregame_lobby_pane_1_text_21,
	_pregame_lobby_pane_1_text_22,
	_pregame_lobby_pane_1_text_23,
	_pregame_lobby_pane_1_text_24,
	_pregame_lobby_pane_1_text_25,
	_pregame_lobby_pane_1_text_26,
	_pregame_lobby_pane_1_text_27,
	_pregame_lobby_pane_1_text_28,
	_pregame_lobby_pane_1_text_29,
	_pregame_lobby_pane_1_text_30,
	_pregame_lobby_pane_1_text_31,
	_pregame_lobby_pane_1_text_32,
	_pregame_lobby_pane_1_text_33,
	_pregame_lobby_pane_1_text_34,
	_pregame_lobby_pane_1_text_35,
	_pregame_lobby_pane_1_text_36,
	_pregame_lobby_pane_1_text_37,
	_pregame_lobby_pane_1_text_party_leader,
	_pregame_lobby_pane_1_text_player_count,
	_pregame_lobby_pane_1_text_view_postgame,
	_pregame_lobby_pane_1_text_online_single_xbox_pregame_lobby_help,
	_pregame_lobby_pane_1_text_active_protocol,
	_pregame_lobby_pane_1_text_server_party_leader,
	
	//custom addition	
	_pregame_lobby_pane_1_text_chat_body,


	k_pregame_lobby_pane_1_text_count,
	k_pregame_lobby_pane_1_text_count_orignal = 44,
	k_number_of_pregame_pane_1_text_addons = k_pregame_lobby_pane_1_text_count - k_pregame_lobby_pane_1_text_count_orignal,

};

enum e_pregame_lobby_bitmap_blocks
{
	_pregame_lobby_pane_0_bitmap_name_field,
	_pregame_lobby_pane_0_bitmap_lobby_display,
	_pregame_lobby_pane_0_bitmap_unknown_map,
	_pregame_lobby_pane_0_bitmap_divider,
	_pregame_lobby_pane_0_bitmap_mp_games_type_icon,
	_pregame_lobby_pane_0_bitmap_item_background_1,
	_pregame_lobby_pane_0_bitmap_item_background_2,
	_pregame_lobby_pane_0_bitmap_item_background_3,
	_pregame_lobby_pane_0_bitmap_item_background_4,
	_pregame_lobby_pane_0_bitmap_item_background_5,
	_pregame_lobby_pane_0_bitmap_item_background_6,
	_pregame_lobby_pane_0_bitmap_item_background_7,
	_pregame_lobby_pane_0_bitmap_item_background_8,
	_pregame_lobby_pane_0_bitmap_item_background_9,
	_pregame_lobby_pane_0_bitmap_item_background_10,
	_pregame_lobby_pane_0_bitmap_item_background_11,
	_pregame_lobby_pane_0_bitmap_item_background_12,
	_pregame_lobby_pane_0_bitmap_item_background_13,
	_pregame_lobby_pane_0_bitmap_item_background_14,
	_pregame_lobby_pane_0_bitmap_item_background_15,
	_pregame_lobby_pane_0_bitmap_item_background_16,
	_pregame_lobby_pane_0_bitmap_speaker_1,
	_pregame_lobby_pane_0_bitmap_speaker_2,
	_pregame_lobby_pane_0_bitmap_speaker_3,
	_pregame_lobby_pane_0_bitmap_speaker_4,
	_pregame_lobby_pane_0_bitmap_speaker_5,
	_pregame_lobby_pane_0_bitmap_speaker_6,
	_pregame_lobby_pane_0_bitmap_speaker_7,
	_pregame_lobby_pane_0_bitmap_speaker_8,
	_pregame_lobby_pane_0_bitmap_speaker_9,
	_pregame_lobby_pane_0_bitmap_speaker_10,
	_pregame_lobby_pane_0_bitmap_speaker_11,
	_pregame_lobby_pane_0_bitmap_speaker_12,
	_pregame_lobby_pane_0_bitmap_speaker_13,
	_pregame_lobby_pane_0_bitmap_speaker_14,
	_pregame_lobby_pane_0_bitmap_speaker_15,
	_pregame_lobby_pane_0_bitmap_speaker_16,
	_pregame_lobby_pane_0_bitmap_settings_framing,
	_pregame_lobby_pane_0_bitmap_game_settings,// last h2x bitmap

	_pregame_lobby_pane_0_bitmap_arrows_up, // h2v starts from here
	_pregame_lobby_pane_0_bitmap_arrows_down,
	_pregame_lobby_pane_0_bitmap_favorites_icon,
	_pregame_lobby_pane_0_bitmap_download_progress_1,
	_pregame_lobby_pane_0_bitmap_download_progress_2,
	_pregame_lobby_pane_0_bitmap_download_progress_3,
	_pregame_lobby_pane_0_bitmap_download_progress_4,
	_pregame_lobby_pane_0_bitmap_download_progress_5,
	_pregame_lobby_pane_0_bitmap_download_progress_6,
	_pregame_lobby_pane_0_bitmap_download_progress_7,
	_pregame_lobby_pane_0_bitmap_download_progress_8,
	_pregame_lobby_pane_0_bitmap_download_progress_9,
	_pregame_lobby_pane_0_bitmap_download_progress_10,
	_pregame_lobby_pane_0_bitmap_download_progress_11,
	_pregame_lobby_pane_0_bitmap_download_progress_12,
	_pregame_lobby_pane_0_bitmap_download_progress_13,
	_pregame_lobby_pane_0_bitmap_download_progress_14,
	_pregame_lobby_pane_0_bitmap_download_progress_15,
	_pregame_lobby_pane_0_bitmap_download_progress_16,
	k_pregame_lobby_pane_0_bitmap_count,


	// pane 1 : cooperative

	_pregame_lobby_pane_1_bitmap_name_field = 0,
	_pregame_lobby_pane_1_bitmap_lobby_display,
	_pregame_lobby_pane_1_bitmap_dervish_placeholder_map,
	_pregame_lobby_pane_1_bitmap_divider,
	_pregame_lobby_pane_1_bitmap_difficulty_icon,
	_pregame_lobby_pane_1_bitmap_item_background_1,
	_pregame_lobby_pane_1_bitmap_item_background_2,
	_pregame_lobby_pane_1_bitmap_item_background_3,
	_pregame_lobby_pane_1_bitmap_item_background_4,
	_pregame_lobby_pane_1_bitmap_item_background_5,
	_pregame_lobby_pane_1_bitmap_item_background_6,
	_pregame_lobby_pane_1_bitmap_item_background_7,
	_pregame_lobby_pane_1_bitmap_item_background_8,
	_pregame_lobby_pane_1_bitmap_item_background_9,
	_pregame_lobby_pane_1_bitmap_item_background_10,
	_pregame_lobby_pane_1_bitmap_item_background_11,
	_pregame_lobby_pane_1_bitmap_item_background_12,
	_pregame_lobby_pane_1_bitmap_item_background_13,
	_pregame_lobby_pane_1_bitmap_item_background_14,
	_pregame_lobby_pane_1_bitmap_item_background_15,
	_pregame_lobby_pane_1_bitmap_item_background_16,
	_pregame_lobby_pane_1_bitmap_speaker_1,
	_pregame_lobby_pane_1_bitmap_speaker_2,
	_pregame_lobby_pane_1_bitmap_speaker_3,
	_pregame_lobby_pane_1_bitmap_speaker_4,
	_pregame_lobby_pane_1_bitmap_speaker_5,
	_pregame_lobby_pane_1_bitmap_speaker_6,
	_pregame_lobby_pane_1_bitmap_speaker_7,
	_pregame_lobby_pane_1_bitmap_speaker_8,
	_pregame_lobby_pane_1_bitmap_speaker_9,
	_pregame_lobby_pane_1_bitmap_speaker_10,
	_pregame_lobby_pane_1_bitmap_speaker_11,
	_pregame_lobby_pane_1_bitmap_speaker_12,
	_pregame_lobby_pane_1_bitmap_speaker_13,
	_pregame_lobby_pane_1_bitmap_speaker_14,
	_pregame_lobby_pane_1_bitmap_speaker_15,
	_pregame_lobby_pane_1_bitmap_speaker_16,
	_pregame_lobby_pane_1_bitmap_settings_framing,
	_pregame_lobby_pane_1_bitmap_game_settings,// last h2x bitmap

	// h2v starts here...these were orignally broken
	_pregame_lobby_pane_1_bitmap_arrows_up, 
	_pregame_lobby_pane_1_bitmap_arrows_down,
	_pregame_lobby_pane_1_bitmap_favorites_icon,

	k_pregame_lobby_pane_1_bitmap_count,
};

const e_pregame_lobby_text_blocks c_screen_pregame_lobby_text_pane_1_mapping(const e_pregame_lobby_text_blocks pane_1)
{
	switch (pane_1)
	{
	case _pregame_lobby_pane_1_text_game_cant_start_no_session:
		return _pregame_lobby_pane_0_text_game_cant_start_no_session;


	case _pregame_lobby_pane_1_text_level_description_label:
		return _pregame_lobby_pane_0_text_gametype_options_format;


	case _pregame_lobby_pane_1_text_level_format:
		return _pregame_lobby_pane_0_text_gametype_format;


	case _pregame_lobby_pane_1_text_difficulty_format:
		return _pregame_lobby_pane_0_text_mapname_format;


	case _pregame_lobby_pane_1_text_level_description_placeholder:
		return _pregame_lobby_pane_0_text_4;


	case _pregame_lobby_pane_1_text_party_privacy:
		return _pregame_lobby_pane_0_text_party_privacy;


	case _pregame_lobby_pane_1_text_6:
		return _pregame_lobby_pane_0_text_11;


	case _pregame_lobby_pane_1_text_7:
		return _pregame_lobby_pane_0_text_12;


	case _pregame_lobby_pane_1_text_8:
		return _pregame_lobby_pane_0_text_13;


	case _pregame_lobby_pane_1_text_9:
		return _pregame_lobby_pane_0_text_14;


	case _pregame_lobby_pane_1_text_10:
		return _pregame_lobby_pane_0_text_15;


	case _pregame_lobby_pane_1_text_11:
		return _pregame_lobby_pane_0_text_16;


	case _pregame_lobby_pane_1_text_12:
		return _pregame_lobby_pane_0_text_17;


	case _pregame_lobby_pane_1_text_13:
		return _pregame_lobby_pane_0_text_18;


	case _pregame_lobby_pane_1_text_14:
		return _pregame_lobby_pane_0_text_19;


	case _pregame_lobby_pane_1_text_15:
		return _pregame_lobby_pane_0_text_20;


	case _pregame_lobby_pane_1_text_16:
		return _pregame_lobby_pane_0_text_21;


	case _pregame_lobby_pane_1_text_17:
		return _pregame_lobby_pane_0_text_22;


	case _pregame_lobby_pane_1_text_18:
		return _pregame_lobby_pane_0_text_23;


	case _pregame_lobby_pane_1_text_19:
		return _pregame_lobby_pane_0_text_24;


	case _pregame_lobby_pane_1_text_20:
		return _pregame_lobby_pane_0_text_25;


	case _pregame_lobby_pane_1_text_21:
		return _pregame_lobby_pane_0_text_26;


	case _pregame_lobby_pane_1_text_22:
		return _pregame_lobby_pane_0_text_27;


	case _pregame_lobby_pane_1_text_23:
		return _pregame_lobby_pane_0_text_28;


	case _pregame_lobby_pane_1_text_24:
		return _pregame_lobby_pane_0_text_29;


	case _pregame_lobby_pane_1_text_25:
		return _pregame_lobby_pane_0_text_30;


	case _pregame_lobby_pane_1_text_26:
		return _pregame_lobby_pane_0_text_31;


	case _pregame_lobby_pane_1_text_27:
		return _pregame_lobby_pane_0_text_32;


	case _pregame_lobby_pane_1_text_28:
		return _pregame_lobby_pane_0_text_33;


	case _pregame_lobby_pane_1_text_29:
		return _pregame_lobby_pane_0_text_34;


	case _pregame_lobby_pane_1_text_30:
		return _pregame_lobby_pane_0_text_35;


	case _pregame_lobby_pane_1_text_31:
		return _pregame_lobby_pane_0_text_36;


	case _pregame_lobby_pane_1_text_32:
		return _pregame_lobby_pane_0_text_37;


	case _pregame_lobby_pane_1_text_33:
		return _pregame_lobby_pane_0_text_38;


	case _pregame_lobby_pane_1_text_34:
		return _pregame_lobby_pane_0_text_39;


	case _pregame_lobby_pane_1_text_35:
		return _pregame_lobby_pane_0_text_40;


	case _pregame_lobby_pane_1_text_36:
		return _pregame_lobby_pane_0_text_41;


	case _pregame_lobby_pane_1_text_37:
		return _pregame_lobby_pane_0_text_42;


	case _pregame_lobby_pane_1_text_party_leader:
		return _pregame_lobby_pane_0_text_party_leader;


	case _pregame_lobby_pane_1_text_player_count:
		return _pregame_lobby_pane_0_text_player_count;


	case _pregame_lobby_pane_1_text_view_postgame:
		return _pregame_lobby_pane_0_text_view_postgame;


	case _pregame_lobby_pane_1_text_online_single_xbox_pregame_lobby_help:
		return _pregame_lobby_pane_0_text_online_single_xbox_pregame_lobby_help;


	case _pregame_lobby_pane_1_text_active_protocol:
		return _pregame_lobby_pane_0_text_active_protocol;


	case _pregame_lobby_pane_1_text_server_party_leader:
		return _pregame_lobby_pane_0_text_server_party_leader;


	}
	return k_pregame_lobby_pane_1_text_count;
}

void fix_server_party_leader_texts(e_pregame_pane_type pane_type)
{
	e_pregame_lobby_text_blocks party_leader_text_id = _pregame_lobby_pane_0_text_party_leader;
	e_pregame_lobby_text_blocks server_party_leader_text_id = _pregame_lobby_pane_0_text_server_party_leader;

	if (pane_type == _pregame_pane_custom_game)
	{
		party_leader_text_id = _pregame_lobby_pane_0_text_party_leader;
		server_party_leader_text_id = _pregame_lobby_pane_0_text_server_party_leader;
	}
	else if (pane_type == _pregame_pane_cooperative)
	{
		party_leader_text_id = _pregame_lobby_pane_1_text_party_leader;
		server_party_leader_text_id = _pregame_lobby_pane_1_text_server_party_leader;
	}
	// fixes code breaking _pregame_lobby_pane_1_text_party_leader
	// fixes code breaking _pregame_lobby_pane_1_text_server_party_leader
	WriteValue<uint8>(Memory::GetAddress(0x2452DD) + 1, TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(party_leader_text_id));
	WriteValue<uint8>(Memory::GetAddress(0x245275) + 1, TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(server_party_leader_text_id));
}

void c_screen_multiplayer_pregame_lobby::initialize_long_text_chat()
{
	e_pregame_lobby_text_blocks chat_box_receiver = _pregame_lobby_pane_0_text_chat_body;
	if (m_pregame_pane_type == _pregame_pane_cooperative)
	{
		chat_box_receiver = _pregame_lobby_pane_1_text_chat_body;

	}
	else if (m_pregame_pane_type == _pregame_pane_custom_game)
	{
		chat_box_receiver = _pregame_lobby_pane_0_text_chat_body;
	}
	else
	{
		// complete this once we have matchmaking lobby
		return;
	}

	c_text_widget* chat_box_text = try_find_text_widget(TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(chat_box_receiver));
	if (chat_box_text && this->m_communications_allowed)
	{
		rectangle2d chat_text_bounds;
		chat_box_text->get_unprojected_bounds(&chat_text_bounds);

		const real_rgb_color* rgb_text_color = chat_box_text->get_interface()->get_color();
		real_argb_color argb_text_color;
		argb_text_color.alpha = 1.0f;
		argb_text_color.red = rgb_text_color->red;
		argb_text_color.blue = rgb_text_color->blue;
		argb_text_color.green = rgb_text_color->green;

		this->add_new_child(&this->m_long_chat_text);

		this->m_long_chat_text.set_text_properties(
			text_flag_left_justify_text,
			chat_box_text->get_animation_type(),
			&argb_text_color,
			chat_box_text->get_interface()->get_font(),
			&chat_text_bounds);

		this->m_long_chat_text.set_field74();
	}

}

void c_screen_multiplayer_pregame_lobby::update_protocol()
{
	//INVOKE_TYPE(0x24300D, 0x0, void(__thiscall*)(c_screen_multiplayer_pregame_lobby*), this);

	e_pregame_pane_type pane_type = this->m_pregame_pane_type;
	switch (user_interface_squad_get_active_protocol())
	{
	case _session_protocol_splitscreen_coop:
	case _session_protocol_system_link_coop:
	case _session_protocol_xbox_live_coop:
		pane_type = _pregame_pane_cooperative;
		break;
	case _session_protocol_splitscreen_custom:
	case _session_protocol_system_link_custom:
	case _session_protocol_xbox_live_custom:
		pane_type = _pregame_pane_custom_game;
		break;
	case _session_protocol_xbox_live_optimatch:
		pane_type = _pregame_pane_optimatch;
		break;
	}

	if (pane_type != this->m_pregame_pane_type)
	{
		int32 new_pane_type = (int32)pane_type;
		this->switch_panes(&new_pane_type);
		this->m_pregame_pane_type = pane_type;

		// add lobby_switch_codes_here
		initialize_long_text_chat();
		fix_server_party_leader_texts(pane_type);
		update_chat_icons();
	}
	update_favourites_icons();

}
__declspec(naked) void jmp_update_protocol() { __asm { jmp c_screen_multiplayer_pregame_lobby::update_protocol } }


void c_screen_multiplayer_pregame_lobby::update_chat_icons()
{
	e_pregame_lobby_bitmap_blocks up_arrow_icon_id = _pregame_lobby_pane_0_bitmap_arrows_up;
	e_pregame_lobby_bitmap_blocks down_arrow_icon_id = _pregame_lobby_pane_0_bitmap_arrows_down;
	if (m_pregame_pane_type == _pregame_pane_custom_game)
	{
		up_arrow_icon_id = _pregame_lobby_pane_0_bitmap_arrows_up;
		down_arrow_icon_id = _pregame_lobby_pane_0_bitmap_arrows_down;

		c_text_widget* map_download_progress_text = try_find_text_widget(TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(_pregame_lobby_pane_0_text_map_download_progress));
		if (map_download_progress_text)
		{
			rectangle2d new_bounds;
			map_download_progress_text->get_bounds(&new_bounds);
			new_bounds.bottom = 2 * new_bounds.bottom - new_bounds.top;
			map_download_progress_text->set_bounds(&new_bounds);
		}
	}
	else if (m_pregame_pane_type == _pregame_pane_cooperative)
	{
		up_arrow_icon_id = _pregame_lobby_pane_1_bitmap_arrows_up;
		down_arrow_icon_id = _pregame_lobby_pane_1_bitmap_arrows_down;
	}
	else
	{
		// complete this once we have matchmaking lobby
		return;
	}

	this->m_special_widgets[_special_widget_bitmap_arrow_up] = (c_user_interface_widget*)try_find_bitmap_widget(up_arrow_icon_id);
	this->m_special_widgets[_special_widget_bitmap_arrow_down] = (c_user_interface_widget*)try_find_bitmap_widget(down_arrow_icon_id);
	
	if (!this->m_communications_allowed)
	{
		if (this->m_special_widgets[_special_widget_bitmap_arrow_up])
			this->m_special_widgets[_special_widget_bitmap_arrow_up]->set_visible(false);

		if (this->m_special_widgets[_special_widget_bitmap_arrow_down])
			this->m_special_widgets[_special_widget_bitmap_arrow_down]->set_visible(false);
	}

}

void c_screen_multiplayer_pregame_lobby::update_favourites_icons()
{
	e_pregame_lobby_bitmap_blocks favourites_icon_id = _pregame_lobby_pane_0_bitmap_favorites_icon;
	if (m_pregame_pane_type == _pregame_pane_custom_game)
	{
		favourites_icon_id = _pregame_lobby_pane_0_bitmap_favorites_icon;
	}
	else if (m_pregame_pane_type == _pregame_pane_cooperative)
	{
		favourites_icon_id = _pregame_lobby_pane_1_bitmap_favorites_icon;
	}
	else
	{
		// complete this once we have matchmaking lobby
		return;
	}
	this->m_special_widgets[_special_widget_bitmap_favourites] = (c_user_interface_widget*)this->try_find_bitmap_widget(favourites_icon_id);
	if (this->m_special_widgets[_special_widget_bitmap_favourites])
	{
		this->m_special_widgets[_special_widget_bitmap_favourites]->set_visible(user_interface_squad_session_is_xbox_live());
	}
}


void c_screen_multiplayer_pregame_lobby::apply_instance_patches()
{
	// should probably replace these with asserts
	NopFill(Memory::GetAddress(0x244F0A), 4); //	vote_status_text_block->m_visible = 0; 
	NopFill(Memory::GetAddress(0x244F0E), 4); //	vote_tally_text_block->m_visible = 0;
	NopFill(Memory::GetAddress(0x244F15), 4); //	vote_countdown_text_block->m_visible = 0;

	// fix chatbox for cooperative_pane
	WriteValue<uint8>(Memory::GetAddress(0x2435AE) + 1, TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(_pregame_lobby_pane_1_text_chat_body));
	PatchCall(Memory::GetAddress(0x24589A), jmp_update_protocol); //inside c_screen_multiplayer_pregame_lobby::update

}

void c_screen_multiplayer_pregame_lobby::apply_patches_on_map_load()
{
	const char* main_widget_tag_path = "ui\\screens\\game_shell\\pregame_lobby\\pregame_lobby";

	datum main_widget_datum_index = tags::find_tag(_tag_group_user_interface_screen_widget_definition, main_widget_tag_path);

	if (DATUM_IS_NONE(main_widget_datum_index))
	{
		LOG_ERROR_FUNC("bad datum found");
		return;
	}

	s_user_interface_screen_widget_definition* main_widget_tag = tags::get_tag_fast<s_user_interface_screen_widget_definition>(main_widget_datum_index);

	s_window_pane_reference* custom_games_pane = main_widget_tag->panes[0];
	s_window_pane_reference* cooperative_pane = main_widget_tag->panes[1];

	//	add missing text_chat block
	MetaExtender::add_tag_block3<s_bitmap_block_reference>((uint32)&cooperative_pane->text_blocks, k_number_of_pregame_pane_1_text_addons);

	//	fix bitmap offsets
	for (uint8 block_idx = 0; block_idx <= _pregame_lobby_pane_1_bitmap_game_settings; block_idx++)
	{
		cooperative_pane->bitmap_blocks[block_idx]->topleft = custom_games_pane->bitmap_blocks[block_idx]->topleft;
	}
	//	_pregame_lobby_pane_1_bitmap_name_field bitmap is not used in h2v 
	//	creates a big box in the middle so nopped the bitmap it references
	//	edit : turns out its the remnant of y-menu
	cooperative_pane->bitmap_blocks[_pregame_lobby_pane_1_bitmap_name_field]->bitmap_tag.index = NONE;
	//copy data for broken bitmap blocks
	csmemcpy(cooperative_pane->bitmap_blocks[_pregame_lobby_pane_1_bitmap_arrows_up], custom_games_pane->bitmap_blocks[_pregame_lobby_pane_0_bitmap_arrows_up], sizeof(s_bitmap_block_reference));
	csmemcpy(cooperative_pane->bitmap_blocks[_pregame_lobby_pane_1_bitmap_arrows_down], custom_games_pane->bitmap_blocks[_pregame_lobby_pane_0_bitmap_arrows_down], sizeof(s_bitmap_block_reference));
	csmemcpy(cooperative_pane->bitmap_blocks[_pregame_lobby_pane_1_bitmap_favorites_icon], custom_games_pane->bitmap_blocks[_pregame_lobby_pane_0_bitmap_favorites_icon], sizeof(s_bitmap_block_reference));


	//	fix player fields
	s_player_block_reference* custom_games_player_block = custom_games_pane->player_blocks[0];
	s_player_block_reference* cooperative_player_block = cooperative_pane->player_blocks[0];
	cooperative_player_block->bottomleft = custom_games_player_block->bottomleft;
	cooperative_player_block->row_height = custom_games_player_block->row_height;

	//	change "ui\player_skins\player_skin_default.skin" to "ui\player_skins\player_skin_lobby.skin"
	cooperative_player_block->skin.index = custom_games_player_block->skin.index;




	//	fix button bounds
	for (uint8 block_idx = 0; block_idx < k_pregame_lobby_pane_1_button_count; block_idx++)
	{
		cooperative_pane->buttons[block_idx]->bounds = custom_games_pane->buttons[block_idx]->bounds;
		cooperative_pane->buttons[block_idx]->bitmap_offset = custom_games_pane->buttons[block_idx]->bitmap_offset;
	}

	//	fix text bounds
	for (uint8 block_idx = 0; block_idx < k_pregame_lobby_pane_1_text_count_orignal; block_idx++)
	{
		const uint8 custom_game_block_idx = c_screen_pregame_lobby_text_pane_1_mapping((e_pregame_lobby_text_blocks)block_idx);
		cooperative_pane->text_blocks[block_idx]->text_bounds = custom_games_pane->text_blocks[custom_game_block_idx]->text_bounds;
	}

	//	_pregame_lobby_pane_1_text_level_description_placeholder bottom and right still need to be adjusted
	cooperative_pane->text_blocks[_pregame_lobby_pane_1_text_level_description_placeholder]->text_bounds.right = -290;
	cooperative_pane->text_blocks[_pregame_lobby_pane_1_text_level_description_placeholder]->text_bounds.bottom = -400;

	//copy data for _pregame_lobby_pane_1_text_text_chat_body block
	csmemcpy(cooperative_pane->text_blocks[_pregame_lobby_pane_1_text_chat_body], custom_games_pane->text_blocks[_pregame_lobby_pane_0_text_chat_body], sizeof(s_text_block_reference));

}
