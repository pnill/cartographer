#pragma once

// miscellaneous cartographer menus

#include "interface/user_interface_headers.h"

#include "H2MOD/Modules/Updater/Updater.h"

void* ui_load_cartographer_guide_menu();
void* ui_load_cartographer_update_notice_menu();
void* ui_load_cartographer_credits_menu();
void* ui_load_cartographer_update_menu();

enum e_cartographer_guide_string_table
{
	_cartographer_guide_text_none = NONE,
	_cartographer_guide_text_header,
	_cartographer_guide_text_subheader,

	k_cartographer_guide_button_label_index_start,
	_cartographer_guide_button_advanced_settings_text = k_cartographer_guide_button_label_index_start,
	_cartographer_guide_button_text_website,
	_cartographer_guide_button_text_credits,
	_cartographer_guide_button_text_update,

	k_cartographer_guide_end,
};

static void get_guide_label(int32 label_id, wchar_t** out_label)
{
	wchar_t* label_table[k_language_count][k_cartographer_guide_end] = {};

	label_table[_language_english][_cartographer_guide_text_header] = L"Project Cartographer Guide";
	label_table[_language_english][_cartographer_guide_text_subheader] = L"Press the %S Key to open this guide from anywhere.";
	label_table[_language_english][_cartographer_guide_button_advanced_settings_text] = L"Advanced Settings";
	label_table[_language_english][_cartographer_guide_button_text_website] = L"Website";
	label_table[_language_english][_cartographer_guide_button_text_credits] = L"Credits";
	label_table[_language_english][_cartographer_guide_button_text_update] = L"Update";

	*out_label = label_table[_language_english][label_id];
}

class c_cartographer_guide_edit_list : public c_list_widget
{
public:
	c_list_item_widget m_list_item_widgets[4];
	int field_2C0;
	c_slot2<c_cartographer_guide_edit_list, s_event_record*, int32> m_slot_2;

	c_cartographer_guide_edit_list(uint16 _flags);

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_guide_edit_list();

		return this;
	}

	virtual c_list_item_widget* get_list_items() override
	{
		return m_list_item_widgets;
	}

	virtual int32 get_list_items_count() override
	{
		return NUMBEROF(m_list_item_widgets);
	}

	void update_list_items(c_list_item_widget* item, int32 skin_index) override
	{
		datum list_item_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index());

		c_text_widget* text_widget = (c_text_widget*)item->try_find_child(_widget_type_text, 0, false);
		if (text_widget)
		{
			wchar_t* button_label = nullptr;
			get_guide_label(k_cartographer_guide_button_label_index_start + (int32)list_item_index, &button_label);

			ASSERT(button_label != nullptr);
			text_widget->set_text(button_label);
		}
	}

	void button_handler(s_event_record* event_record, int32* a3);
};

class c_cartographer_guide_menu : public c_screen_with_menu
{
	c_cartographer_guide_edit_list m_guide_edit_list;
public:
	c_cartographer_guide_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);

	static void* open(s_screen_parameters* parameters);

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_guide_menu();
		return this;
	}

	virtual void post_initialize() override
	{
		return c_screen_with_menu::post_initialize();
	}

	virtual void* load_proc() override
	{
		return c_cartographer_guide_menu::open;
	}

	virtual void initialize(s_screen_parameters* screen_parameters) override
	{
		c_screen_with_menu::initialize(screen_parameters);

		wchar_t* header_text = nullptr;
		wchar_t* subheader_text = nullptr;
		wchar_t text_buffer[512];

		get_guide_label(_cartographer_guide_text_header, &header_text);
		get_guide_label(_cartographer_guide_text_subheader, &subheader_text);

		ASSERT(header_text != nullptr && subheader_text != nullptr);

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = (c_text_widget*)try_find_text_widget(2);
		if (subheader_text_widget)
		{
			extern int H2Config_hotkeyIdGuide;
			extern std::string GetVKeyCodeString(int vkey);
			swprintf(text_buffer, ARRAYSIZE(text_buffer), subheader_text, GetVKeyCodeString(H2Config_hotkeyIdGuide).c_str());
			subheader_text = text_buffer;

			subheader_text_widget->set_text(subheader_text);
		}
	}
};

void* ui_load_cartographer_guide_menu();

// credits

enum e_cartographer_credits_string_table
{
	_cartographer_credits_text_none = NONE,
	_cartographer_credits_text_header,
	_cartographer_credits_text_subheader,

	k_cartographer_credits_button_text_index,
	_cartographer_credits_text_button_1 = k_cartographer_credits_button_text_index,
	_cartographer_credits_text_button_2,
	_cartographer_credits_text_button_3,
	_cartographer_credits_text_button_4,
	_cartographer_credits_text_button_5,
	_cartographer_credits_text_button_6,
	_cartographer_credits_text_button_7,
	_cartographer_credits_text_button_8,
	_cartographer_credits_text_button_9,
	_cartographer_credits_text_button_10,
	_cartographer_credits_text_button_11,
	_cartographer_credits_text_button_12,
	_cartographer_credits_text_button_13,
	_cartographer_credits_text_button_14,
	_cartographer_credits_text_button_15,
	_cartographer_credits_text_button_16,

	k_cartographer_credits_end,
};

static void get_credits_label(int32 label_id, wchar_t** out_label)
{
	wchar_t* label_table[k_language_count][k_cartographer_credits_end] = {};

	label_table[_language_english][_cartographer_credits_text_header] = L"Credits";
	label_table[_language_english][_cartographer_credits_text_subheader] = L"Praise the Following:";
	label_table[_language_english][_cartographer_credits_text_button_1] = L"--- The Devs ---";
	label_table[_language_english][_cartographer_credits_text_button_2] = L"pnill";
	label_table[_language_english][_cartographer_credits_text_button_3] = L"Glitchy Scripts";
	label_table[_language_english][_cartographer_credits_text_button_4] = L"Himanshu01";
	label_table[_language_english][_cartographer_credits_text_button_5] = L"nukeulater";
	label_table[_language_english][_cartographer_credits_text_button_6] = L"Berthalamew";
	label_table[_language_english][_cartographer_credits_text_button_7] = L"Rude Yoshi";
	label_table[_language_english][_cartographer_credits_text_button_8] = L"num005";
	label_table[_language_english][_cartographer_credits_text_button_9] = L"Supersniper";
	label_table[_language_english][_cartographer_credits_text_button_10] = L"--- Content Creators ---";
	label_table[_language_english][_cartographer_credits_text_button_11] = L"General_101";
	label_table[_language_english][_cartographer_credits_text_button_12] = L"--- Additional 7hanks ---";
	label_table[_language_english][_cartographer_credits_text_button_13] = L"Project Cartographer Staff";
	label_table[_language_english][_cartographer_credits_text_button_14] = L"Dev Preview Members";
	label_table[_language_english][_cartographer_credits_text_button_15] = L"And the many many more";
	label_table[_language_english][_cartographer_credits_text_button_16] = L"from the Halo 2 Community!";

	*out_label = label_table[_language_english][label_id];
}

class c_cartographer_credits_edit_list : public c_list_widget
{
public:
	c_list_item_widget m_list_item_widgets[4];
	int field_2C0;
	c_slot2<c_cartographer_credits_edit_list, s_event_record*, int32> m_slot_2;

	c_cartographer_credits_edit_list(uint16 _flags);

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_credits_edit_list();

		return this;
	}

	virtual c_list_item_widget* get_list_items() override
	{
		return m_list_item_widgets;
	}

	virtual int32 get_list_items_count() override
	{
		return NUMBEROF(m_list_item_widgets);
	}

	void update_list_items(c_list_item_widget* item, int32 skin_index) override
	{
		datum list_item_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index());

		c_text_widget* text_widget = (c_text_widget*)item->try_find_text_widget(0);
		if (text_widget)
		{
			wchar_t* button_label = nullptr;
			get_credits_label(k_cartographer_credits_button_text_index + (int32)list_item_index, &button_label);

			ASSERT(button_label != nullptr);
			text_widget->set_text(button_label);
		}
	}

	void button_handler(s_event_record* event_record, int32* a3);
};

class c_cartographer_credits_menu : public c_screen_with_menu
{
	c_cartographer_credits_edit_list m_credits_edit_list;
public:
	c_cartographer_credits_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);

	static void* open(s_screen_parameters* parameters);

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_credits_menu();

		return this;
	}

	virtual void post_initialize() override
	{
		return c_screen_with_menu::post_initialize();
	}

	virtual void* load_proc() override
	{
		return c_cartographer_credits_menu::open;
	}

	virtual void initialize(s_screen_parameters* screen_parameters) override
	{
		c_screen_with_menu::initialize(screen_parameters);

		wchar_t* header_text = nullptr;
		wchar_t* subheader_text = nullptr;

		get_credits_label(_cartographer_credits_text_header, &header_text);
		get_credits_label(_cartographer_credits_text_subheader, &subheader_text);

		ASSERT(header_text != nullptr && subheader_text != nullptr);

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = (c_text_widget*)try_find_text_widget(2);
		if (subheader_text_widget)
		{
			subheader_text_widget->set_text(subheader_text);
		}
	}
};


// update

enum e_cartographer_update_string_table
{
	_cartographer_update_text_none = NONE,
	_cartographer_update_text_header,
	_cartographer_update_text_subheader,

	k_cartographer_update_button_label_index_start,
	_cartographer_update_text_button_check_for_updates = k_cartographer_update_button_label_index_start,
	_cartographer_update_text_button_status_checking_for_updates,
	_cartographer_update_text_button_download_updates,
	_cartographer_update_text_button_status_downloading_update,
	_cartographer_update_text_button_install_update,
	_cartographer_update_text_button_status_installing_update,
	_cartographer_update_text_button_status_failed_to_run_updater_app,
	_cartographer_update_text_button_status_download_the_following,
	_cartographer_update_text_button_status_install_the_following,
	_cartographer_update_text_button_status_up_to_date,
	_cartographer_update_text_button_status_cancel,

	k_cartographer_update_text_end,
};

static void get_update_label(int32 label_id, wchar_t** out_label)
{
	wchar_t* label_table[k_language_count][k_cartographer_update_text_end] = {};

	label_table[_language_english][_cartographer_update_text_header] = L"Update!";
	label_table[_language_english][_cartographer_update_text_subheader] = L"Update Project Cartographer";
	label_table[_language_english][_cartographer_update_text_button_check_for_updates] = L"Check for Update";
	label_table[_language_english][_cartographer_update_text_button_status_checking_for_updates] = L"Checking For Update...";
	label_table[_language_english][_cartographer_update_text_button_download_updates] = L"Download Update";
	label_table[_language_english][_cartographer_update_text_button_status_downloading_update] = L"Downloading Update...";
	label_table[_language_english][_cartographer_update_text_button_install_update] = L"Install Update";
	label_table[_language_english][_cartographer_update_text_button_status_installing_update] = L"Installing Update...";
	label_table[_language_english][_cartographer_update_text_button_status_failed_to_run_updater_app] = L"Failed to run updater app!";
	label_table[_language_english][_cartographer_update_text_button_status_cancel] = L"Cancel";

	*out_label = label_table[_language_english][label_id];
}

void* ui_load_cartographer_update_menu();

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

class c_cartographer_update_edit_list : public c_list_widget
{
public:
	c_list_item_widget m_list_item_widgets[2];
	int field_2C0;
	c_slot2<c_cartographer_update_edit_list, s_event_record*, int32> m_slot_2;
	int32 m_update_status;
	bool m_keep_screen_open;

	c_cartographer_update_edit_list(uint16 _flags);
	~c_cartographer_update_edit_list()
	{
		if (m_keep_screen_open)
		{
			ui_load_cartographer_update_menu();
		}
	}

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_update_edit_list();

		return this;
	}

	virtual c_list_item_widget* get_list_items() override
	{
		return m_list_item_widgets;
	}

	virtual int32 get_list_items_count() override
	{
		return NUMBEROF(m_list_item_widgets);
	}

	void update_list_items(c_list_item_widget* item, int32 skin_index) override
	{
		datum list_item_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index());

		c_text_widget* text_widget = (c_text_widget*)item->try_find_text_widget(0);
		if (text_widget)
		{
			wchar_t* button_label = nullptr;

			switch (list_item_index)
			{
			case 0:
				get_update_label(k_cartographer_update_button_label_index_start + m_update_status, &button_label);
				text_widget->get_interface()->set_pulsating((m_update_status % 2) == 0);
				break;
			case 1:
				get_update_label(_cartographer_update_text_button_status_cancel, &button_label);
				break;
			default:
				break;
			}

			ASSERT(button_label != nullptr);
			text_widget->set_text(button_label);
		}
	}

	void update_updater_status()
	{
		if (m_update_status < _cartographer_update_status_pending_download_update && updater_has_files_to_download)
		{
			m_update_status = _cartographer_update_status_pending_download_update;
		}

		if (m_update_status < _cartographer_update_status_pending_install_update && updater_has_files_to_install)
		{
			m_update_status = _cartographer_update_status_pending_install_update;
		}
	}

	void button_handler(s_event_record* event_record, int32* a3);
};

class c_cartographer_update_menu : public c_screen_with_menu
{
	c_cartographer_update_edit_list m_update_edit_list;

public:
	c_cartographer_update_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_update_menu();

		return this;
	}

	static void* open(s_screen_parameters* parameters);

	virtual void post_initialize() override
	{
		return c_screen_with_menu::post_initialize();
	}

	virtual void update() override
	{
		m_update_edit_list.update_updater_status();
		c_screen_with_menu::update();
	}

	virtual void* load_proc() override
	{
		return c_cartographer_update_menu::open;
	}

	virtual void initialize(s_screen_parameters* screen_parameters) override
	{
		c_screen_with_menu::initialize(screen_parameters);

		wchar_t* header_text = nullptr;
		wchar_t* subheader_text = nullptr;

		get_update_label(_cartographer_update_text_header, &header_text);
		get_update_label(_cartographer_update_text_subheader, &subheader_text);

		ASSERT(header_text != nullptr && subheader_text != nullptr);

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = (c_text_widget*)try_find_text_widget(2);
		if (subheader_text_widget)
		{
			subheader_text_widget->set_text(subheader_text);
		}
	}

	void set_update_status(e_cartographer_update_status status)
	{
		m_update_edit_list.m_update_status = status;
	}
};

// update notice

enum e_cartographer_update_notice_string_table
{
	_cartographer_update_notice_text_none = NONE,
	_cartographer_update_notice_text_header,
	_cartographer_update_notice_text_subheader,

	k_cartographer_update_notice_button_label_index_start,
	_cartographer_update_text_button_notice_yes = k_cartographer_update_notice_button_label_index_start,
	_cartographer_update_text_button_notice_no,

	k_cartographer_update_notice_end,
};

static void get_update_notice_label(int32 label_id, wchar_t** out_label)
{
	wchar_t* label_table[k_language_count][k_cartographer_update_notice_end] = {};

	label_table[_language_english][_cartographer_update_notice_text_header] = L"Outdated Version!";
	label_table[_language_english][_cartographer_update_notice_text_subheader] = L"You are using an outdated version of Project Cartographer! Would you like to go install the latest version?";
	label_table[_language_english][_cartographer_update_text_button_notice_yes] = L"Yes";
	label_table[_language_english][_cartographer_update_text_button_notice_no] = L"No";

	*out_label = label_table[_language_english][label_id];
}

class c_cartographer_update_notice_edit_list : public c_list_widget
{
public:
	c_list_item_widget m_list_item_widgets[2];
	int field_2C0;
	c_slot2<c_cartographer_update_notice_edit_list, s_event_record*, int32> m_slot_2;

	c_cartographer_update_notice_edit_list(uint16 _flags);

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_update_notice_edit_list();

		return this;
	}

	virtual c_list_item_widget* get_list_items() override
	{
		return m_list_item_widgets;
	}

	virtual int32 get_list_items_count() override
	{
		return NUMBEROF(m_list_item_widgets);
	}

	void update_list_items(c_list_item_widget* item, int32 skin_index) override
	{
		datum list_item_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index());

		c_text_widget* text_widget = (c_text_widget*)item->try_find_text_widget(0);
		if (text_widget)
		{
			wchar_t* button_label = nullptr;
			get_update_notice_label(k_cartographer_update_notice_button_label_index_start + (int32)list_item_index, &button_label);

			ASSERT(button_label != nullptr);
			text_widget->set_text(button_label);
		}
	}

	void button_handler(s_event_record* event_record, int32* a3);
};

class c_cartographer_update_notice_menu : public c_screen_with_menu
{
	c_cartographer_update_notice_edit_list m_update_notice_edit_list;
public:
	c_cartographer_update_notice_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_update_notice_menu();

		return this;
	}

	static void* open(s_screen_parameters* parameters);

	virtual void post_initialize() override
	{
		return c_screen_with_menu::post_initialize();
	}

	virtual void* load_proc() override
	{
		return c_cartographer_update_notice_menu::open;
	}

	virtual void initialize(s_screen_parameters* screen_parameters) override
	{
		c_screen_with_menu::initialize(screen_parameters);

		wchar_t* header_text = nullptr;
		wchar_t* subheader_text = nullptr;

		get_update_notice_label(_cartographer_update_notice_text_header, &header_text);
		get_update_notice_label(_cartographer_update_notice_text_subheader, &subheader_text);

		ASSERT(header_text != nullptr && subheader_text != nullptr);

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = (c_text_widget*)try_find_text_widget(2);
		if (subheader_text_widget)
		{
			subheader_text_widget->set_text(subheader_text);
		}
	}
};

