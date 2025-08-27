#include "stdafx.h"
#include "user_interface_text.h"

#include "interface/hud.h"
#include "user_interface.h"
#include "user_interface_shared_globals.h"
#include "Blam/Engine/interface/hud.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_session.h"
#include "tag_files/global_string_ids.h"
#include "text/text_group.h"
#include "text/unicode.h"

c_user_interface_text::c_user_interface_text()
{
	this->m_custom_font_type = 0;
	this->m_text_is_pulsating = 0;
	this->field_16 = 1;
	this->field_18 = NONE;
	this->m_text_justification = 2;
	this->field_20 = 1.0;
	this->field_24 = NONE;
	this->field_38 = 0;
	this->field_3C = NONE;
	this->text_length = 0;
	this->m_ui_start_time = 0;
	this->m_text_color.blue = 1.0;
	this->m_text_color.green = 1.0;
	this->m_text_color.red = 1.0;
}

const real_rgb_color* c_user_interface_text::get_color(void) const
{
	return &this->m_text_color;
}

int32 c_user_interface_text::get_font(void) const
{
	return this->m_custom_font_type;
}

void c_user_interface_text::set_font(int32 font_type)
{
	this->m_custom_font_type = font_type;
}

void c_user_interface_text::set_pulsating(bool pulsating)
{
	this->m_text_is_pulsating = pulsating;
}

void c_user_interface_text::set_color(real_rgb_color* color)
{
	this->m_text_color = *color;
}

void c_user_interface_text::set_color(const real_rgb_color* color)
{
	this->m_text_color = *color;
}

bool c_user_interface_text::is_private_use_character(wchar_t character)
{
	if (IN_RANGE(character, k_private_use_character_space_start, k_private_use_character_space_end))
		return true;
	return false;
}

// Seperated scale for the text labels (carto addition)
float ui_text_label_scaling = 0.0f;

float get_ui_text_label_scale()
{
	return ui_text_label_scaling;
}

void set_ui_text_label_scale(float scale)
{
	ui_text_label_scaling = scale;
}

bool __cdecl user_interface_parse_string(wchar_t* string, size_t max_length, char a3)
{
	return INVOKE(0x22F712, 0x0, user_interface_parse_string, string, max_length, a3);
}

static c_maximum_interface_text g_user_interface_parse_private_use_charater_game_type_buffer;

void __cdecl user_interface_parse_private_use_character_game_type(int32 a1, wchar_t* buffer)
{
	s_user_interface_shared_globals* user_interface_shared_globals = user_interface_shared_globals_get();

	ASSERT(user_interface_shared_globals);
	s_game_variant* variant = user_interface_get_variant();

	if (!variant)
	{
		c_network_session* network_session;
		if (network_life_cycle_in_squad_session(&network_session))
			variant = &network_session->m_session_parameters.game_variant;
	}

	if (variant)
	{
		string_id variant_id = _string_id_campaign;

		switch (variant->variant_game_engine_index)
		{
			case _game_engine_type_ctf:
				variant_id = _string_id_capture_the_flag;
				break;
			case _game_engine_type_slayer:
				variant_id = _string_id_slayer;
				break;
			case _game_engine_type_oddball:
				variant_id = _string_id_oddball;
				break;
			case _game_engine_type_koth:
				variant_id = _string_id_king_of_the_hill;
				break;
			case _game_engine_type_race:
				variant_id = _string_id_race;
				break;
			case _game_engine_type_headhunter:
				variant_id = _string_id_headhunter;
				break;
			case _game_engine_type_juggernaut:
				variant_id = _string_id_juggernaut;
				break;
			case _game_engine_type_territories:
				variant_id = _string_id_territories;
				break;
			case _game_engine_type_assault:
				variant_id = _string_id_assault;
				break;
			default:
				break;
		}
		g_user_interface_parse_private_use_charater_game_type_buffer.clear();
		string_list_get_normal_string(user_interface_shared_globals->gametype_strings.index, variant_id, &g_user_interface_parse_private_use_charater_game_type_buffer);
	}
	ustrncpy(buffer, g_user_interface_parse_private_use_charater_game_type_buffer.get_buffer(), NUMBEROF(g_user_interface_parse_private_use_charater_game_type_buffer.get_buffer()));
}


// this seems to compute the required space to display the text?
void __cdecl ui_get_text_bounds_and_position_hook(int a1, wchar_t* string, int a3, int a4, float scale)
{
	typedef void(__cdecl* ui_get_text_size_t)(int, wchar_t*, int, int, float);
	auto p_ui_get_text_size = Memory::GetAddress<ui_get_text_size_t>(0x99D97);

	p_ui_get_text_size(a1, string, a3, a4, *get_secondary_hud_scale());
}

void user_interface_text_apply_hooks()
{
	// Replace the ui_scale_factor with our own scaling for text labels
	WritePointer(Memory::GetAddress(0x2305AC) + 4, &ui_text_label_scaling);
	WritePointer(Memory::GetAddress(0x23066A) + 4, &ui_text_label_scaling);

	PatchCall(Memory::GetAddress(0x22CFFD), ui_get_text_bounds_and_position_hook);

	WritePointer(Memory::GetAddress(0x464350), &user_interface_parse_private_use_character_game_type);
}
