#pragma once
#include "user_interface_widget.h"
#include "signal_slot.h"
#include "memory/data.h"
#include "tag_files/string_id.h"

/* forward declarations */

class c_list_item_widget;

/* enums */

enum e_default_list_skin_texts
{
	_default_list_skin_text_main = 0,
	k_number_of_default_list_skin_texts,
};


/* structures */

struct s_item_text_mapping
{
	int16 item_id;
	string_id item_text;
};
ASSERT_STRUCT_SIZE(s_item_text_mapping, 8);

struct s_list_item_datum
{
	uint16 unk;
	int16 item_id;
};
ASSERT_STRUCT_SIZE(s_list_item_datum, 4);


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
	_slot_linker linker_type1;
	_slot_linker linker_type2;

public:
	c_list_widget(uint16 user_flags);

	c_list_item_widget* try_find_item_widget(uint32 idx);
	datum get_old_data_index();
	void update_list_items_from_mapping(c_list_item_widget* item, int32 skin_index, int32 text_widget_idx, s_item_text_mapping* mapping, int32 total_mappings);
	void set_focused_item_index(datum item_index);
	void remove_focused_item_datum_from_data_array();
	void remove_item_from_list(c_list_item_widget* item);


	// c_list_widget virtual functions

	virtual ~c_list_widget() = default;
	virtual int32 setup_children() override;
	virtual void pre_destroy() override;
	virtual void update() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual int32 get_intro_delay() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void construct_animation_on_region_enter(int32 a1) override;
	virtual c_user_interface_text* get_interface() override;

	// c_list_widget additions

	virtual int32 link_item_widgets();
	virtual c_list_item_widget* get_list_items() = 0;
	virtual int32 get_list_items_count() = 0;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) = 0;
	virtual bool verify_item_in_focus(c_list_item_widget* item);

private:
	template<typename T>
	static T get_base_vtable_fn_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x3CF5FC)[idx]);
	}
};
ASSERT_STRUCT_SIZE(c_list_widget, 0xB0);