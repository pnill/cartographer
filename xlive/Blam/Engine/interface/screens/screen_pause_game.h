#pragma once
#include "interface/user_interface.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"


/* enums */

enum e_pause_game_list_item_type
{
	_pause_game_list_item_continue = 0,
	_pause_game_list_item_revert,
	_pause_game_list_item_reset,
	_pause_game_list_item_settings,
	_pause_game_list_item_save_and_quit,
	k_pause_game_list_item_count,
};

/* classes */

class c_pause_game_list_item : public c_list_item_widget
{
public:
	c_pause_game_list_item(void) = default;
	
	virtual ~c_pause_game_list_item(void) = default;
};

class c_pause_game_list : public c_list_widget
{
public:
	c_pause_game_list(int16 user_flags);

	virtual ~c_pause_game_list(void) = default;
	virtual c_list_item_widget* get_list_items(void) override;
	virtual int32 get_list_items_count(void) override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

	void handle_item_pressed_event(struct s_event_record* const& event, datum* item_index);

private:
	c_pause_game_list_item m_list_items[k_pause_game_list_item_count];
	c_slot2<c_pause_game_list, s_event_record*, datum> m_slot;
};

class c_screen_pause_game : public c_screen_with_menu
{
public:
	c_screen_pause_game(const c_screen_parameters* parameters);
	c_screen_pause_game(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);
	
	// updates the objective text displayed on the right of the screen
	void update_objective_text(void);

	void revert_to_last_save(void);
	
	void continue_game(struct s_event_record* const& event);
	void restart_level(struct s_event_record* const& event);
	void handle_settings(struct s_event_record* const& event);
	void save_and_quit(struct s_event_record* const& event);

	static void* load(class c_screen_parameters* parameters);

	virtual ~c_screen_pause_game(void) = default;
	virtual void update(void) override;
	virtual const void* load_proc(void) const override;
	virtual void initialize(class c_screen_parameters const* parameters) override;
	virtual bool handle_event(s_event_record* event) override;

private:
	c_pause_game_list m_pause_game_list;
	c_maximum_interface_text m_objectives_string;
};
ASSERT_STRUCT_SIZE(c_screen_pause_game, 4540);

/* prototypes */

void screen_pause_game_apply_patches(void);
