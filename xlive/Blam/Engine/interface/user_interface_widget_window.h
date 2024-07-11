#pragma once
#include "user_interface_widget.h"
#include "user_interface_widget_text.h"
#include "user_interface_screen_widget_definition.h"
#include "signal_slot.h"

/* macro defines */

#define K_MAXIMUM_NUMBER_OF_SPECIAL_WIDGETS 16

#define K_HEADER_TEXT_BLOCK_INDEX 0
#define K_BUTTON_KEY_TEXT_BLOCK_INDEX 1
#define K_MINIMUM_NUMBER_OF_DEFAULT_SCREEN_TEXTS 2

#define TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(block_idx) \
			(block_idx + K_MINIMUM_NUMBER_OF_DEFAULT_SCREEN_TEXTS)



/* forward declarations */

class c_button_widget;
class c_tab_view_widget;
class c_player_widget_representation;


/* structures */

// todo : need to verify
struct s_interface_expected_pane
{
	c_button_widget** expected_buttons;
	c_list_widget* expected_list;
	uint32 buttons_count;
	bool list_exists;
};
ASSERT_STRUCT_SIZE(s_interface_expected_pane, 0x10);

// todo : need to verify
struct s_interface_expected_screen_layout
{
	c_tab_view_widget* tab_view;
	int32 panes_count;
	int32 field_8;
	s_interface_expected_pane panes[6];
};
ASSERT_STRUCT_SIZE(s_interface_expected_screen_layout, 0x6C);


/* classes */

class c_screen_widget : protected c_user_interface_widget
{
protected:
	e_screen_id m_screen_id; //should be 4 bytes?
	uint16 pad;
	e_user_interface_channel_type m_channel_type;
	e_user_interface_render_window m_window_index;
	int32 m_child_count;
	c_normal_text_widget m_header_text;
	c_normal_text_widget m_screen_button_key_text;
	int16 m_pane_index;
	bool field_9FA;
	bool field_9FB;
	bool m_disable_overlay_effect;
	bool field_9FD;
	bool field_9FE;
	bool field_9FF;
	bool field_A00;
	bool field_A01;
	uint8 gap_A02[2];
	c_user_interface_widget* m_special_widgets[K_MAXIMUM_NUMBER_OF_SPECIAL_WIDGETS];
	c_slot1<c_screen_widget, long> m_screen_slot;


	void destroy();
	char switch_panes(long* pane_index_ptr);

public:
	c_screen_widget(e_screen_id menu_id, e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);
	
	void verify_and_load_from_layout(datum widget_tag, s_interface_expected_screen_layout* expected_layout);
	void apply_new_representations_to_players(c_player_widget_representation* representations, int32 player_count);
	void* get_screen_definition();
	void animate_screen(int32 type);

	// c_screen_widget virtual functions

	virtual ~c_screen_widget();
	virtual char handle_event(s_event_record* event) override;
	virtual c_user_interface_text* get_interface() override;
	virtual bool sub_6114B9() override;

	// c_screen_widget additions

	virtual void sub_60E884();
	virtual void initialize(s_screen_parameters* parameters) = 0;
	virtual void cache_local_bitmaps_and_cursor();
	virtual void setup_special_widgets();
	virtual c_user_interface_widget* sub_6102C5();
	virtual char sub_6103D6();
	virtual int sub_60F1F4(s_event_record* a2);
	virtual char sub_60EFC1(s_event_record* event);
	virtual int sub_60F081(s_event_record* a2);
	virtual int sub_60F151(int a2);
	virtual char sub_40AD53();
	virtual e_user_interface_channel_type  get_channel();
	virtual e_user_interface_render_window  get_render_window();
	virtual int sub_60EB92(int a2);
	virtual void sub_60EBC2(int a1);
	virtual void* sub_60EC5C(s_screen_state* state);
	virtual void sub_60ECC9(s_screen_state* state);
	virtual void* load_proc() = 0;
	virtual bool overlay_effect_is_disabled();
	virtual void sub_60F2A4(char bitmap_index);

};
ASSERT_STRUCT_SIZE(c_screen_widget, 0xA5C);

// Todo : move to proper location
void user_interface_register_screen_to_channel(c_screen_widget* new_screen, s_screen_parameters* parameters);
