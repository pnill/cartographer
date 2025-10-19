#pragma once
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* enums */

enum e_screen_network_squad_browser_columns : uint32
{
	_screen_network_squad_browser_column_favorite,
	_screen_network_squad_browser_column_dedicated_server,
	_screen_network_squad_browser_column_host_name,
	_screen_network_squad_browser_column_map,
	_screen_network_squad_browser_column_custom_map,
	_screen_network_squad_browser_column_game_type,
	_screen_network_squad_browser_column_variant,
	_screen_network_squad_browser_column_players,
	_screen_network_squad_browser_column_description,

	k_screen_network_squad_browser_column_count
};

/* structures */

struct s_network_squad_list_sort_fields
{
	int32 field_0;
	int32 session_index;
	bool field_8;
};

/* classes */

class c_network_squad_list : public c_list_widget
{	
public:
	c_network_squad_list(uint16 user_flags);
	virtual ~c_network_squad_list() = default;

	virtual void pre_destroy() override;
	virtual void update() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget*, int32 skin_index) override;


	void sort_event(s_event_record* event, int32 selected_column_index);

private:
	c_list_item_widget* m_list;
	int32 m_list_count;
	c_list_item_widget m_live_list[20];
	c_list_item_widget m_network_list[18];
	int8 m_field_1450;
	int8 pad[3];
	int32 m_field_1454;
	c_slot2<c_network_squad_list, s_event_record*, int32> m_slot;
	s_network_squad_list_sort_fields m_sort_fields[3];
	int32 m_field_1494;
	int32 field_1498;
	bool m_populate_server_list;
	bool m_use_live_squad_list;
	int16 field_149E;

	void populate_servers(void);
	static void populate_server_data(
		struct s_locator_server_data* data,
		int32 unk_2,
		int32 unk_3,
		int32 field,
		int32 available_squad_index,
		s_network_squad_list_sort_fields* sort_fields,
		int32 sort_fields_count,
		int32 sort_field_index,
		bool use_live_squad_list
	);
};
ASSERT_STRUCT_SIZE(c_network_squad_list, 5280);

class c_screen_network_squad_browser : c_screen_with_menu
{
	c_network_squad_list m_list;
	int16 m_live_list;
	bool m_list_is_sorted;
	int8 field_1F03;
	int32 m_selected_column_index;
	int32 field_1F08;

	static c_screen_network_squad_browser* __cdecl load_network_browser(s_screen_parameters* parameters);
	static c_screen_network_squad_browser* __cdecl load_live_browser(s_screen_parameters* parameters);

public:
	static void apply_patches(void);

	c_screen_network_squad_browser(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);
	virtual ~c_screen_network_squad_browser() = default;

	void build_players_list_fix(c_player_widget_representation* representations, int32 player_count);

	static void* __cdecl load(s_screen_parameters* parameters);
};
ASSERT_STRUCT_SIZE(c_screen_network_squad_browser, 0x1F0C);
