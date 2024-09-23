#include "stdafx.h"
#include "screen_virtual_keyboard.h"
#include "main/game_preferences.h"

/* macro defines */

#define K_VIRTUAL_KEYBOARD_DESCRIPTION_TEXT_BLOCK_INDEX 3

/* structures */

struct s_keyboard_custom_labels
{
	wchar_t* header_text;
	wchar_t* subheader_text;
};

/* global constants */

const s_keyboard_custom_labels k_keyboard_custom_label_globals[k_language_count][k_number_of_custom_vkbd_context_types] =
{
	{
		{
			L"ENTER EMAIL",
			L"Please enter valid Email address for your account",
		},
		{
			L"ENTER USERNAME",
			L"Use the virtual keyboard to enter a name for this profile. User names may be up to 16 characters.",
		},
		{
			L"ENTER USERNAME or EMAIL",
			L"Please enter valid Username or Email address for your account",
		},
		{
			 L"ENTER PASSWORD",
			 L"Please enter password associated with your account.",
		},
	}
};

// header & subheader
static void get_keyboard_labels(e_vkbd_context_type context, wchar_t** out_header_text, wchar_t** out_subheader_text)
{
	ASSERT(context >= k_virtual_keyboard_custom_context_start);

	// ### TODO FIXME localization
	*out_header_text = k_keyboard_custom_label_globals[_language_english][context - k_virtual_keyboard_custom_context_start].header_text;
	*out_subheader_text = k_keyboard_custom_label_globals[_language_english][context - k_virtual_keyboard_custom_context_start].subheader_text;
}

c_virtual_keyboard_button::c_virtual_keyboard_button():
	c_button_widget(NONE,0)
{
}

c_virtual_keyboard_button::~c_virtual_keyboard_button()
{
}

bool c_virtual_keyboard_button::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x23CF88, 0x0, bool(__thiscall*)(c_virtual_keyboard_button*, s_event_record*), this, event);
}

int32 c_virtual_keyboard_button::get_bitmap_active_index()
{
	return INVOKE_TYPE(0x23ADB1, 0x0, int32(__thiscall*)(c_button_widget*), this);
}


c_screen_virtual_keyboard::c_screen_virtual_keyboard(e_user_interface_channel_type _channel_type, e_user_interface_render_window _window_index, uint16 _user_flags) :
	c_screen_widget(_screen_virtual_keyboard, _channel_type, _window_index, _user_flags)
{
	m_context = _vkbd_context_invalid;
	m_saved_game_file_type = _saved_game_file_type_invalid;
	m_buffer.clear();
	m_textbox_output = 0;
	m_textbox_message = 0;
	m_buffer_length = 0;
	m_row = k_number_of_virtual_keyboard_rows;
	m_column = k_number_of_virtual_keyboard_columns;
	m_vkbd_flags = 0;
	field_C74 = 0;

	field_3B78 = NONE;
	field_3B7C = 1;
	field_3B7D = 0;

	for (int16 i = 0; i < NUMBEROF(m_buttons); i++)
	{
		m_buttons[i].m_button_index = i;
		m_buttons[i].m_controllers_mask = _user_flags;
	}

	ui_set_virtual_keyboard_in_use(true);
}

c_screen_virtual_keyboard::~c_screen_virtual_keyboard()
{
	ui_set_virtual_keyboard_in_use(false);
}

void c_screen_virtual_keyboard::update()
{
	c_user_interface_widget::update();
}

void c_screen_virtual_keyboard::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x23C042, 0x0, void(__thiscall*)(c_screen_virtual_keyboard*, rectangle2d*), this, viewport_bounds);
}

bool c_screen_virtual_keyboard::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x23D060, 0x0, bool(__thiscall*)(c_screen_virtual_keyboard*, s_event_record*), this, event);
}

void c_screen_virtual_keyboard::initialize(s_screen_parameters* parameters)
{
	e_vkbd_context_type old_context = m_context;
	if (IN_RANGE(this->m_context, k_virtual_keyboard_custom_context_start, k_virtual_keyboard_custom_context_end))
	{
		// do not exceed this before calling the initializer
		m_context = _vkbd_context_search_description;
	}

	// call the orignal screen initializer
	INVOKE_TYPE(0x23BF3E, 0x0, int(__thiscall*)(c_screen_virtual_keyboard*, s_screen_parameters*), this, parameters);

	update_custom_labels(old_context);
}

void* c_screen_virtual_keyboard::load_proc()
{
	return &c_screen_virtual_keyboard::load;
}

void c_screen_virtual_keyboard::update_custom_labels(e_vkbd_context_type context)
{
	if (IN_RANGE(context, k_virtual_keyboard_custom_context_start, k_virtual_keyboard_custom_context_end))
	{
		wchar_t* header_text = L"<unknown-title>";
		wchar_t* subheader_text = L"<unknown-description>";

		get_keyboard_labels(context, &header_text, &subheader_text);

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = try_find_text_widget(K_VIRTUAL_KEYBOARD_DESCRIPTION_TEXT_BLOCK_INDEX);
		if (subheader_text_widget)
		{
			subheader_text_widget->set_text(subheader_text);
		}
	}
}


void c_screen_virtual_keyboard::set_input_string_buffer(wchar_t* buffer, uint32 buffer_size)
{
	INVOKE_TYPE(0x23B118, 0x0, void(__thiscall*)(c_screen_virtual_keyboard*, wchar_t*, int), this, buffer, buffer_size);
}

void c_screen_virtual_keyboard::set_context(e_vkbd_context_type context)
{
	m_context = context;
}

void* c_screen_virtual_keyboard::load(s_screen_parameters* parameters)
{
	c_screen_virtual_keyboard* virtual_keyboard_menu = nullptr;
	BYTE* ui_buffer = ui_pool_allocate_space(sizeof(c_screen_virtual_keyboard), 0);
	if (ui_buffer) {
		virtual_keyboard_menu = new (ui_buffer) c_screen_virtual_keyboard(parameters->m_channel_type, parameters->m_window_index, parameters->user_flags);
		virtual_keyboard_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(virtual_keyboard_menu, parameters);
	return virtual_keyboard_menu;
}

void* ui_load_virtual_keyboard(wchar_t* out_keyboard_text, uint32 out_keyboard_text_lenght, e_vkbd_context_type keyboard_type)
{
	s_screen_parameters virtual_keyboard_params;
	virtual_keyboard_params.m_context = nullptr;
	virtual_keyboard_params.data_new(0, FLAG(0), _user_interface_channel_type_keyboard, _window_4, c_screen_virtual_keyboard::load);
	c_screen_virtual_keyboard* virtual_keyboard = (c_screen_virtual_keyboard*)virtual_keyboard_params.ui_screen_load_proc_exec();

	virtual_keyboard->set_context(keyboard_type);
	virtual_keyboard->set_input_string_buffer(out_keyboard_text, out_keyboard_text_lenght);

	return virtual_keyboard;
}

void ui_set_virtual_keyboard_in_use(bool state)
{
	*Memory::GetAddress<bool*>(0x971290) = state;
}
