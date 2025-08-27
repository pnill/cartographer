#pragma once
#include "interface/user_interface_widget.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"
#include "saved_games/saved_game_files.h"

#define create_game_engine_functions(name) \
	static void* load_##name##_settings(s_screen_parameters* parameters); \
	static void* load_##name##_lobby(s_screen_parameters* parameters); \
	static void* load_##name##_unused(s_screen_parameters* parameters) \

#define k_custom_game_profile_list_name "custom game profile list"

enum e_screen_game_engine_category_type : int8;

enum e_screen_game_profile_list_items : uint16
{

	k_game_profile_list_item_count = 15
};

struct s_custom_game_profile_list_item
{
	int8 data[2];
	int16 unk_1;
	enumerated_file_index enumerated_index;
	string_id description_text;
};
ASSERT_STRUCT_SIZE(s_custom_game_profile_list_item, 12);

class c_custom_game_profile_list : public c_list_widget
{
public:
	c_list_item_widget m_list_items[k_game_profile_list_item_count];
	int32 enumerated_file_index_storage[k_maximum_enumerated_total_save_game_files];
	e_saved_game_file_type save_game_type;
	int32 unk_1;
	int32 enumerated_index;
	int8 data_2[2];
	bool unk_bool;
	int8 data_3;
	c_slot2<c_custom_game_profile_list, s_event_record*, datum> m_slot;
	void handle_item_pressed_event(s_event_record** pevent, datum* pitem_index);
	void perform_save_type_search();
	void sub_6515A6(); // FO: 0x2515A6

	c_custom_game_profile_list(int16 user_flags);

	virtual void setup_children() override;
	virtual void update() override;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;
};
ASSERT_STRUCT_SIZE(c_custom_game_profile_list, 0x4A28);

class c_screen_custom_game_profile_select : public c_screen_with_menu
{
protected:
	c_custom_game_profile_list m_profile_list;
	e_saved_game_file_type save_game_type;
	int8 unk_data[4];

private:
	static void* load_settings(s_screen_parameters* parameters, e_saved_game_file_type type);
	static void* load_lobby(s_screen_parameters* parameters, e_saved_game_file_type type);
	static void* load_unused(s_screen_parameters* parameters, e_saved_game_file_type type);

public:

	create_game_engine_functions(slayer);
	create_game_engine_functions(king);
	create_game_engine_functions(oddball);
	create_game_engine_functions(juggernaut);
	create_game_engine_functions(ctf);
	create_game_engine_functions(assault);
	create_game_engine_functions(territories);
	create_game_engine_functions(headhunter);

	void update() override;
	virtual void sub_60E884() override;
	const void* load_proc() const override;

	c_screen_custom_game_profile_select(e_user_interface_channel_type ui_channel, e_user_interface_render_window window_index, uint16 user_flags, e_user_interface_screen_id screen_id);

	static void set_global_enumerated_index(enumerated_file_index enumerated_file_index);
};

ASSERT_STRUCT_SIZE(c_screen_custom_game_profile_select, 21648);

#undef create_game_engine_functions
