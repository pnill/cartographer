#pragma once
#include "interface/user_interface_widget_window.h"

/* forward defines */
class c_screen_xbox_live_task_progress_dialog;

typedef void (__cdecl* proc_task_cb_t)(c_screen_xbox_live_task_progress_dialog*);

/* classes */

class c_screen_xbox_live_task_progress_dialog : c_screen_widget
{
protected:
	proc_task_cb_t m_update_function;
	proc_task_cb_t m_close_function;
	uint8* m_data;
	datum m_online_task_datum;
	int32 m_online_task_result;

	void set_task_datum(datum task_datum);
	void set_update_function(proc_task_cb_t function);
	void set_close_function(proc_task_cb_t function);

	void close_task_internal();

public:
	c_screen_xbox_live_task_progress_dialog(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);
	uint8* get_data();
	void set_header_text(string_id header);
	void set_display_text(string_id display);
	void set_display_text_raw(wchar_t* raw_text);
	void close_task();

	static void add_task(proc_task_cb_t callback);
	static void add_task_ex(datum task_datum, e_controller_index controller_index, proc_task_cb_t update_function, proc_task_cb_t close_function, uint8* data);
	static void apply_patches_on_map_load();
	static void* load(s_screen_parameters* parameters);


	// c_screen_xbox_live_task_progress_dialog virtual functions

	virtual ~c_screen_xbox_live_task_progress_dialog();
	virtual void update() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void initialize(s_screen_parameters* parameters) override;
	virtual void* load_proc() override;
};
ASSERT_STRUCT_SIZE(c_screen_xbox_live_task_progress_dialog, 0xA70);