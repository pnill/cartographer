#pragma once
#include "game/game_allegiance.h"
#include "interface/user_interface_widget_window.h"
#include "interface/user_interface_widget_text.h"
#include "interface/user_interface_widget_button.h"
#include "interface/user_interface_widget_text_entry.h"
#include "interface/signal_slot.h"

/* macro defines */

/* enums */

enum e_pregame_pane_type : uint16
{
	_pregame_pane_custom_game = 0,
	_pregame_pane_cooperative,
	_pregame_pane_optimatch,
	k_total_number_of_pregame_pane_types,
};

/* forward declarations */

/* structures */

struct s_pregame_controller_team_mapping
{
	bool active;
	e_game_team team;
};
ASSERT_STRUCT_SIZE(s_pregame_controller_team_mapping, 4);


/* classes */

class c_screen_multiplayer_pregame_lobby : protected c_screen_widget
{
protected:
	c_button_widget m_start_game_button;
	c_button_widget m_game_setup_button;
	c_button_widget m_find_game_button;
	c_text_entry_widget_for_pregame_lobby m_text_chat_box_button;
	c_button_widget m_game_details_button;
	bool field_13A4;
	uint8 gap_13A5[3];
	uint32 field_13A8;
	c_long_text_widget m_long_chat_text;
	bool field_1C68;
	uint8 field_1C69;
	bool m_communications_allowed;
	uint8 gap_1C6B;
	s_pregame_controller_team_mapping local_team_mapping[4];
	uint8 gap_1C79[6144];
	uint32 m_countdown_timer;
	uint32 m_countdown_timer_end_time;
	e_pregame_pane_type m_pregame_pane_type;
	bool field_3486;
	uint8 gap_3487;
	c_slot2<c_screen_multiplayer_pregame_lobby, s_event_record*, int16> start_game_button_slot;
	c_slot2<c_screen_multiplayer_pregame_lobby, s_event_record*, int16> game_setup_button_slot;
	c_slot2<c_screen_multiplayer_pregame_lobby, s_event_record*, int16> find_game_button_slot;
	c_slot2<c_screen_multiplayer_pregame_lobby, s_event_record*, int16> text_chatbox_button_slot;
	c_slot2<c_screen_multiplayer_pregame_lobby, s_event_record*, int16> game_details_button_slot;
	bool field_3500;
	uint8 gap_3501[3];
	uint32 field_3504;
	bool field_3508;
	uint8 gap_3509[3];
	uint32 field_350C;
	uint32 field_3510;
	uint32 m_map_id;
	uint8 field_3518[96];
	bool field_3578;
	uint8 field_3579;
	uint8 gap_357A[2];
	uint32 field_357C;
	uint32 m_map_download_progress_state;
	rectangle2d start_game_button_bounds;
	rectangle2d game_details_button_bounds;


	void initialize_long_text_chat();
	void update_protocol();
	void update_chat_icons();
	void update_favourites_icons();

public:
	static void apply_instance_patches();
	static void apply_patches_on_map_load();
};
ASSERT_STRUCT_SIZE(c_screen_multiplayer_pregame_lobby, 0x3594);
