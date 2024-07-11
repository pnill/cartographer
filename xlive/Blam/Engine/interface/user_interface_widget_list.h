#pragma once
#include "user_interface_widget.h"
#include "signal_slot.h"
#include "memory/data.h"
#include "tag_files/string_id.h"

/* forward declarations */

class c_list_item_widget;


/* structures */

struct s_item_text_mapping
{
	int16 item_id;
	string_id item_text;
};
ASSERT_STRUCT_SIZE(s_item_text_mapping, 8);

struct s_dynamic_list_item
{
	uint16 unk;
	int16 item_id;
};
ASSERT_STRUCT_SIZE(s_dynamic_list_item, 4);


/* classes */

class c_list_widget : public c_user_interface_widget
{
protected:
	s_data_array* m_list_data;
	uint16 m_up_arrow_transition_time;
	uint16 m_down_arrow_transition_time;
	int32 m_intro_delay_milliseconds;
	uint16 m_tabbing_count;
	uint8 gap_7E[2];
	real32 m_up_arrows_position_x0;
	real32 m_up_arrows_position_x1;
	real32 m_up_arrows_position_y0;
	real32 m_up_arrows_position_y1;
	real32 m_down_arrows_position_x0;
	real32 m_down_arrows_position_x1;
	real32 m_down_arrows_position_y0;
	real32 m_down_arrows_position_y1;
	bool m_up_arrow_drawn;
	bool m_down_arrow_drawn;
	bool field_A2;
	bool m_list_wraps;
	bool m_list_interactive;
	bool m_list_has_hidden_items;
	bool field_A6;
	bool field_A7;
	_slot1<>* signal1;
	_slot2<>* signal2;

public:
	c_list_widget(int16 user_flags);
	c_list_item_widget* try_find_item_widget(uint32 idx);
	datum get_old_data_index();
	void update_list_items_from_mapping(c_list_item_widget* item, int32 skin_index, int32 text_widget_idx, s_item_text_mapping* mapping, int32 total_mappings);
	void set_focused_item_index(datum item_index);
	void remove_focused_item_datum_from_data_array();
	void remove_item_from_list(c_list_item_widget* item);


	// c_list_widget virtual functions

	virtual ~c_list_widget();
	virtual int32 setup_children() override;
	virtual void on_screen_leave() override;
	virtual void update() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual int32 get_intro_delay() override;
	virtual uint8 handle_event(s_event_record* event) override;
	virtual void construct_animation_on_region_enter(int32 a1) override;
	virtual c_user_interface_text* get_interface() override;

	// c_list_widget additions

	virtual int32 link_item_widgets();
	virtual c_list_item_widget* get_list_items() = 0;
	virtual int32 get_list_items_count() = 0;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) = 0;
	virtual bool verify_item_in_focus(c_list_item_widget* item);
};
ASSERT_STRUCT_SIZE(c_list_widget, 0xB0);