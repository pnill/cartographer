#include "stdafx.h"
#include "screen_virtual_keyboard.h"

#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "main/game_preferences.h"

/* macro defines */

#define k_virtual_keyboard_description_text_block_index 3

/* structures */

struct s_keyboard_custom_labels
{
	const wchar_t* header_text;
	const wchar_t* subheader_text;
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

/* global variables */

bool g_vkbd_create_open_profile_config = true;

/* prototypes */

static void get_keyboard_labels(e_vkbd_context_type context, const wchar_t** out_header_text, const wchar_t** out_subheader_text);
static void apply_rename_squad_entry_patches();


/* private code */

// header & subheader
static void get_keyboard_labels(e_vkbd_context_type context, const wchar_t** out_header_text, const wchar_t** out_subheader_text)
{
	ASSERT(context >= k_virtual_keyboard_custom_context_start && k_virtual_keyboard_custom_context_start < k_virtual_keyboard_custom_context_end);

	// ### TODO FIXME localization
	*out_header_text = k_keyboard_custom_label_globals[_language_english][context - k_virtual_keyboard_custom_context_start].header_text;
	*out_subheader_text = k_keyboard_custom_label_globals[_language_english][context - k_virtual_keyboard_custom_context_start].subheader_text;
	return;
}

static void apply_rename_squad_entry_patches()
{
    static uint8 accentsPatchCode[] =
    {
        0x83,0xF9,_vkbd_context_message_entry,
        0x0F,0x8C,0x03,0xFF,0xFF,0xFF,
        0x83,0xF9,_vkbd_context_search_description,
        0x0F,0x8F,0xFA,0xFE,0xFF,0xFF,
        0x90
    };

    static uint8 symbolsPatchCode[] =
    {
        0x83,0xF9,_vkbd_context_message_entry,
        0x0F,0x8C,0xE4,0xFE,0xFF,0xFF,
        0x83,0xF9,_vkbd_context_search_description,
        0x0F,0x8F,0xDB,0xFE,0xFF,0xFF,
        0x90
    };

    /*
    
          if ( vkbd_context == _vkbd_context_type_message_entry
          || vkbd_context == _vkbd_context_type_message_entry2
          || vkbd_context == _vkbd_context_type_search_description )

          with

          if ( vkbd_context >= _vkbd_context_type_message_entry 
          && vkbd_context <= _vkbd_context_type_search_description )

    */

    WriteBytes(Memory::GetAddress(0x23CED1), accentsPatchCode, NUMBEROF(accentsPatchCode));// case _vkbd_button_type_accents_0:
    WriteBytes(Memory::GetAddress(0x23CEF0), symbolsPatchCode, NUMBEROF(symbolsPatchCode));// case _vkbd_button_type_symbols_0:

}


/* public code */

c_virtual_keyboard_button::c_virtual_keyboard_button():
	c_button_widget(NONE,0)
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

	csmemset(m_buffer, L'\0', sizeof(m_buffer));
	m_textbox_output = 0;
	m_textbox_message = 0;
	m_buffer_length = 0;
	m_row = k_number_of_virtual_keyboard_rows;
	m_column = k_number_of_virtual_keyboard_columns;
	m_vkbd_flags = 0;
	field_C74 = false;

	field_3B78 = NONE;
	field_3B7C = true;
	field_3B7D = false;

	for (int16 i = 0; i < NUMBEROF(m_buttons); i++)
	{
		m_buttons[i].set_button_index(i);
		m_buttons[i].set_controller_mask(_user_flags);
	}

	ui_set_virtual_keyboard_in_use(true);
}

c_screen_virtual_keyboard::~c_screen_virtual_keyboard()
{
	g_vkbd_create_open_profile_config = true;
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

void c_screen_virtual_keyboard::initialize(
	c_screen_parameters const* parameters)
{
	e_vkbd_context_type old_context = m_context;
	if (IN_RANGE(this->m_context, k_virtual_keyboard_custom_context_start, k_virtual_keyboard_custom_context_end))
	{
		// do not exceed this before calling the initializer
		switch (old_context)
		{
		case _vkbd_custom_context_username:
			m_context = _vkbd_context_player_profile_name;
			g_vkbd_create_open_profile_config = false;
			break;
		case _vkbd_custom_context_email_or_username:
		case _vkbd_custom_context_email:
		case _vkbd_custom_context_password:
			m_context = _vkbd_context_search_description;
			break;
		}
	}

	// call the orignal screen initializer
	INVOKE_TYPE(0x23BF3E, 0x0, int(__thiscall*)(c_screen_virtual_keyboard*, c_screen_parameters const*), this, parameters);

	update_custom_labels(old_context);
}

const void* c_screen_virtual_keyboard::load_proc(void) const
{
	return &c_screen_virtual_keyboard::load;
}

void c_screen_virtual_keyboard::update_custom_labels(e_vkbd_context_type context)
{
	if (IN_RANGE(context, k_virtual_keyboard_custom_context_start, k_virtual_keyboard_custom_context_end))
	{
		const wchar_t* header_text = L"<unknown-title>";
		const wchar_t* subheader_text = L"<unknown-description>";

		get_keyboard_labels(context, &header_text, &subheader_text);

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = try_find_text_widget(k_virtual_keyboard_description_text_block_index);
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

void* c_screen_virtual_keyboard::load(c_screen_parameters* parameters)
{
	c_screen_virtual_keyboard* virtual_keyboard_menu = nullptr;
	BYTE* ui_buffer = ui_pool_allocate_space(sizeof(c_screen_virtual_keyboard), 0);
	if (ui_buffer) {
		virtual_keyboard_menu = new (ui_buffer) c_screen_virtual_keyboard(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags()
		);
		virtual_keyboard_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(virtual_keyboard_menu, parameters);
	return virtual_keyboard_menu;
}

void* ui_load_virtual_keyboard(wchar_t* out_keyboard_text, uint32 out_keyboard_text_length, e_vkbd_context_type keyboard_type)
{
	c_screen_virtual_keyboard* virtual_keyboard;
	c_screen_parameters virtual_keyboard_params;

	virtual_keyboard_params.initialize_default_user(FLAG(0), _user_interface_channel_type_virtual_keyboard, _window_4, c_screen_virtual_keyboard::load);
	
	virtual_keyboard = (c_screen_virtual_keyboard*)virtual_keyboard_params.execute_load_function();

	virtual_keyboard->set_context(keyboard_type);
	virtual_keyboard->set_input_string_buffer(out_keyboard_text, out_keyboard_text_length);

	return virtual_keyboard;
}

typedef bool(__thiscall* t_load_player_profile_edit)(c_screen_virtual_keyboard*);
t_load_player_profile_edit p_load_player_profile_edit;

CLASS_HOOK_DECLARE_LABEL(c_screen_virtual_keyboard__load_player_profile_edit, c_screen_virtual_keyboard::load_player_profile_edit);
bool __thiscall c_screen_virtual_keyboard::load_player_profile_edit()
{
	bool result = true;
	if (g_vkbd_create_open_profile_config)
	{
		result = p_load_player_profile_edit(this);
	}

	return result;
}

void __declspec(naked) jmp_c_screen_virtual_keyboard__load_player_profile_edit()
{ 
	CLASS_HOOK_JMP(c_screen_virtual_keyboard__load_player_profile_edit, c_screen_virtual_keyboard::load_player_profile_edit);
}

void c_screen_virtual_keyboard::apply_patches()
{
	DETOUR_ATTACH(p_load_player_profile_edit, Memory::GetAddress<t_load_player_profile_edit>(0x23C7B2), jmp_c_screen_virtual_keyboard__load_player_profile_edit);

    apply_rename_squad_entry_patches();
}

void ui_set_virtual_keyboard_in_use(bool state)
{
	*Memory::GetAddress<bool*>(0x971290) = state;
}
