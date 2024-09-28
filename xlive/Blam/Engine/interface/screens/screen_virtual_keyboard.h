#pragma once
#include "interface/user_interface_headers.h"
#include "saved_games/saved_game_files.h"

/* macro defines */

#define k_number_of_virtual_keyboard_rows 4
#define k_number_of_virtual_keyboard_columns 10

/* enums */

enum e_vkbd_button_type : int8
{
	_vkbd_button_numeral_one = 0x0,
	_vkbd_button_numeral_two = 0x1,
	_vkbd_button_numeral_three = 0x2,
	_vkbd_button_numeral_four = 0x3,
	_vkbd_button_numeral_five = 0x4,
	_vkbd_button_numeral_six = 0x5,
	_vkbd_button_numeral_seven = 0x6,
	_vkbd_button_numeral_eight = 0x7,
	_vkbd_button_numeral_nine = 0x8,
	_vkbd_button_numeral_zero = 0x9,
	_vkbd_button_letter_a = 0xA,
	_vkbd_button_letter_b = 0xB,
	_vkbd_button_letter_c = 0xC,
	_vkbd_button_letter_d = 0xD,
	_vkbd_button_letter_e = 0xE,
	_vkbd_button_letter_f = 0xF,
	_vkbd_button_letter_g = 0x10,
	_vkbd_button_letter_h = 0x11,
	_vkbd_button_letter_i = 0x12,
	_vkbd_button_letter_j = 0x13,
	_vkbd_button_letter_k = 0x14,
	_vkbd_button_letter_l = 0x15,
	_vkbd_button_letter_m = 0x16,
	_vkbd_button_letter_n = 0x17,
	_vkbd_button_letter_o = 0x18,
	_vkbd_button_letter_p = 0x19,
	_vkbd_button_letter_q = 0x1A,
	_vkbd_button_letter_r = 0x1B,
	_vkbd_button_letter_s = 0x1C,
	_vkbd_button_letter_t = 0x1D,
	_vkbd_button_letter_u = 0x1E,
	_vkbd_button_letter_v = 0x1F,
	_vkbd_button_letter_w = 0x20,
	_vkbd_button_letter_x = 0x21,
	_vkbd_button_letter_y = 0x22,
	_vkbd_button_letter_z = 0x23,
	_vkbd_button_backspace = 0x24,
	_vkbd_button_space = 0x25,
	_vkbd_button_left_arrow = 0x26,
	_vkbd_button_right_arrow = 0x27,
	_vkbd_button_done = 0x28,
	_vkbd_button_shift = 0x29,
	_vkbd_button_caps_lock = 0x2A,
	_vkbd_button_accents_0 = 0x2B,
	_vkbd_button_symbols_0 = 0x2C,
	_vkbd_button_accents_1 = 0x2D,
	_vkbd_button_symbols_1 = 0x2E,

	k_number_of_virtual_keyboard_button_types = 0x2F,
};

enum e_vkbd_context_type : int32
{
	_vkbd_context_invalid = NONE,
	_vkbd_context_player_profile_name = 0x0,
	_vkbd_context_player_profile_name1 = 0x1,
	_vkbd_context_player_profile_name2 = 0x2,
	_vkbd_context_player_profile_name_for_solo = 0x3,
	_vkbd_context_player_profile_name4 = 0x4,
	_vkbd_context_variant_name_entry = 0x5,
	_vkbd_context_variant_name_entry2 = 0x6,
	_vkbd_context_variant_name_rename = 0x7,
	_vkbd_context_playlist_name = 0x8,
	_vkbd_context_playlist_name2 = 0x9,
	_vkbd_context_saved_film_name = 0xA,
	_vkbd_context_saved_film_name2 = 0xB,
	_vkbd_context_message_entry = 0xC,
	_vkbd_context_squad_name_entry = 0xD,
	_vkbd_context_type_14 = 0xE,
	_vkbd_context_message_entry2 = 0xF,
	_vkbd_context_search_description = 0x10,

	k_virtual_keyboard_custom_context_start,

	_vkbd_custom_context_email = k_virtual_keyboard_custom_context_start,
	_vkbd_custom_context_username,
	_vkbd_custom_context_email_or_username,
	_vkbd_custom_context_password,

	k_virtual_keyboard_custom_context_end,
	k_number_of_custom_vkbd_context_types = k_virtual_keyboard_custom_context_end- k_virtual_keyboard_custom_context_start,

	k_number_of_virtual_keyboard_context_types= 0x11,
};

/* classes */

class c_virtual_keyboard_button : public c_button_widget
{
public:
	c_virtual_keyboard_button();

	// c_virtual_keyboard_button virtual functions

	virtual ~c_virtual_keyboard_button() = default;	
	virtual bool handle_event(s_event_record* event) override;
	virtual int32 get_bitmap_active_index() override;
};
ASSERT_STRUCT_SIZE(c_virtual_keyboard_button, 0x100);


class c_screen_virtual_keyboard : protected c_screen_widget
{
protected:
	e_vkbd_context_type m_context;
	e_saved_game_file_type m_saved_game_file_type;
	c_static_wchar_string256 m_buffer;
	wchar_t* m_textbox_output;
	wchar_t* m_textbox_message;
	uint16 m_buffer_length;
	uint16 m_row;
	uint16 m_column;
	uint16 m_vkbd_flags;
	bool field_C74;
	uint8 field_C75[3];
	c_virtual_keyboard_button m_buttons[k_number_of_virtual_keyboard_button_types];
	uint32 field_3B78;
	bool field_3B7C;
	bool field_3B7D;
	bool field_3B7E;
	bool field_3B7F;

	void update_custom_labels(e_vkbd_context_type context);

public:
	c_screen_virtual_keyboard(e_user_interface_channel_type _channel_type, e_user_interface_render_window _window_index, uint16 _user_flags);

	void set_input_string_buffer(wchar_t* buffer, uint32 buffer_size);
	void set_context(e_vkbd_context_type context);

	static void apply_patches();
	bool __thiscall load_player_profile_edit();

	static void* load(s_screen_parameters* parameters);

	// c_screen_virtual_keyboard virtual functions

	virtual ~c_screen_virtual_keyboard();
	virtual void update() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void initialize(s_screen_parameters* parameters) override;
	virtual void* load_proc() override;
};
ASSERT_STRUCT_SIZE(c_screen_virtual_keyboard, 0x3B80);

/* public code */

void* ui_load_virtual_keyboard(wchar_t* out_keyboard_text, uint32 out_keyboard_text_lenght, e_vkbd_context_type keyboard_type);
void ui_set_virtual_keyboard_in_use(bool state);