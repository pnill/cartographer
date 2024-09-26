#pragma once
#include "interface/user_interface_widget_window.h"
#include "interface/user_interface_widget_button.h"
#include "Blam/Engine/interface/user_interface_controller.h"


class c_screen_press_start_introduction : protected c_screen_widget
{
protected:
	c_button_widget m_start_button;
	int32 m_creation_time;
	c_slot2<c_screen_press_start_introduction, s_event_record**, datum> m_slot;
	bool m_has_input_saved;
	char gap[3];
	s_event_record m_saved_input;


	void handle_item_pressed_event(s_event_record** pevent, datum* pitem_index);

public:
	static void* load(s_screen_parameters* parameters);
	static void apply_instance_patches();
	c_screen_press_start_introduction(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);

	// c_screen_press_start_introduction virtual functions

	virtual ~c_screen_press_start_introduction() = default;
	virtual void update() override;
	bool handle_event(s_event_record* event) override;
	virtual void initialize(s_screen_parameters* parameters) override;
	virtual void sub_60EBC2(int32 a1) override;
	virtual void* load_proc() override;
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(c_screen_press_start_introduction, 0xB8C);