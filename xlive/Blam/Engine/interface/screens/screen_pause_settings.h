#pragma once
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* enums */

enum c_pause_settings_list_item_type
{
	_pause_settings_list_item_controller = 0,
	_pause_settings_list_item_keyboard,
	_pause_settings_list_item_video,
	_pause_settings_list_item_audio,
	k_pause_settings_list_item_count,

	_pause_settings_list_unknown = k_pause_settings_list_item_count,
	k_pause_settings_list_item_total_count,
};

/* classes */

class c_pause_settings_list : public c_list_widget
{
public:
	c_pause_settings_list(int16 user_flags);


	virtual ~c_pause_settings_list(void) = default;
	virtual c_list_item_widget* get_list_items(void) override;
	virtual int32 get_list_items_count(void) override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

	void handle_item_pressed_event(struct s_event_record* const& event, datum* item_index);

private:
	c_list_item_widget m_list_items[k_pause_settings_list_item_total_count];
	c_slot2<c_pause_settings_list, s_event_record*, int32> m_slot;
};

class c_screen_pause_settings : public c_screen_with_menu
{
public:
	c_screen_pause_settings(class c_screen_parameters const* parameters);
	c_screen_pause_settings(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);

	virtual ~c_screen_pause_settings(void) = default;
	virtual bool handle_event(s_event_record* event) override;

	virtual const void* load_proc(
		void) const
	{
		return c_screen_pause_settings::load;
	}

	static void* load(
		class c_screen_parameters* parameters)
	{
		void* pool;
		
		c_screen_pause_settings* screen = (pool = ui_pool_allocate_space(sizeof(c_screen_pause_settings), 0)) ? new (pool) c_screen_pause_settings(parameters) : NULL;
		ASSERT(screen != NULL);

		screen->m_allocated = true;
		user_interface_register_screen_to_channel(screen, parameters);

		return screen;
	}


private:
	c_pause_settings_list m_pause_settings_list;
};
