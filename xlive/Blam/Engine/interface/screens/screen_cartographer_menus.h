#pragma once

// miscellaneous cartographer menus

#include "interface/user_interface_headers.h"

#include "main/game_preferences.h"
#include "H2MOD/Modules/Updater/Updater.h"

/* macro defines */

#define k_no_of_visible_items_for_cartographer_guide_list 4
#define k_no_of_visible_items_for_cartographer_credits_list 4
#define k_no_of_visible_items_for_cartographer_update_list 2
#define k_no_of_visible_items_for_cartographer_update_notice_list 2

/* enums */

enum e_cartographer_update_status
{
	_cartographer_update_status_none = NONE,
	_cartographer_update_status_pending_check_for_update,
	_cartographer_update_status_checking_for_update,
	_cartographer_update_status_pending_download_update,
	_cartographer_update_status_downloading_update,
	_cartographer_update_status_pending_install_update,
	_cartographer_update_status_installing_update,

	k_cartographer_update_status_end,
};

/* classes */

class c_cartographer_guide_edit_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_item_widgets[k_no_of_visible_items_for_cartographer_guide_list];
	c_slot2<c_cartographer_guide_edit_list, s_event_record*, int32> m_slot_2;

	void handle_item_pressed_event(s_event_record* pevent, int32* pitem_index);
public:
	c_cartographer_guide_edit_list(uint16 _flags);

	// c_cartographer_guide_edit_list virtual functions

	virtual ~c_cartographer_guide_edit_list();
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;
};

class c_cartographer_guide_menu : public c_screen_with_menu
{
protected:
	c_cartographer_guide_edit_list m_guide_edit_list;
public:
	c_cartographer_guide_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);
	static void* load(s_screen_parameters* parameters);
	
	// c_cartographer_guide_menu virtual functions

	virtual ~c_cartographer_guide_menu();
	virtual void initialize(s_screen_parameters* screen_parameters) override;
	virtual void* load_proc() override;
};


// credits

class c_cartographer_credits_edit_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_item_widgets[k_no_of_visible_items_for_cartographer_credits_list];
	c_slot2<c_cartographer_credits_edit_list, s_event_record*, int32> m_slot_2;

	void handle_item_pressed_event(s_event_record* pevent, int32* pitem_index);

public:
	c_cartographer_credits_edit_list(uint16 _flags);

	// c_cartographer_credits_edit_list virtual functions

	virtual ~c_cartographer_credits_edit_list();
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;
};

class c_cartographer_credits_menu : public c_screen_with_menu
{
	c_cartographer_credits_edit_list m_credits_edit_list;
public:
	c_cartographer_credits_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);
	static void* load(s_screen_parameters* parameters);

	// c_cartographer_credits_menu virtual functions

	virtual ~c_cartographer_credits_menu();
	virtual void* load_proc() override;
	virtual void initialize(s_screen_parameters* screen_parameters) override;
};


// update

class c_cartographer_update_edit_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_item_widgets[k_no_of_visible_items_for_cartographer_update_list];
	c_slot2<c_cartographer_update_edit_list, s_event_record*, int32> m_slot_2;
	int32 m_update_status;
	bool m_keep_screen_open;

	void handle_item_pressed_event(s_event_record* pevent, int32* pitem_index);

public:
	c_cartographer_update_edit_list(uint16 _flags);
	void set_status(e_cartographer_update_status status);
	void update_updater_status();

	// c_cartographer_update_edit_list virtual functions
	
	virtual ~c_cartographer_update_edit_list();
	virtual void pre_destroy() override;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;
};

class c_cartographer_update_menu : public c_screen_with_menu
{
protected:
	c_cartographer_update_edit_list m_update_edit_list;

public:
	c_cartographer_update_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);
	void set_update_status(e_cartographer_update_status status);
	static void* load(s_screen_parameters* parameters);

	// c_cartographer_update_menu virtual functions

	virtual ~c_cartographer_update_menu();
	virtual void update() override;
	virtual void* load_proc() override;
	virtual void initialize(s_screen_parameters* screen_parameters) override;
};

// update notice

class c_cartographer_update_notice_edit_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_item_widgets[k_no_of_visible_items_for_cartographer_update_notice_list];
	c_slot2<c_cartographer_update_notice_edit_list, s_event_record*, int32> m_slot_2;

	void handle_item_pressed_event(s_event_record* pevent, int32* pitem_index);

public:
	c_cartographer_update_notice_edit_list(uint16 _flags);


	// c_cartographer_update_notice_edit_list virtual functions.

	virtual ~c_cartographer_update_notice_edit_list();
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;
};

class c_cartographer_update_notice_menu : public c_screen_with_menu
{
protected:
	c_cartographer_update_notice_edit_list m_update_notice_edit_list;
public:
	c_cartographer_update_notice_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);
	static void* load(s_screen_parameters* parameters);

	// c_cartographer_update_notice_menu virtual functions.

	virtual ~c_cartographer_update_notice_menu();
	virtual void* load_proc() override;
	virtual void initialize(s_screen_parameters* screen_parameters) override;
};

//public code

void* ui_load_cartographer_guide_menu();
void* ui_load_cartographer_update_notice_menu();
void* ui_load_cartographer_credits_menu();
void* ui_load_cartographer_update_menu();
