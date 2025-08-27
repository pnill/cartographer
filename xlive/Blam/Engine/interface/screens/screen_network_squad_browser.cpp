#include "stdafx.h"
#include "screen_network_squad_browser.h"

#include "interface/user_interface_controller.h"
#include "shell/shell.h"
#include "tag_files/global_string_ids.h"


/* globals */

static int8* g_current_session_data = nullptr;
static c_text_widget* g_current_widget_item_variant_text = nullptr;

/* typedefs */

typedef c_screen_network_squad_browser* (__cdecl* load_network_browser_t)(s_screen_parameters*);
load_network_browser_t p_load_network_browser;
load_network_browser_t p_load_live_browser;

/* private code */

int8* __cdecl server_browser_cache_get_session_info(uint32 session_index)
{
	g_current_session_data = INVOKE(0x214FF7, 0, server_browser_cache_get_session_info, session_index);

	return g_current_session_data;
}

bool __cdecl server_browser_cache_session_is_valid(uint32 session_index)
{
	bool result = INVOKE(0x214FDD, 0, server_browser_cache_session_is_valid, session_index);

	if (result && g_current_widget_item_variant_text && g_current_session_data)
	{
		e_game_engine_type type = *(e_game_engine_type*)(g_current_session_data + 424);

		switch (type)
		{
		case _game_engine_type_none:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_unknown);
			break;
		case _game_engine_type_ctf:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_capture_the_flag);
			break;
		case _game_engine_type_slayer:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_slayer);
			break;
		case _game_engine_type_oddball:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_oddball);
			break;
		case _game_engine_type_koth:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_king_of_the_hill);
			break;
		case _game_engine_type_race:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_race);
			break;
		case _game_engine_type_headhunter:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_headhunter);
			break;
		case _game_engine_type_juggernaut:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_juggernaut);
			break;
		case _game_engine_type_territories:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_territories);
			break;
		case _game_engine_type_assault:
			g_current_widget_item_variant_text->set_text_from_string_id(_string_id_assault);
			break;
		}
	}

	return result;
}

/* public code */

__declspec(naked) void jmp_c_network_squad_list_update_list_items() { __asm { jmp  c_network_squad_list::update_list_items} };

void c_network_squad_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	g_current_widget_item_variant_text = item->try_find_text_widget(2);
	g_current_session_data = nullptr;

	INVOKE_TYPE(0x217F13, 0, void(__thiscall*)(c_network_squad_list*, c_list_item_widget*, int32), this, item, skin_index);
}

void c_network_squad_list::sort_event(s_event_record* event, int32 selected_column_index)
{
	INVOKE_TYPE(0x218EE7, 0, void(__thiscall*)(c_network_squad_list*, s_event_record*, int32), this, event, selected_column_index);
}

c_screen_network_squad_browser* c_screen_network_squad_browser::load_network_browser(s_screen_parameters* parameters)
{
	c_screen_network_squad_browser* screen = p_load_network_browser(parameters);

	return screen;
}

c_screen_network_squad_browser* c_screen_network_squad_browser::load_live_browser(s_screen_parameters* parameters)
{
	c_screen_network_squad_browser* screen = p_load_live_browser(parameters);

	screen->m_selected_column_index = _screen_network_squad_browser_column_players;
	screen->m_list_is_sorted = true;

	s_event_record t_event{};
	t_event.type = _user_interface_event_type_mouse_button_left_click;

	// this forces the server list to sort by player count default
	// this called twice because the default behaviour in sorting
	// is ascending instead of descending
	screen->m_list.sort_event(&t_event, _screen_network_squad_browser_column_players);
	screen->m_list.sort_event(&t_event, _screen_network_squad_browser_column_players);

	screen->m_selected_column_index = NONE;

	return screen;
}


#pragma region Live list fix for disappearing labels
CLASS_HOOK_DECLARE_LABEL(c_screen_network_squad_browser__build_players_list_fix, c_screen_network_squad_browser::build_players_list_fix);

void c_screen_network_squad_browser::build_players_list_fix(c_player_widget_representation* representations, int32 player_count)
{
	if (m_live_list)
	{
		// don't load the player list from data, 
		// since we are in the live list, and we don't use the details pane located in the lower right part where players are listed
		return;
	}

	apply_new_representations_to_players(representations, player_count);
}

__declspec(naked) void jmp_build_player_list()
{ 
	CLASS_HOOK_JMP(c_screen_network_squad_browser__build_players_list_fix, c_screen_network_squad_browser::build_players_list_fix);
}
#pragma endregion

void* __cdecl c_screen_network_squad_browser::load(s_screen_parameters* parameters)
{
	return INVOKE(0x21A238, 0x0, c_screen_network_squad_browser::load, parameters);
}

void c_screen_network_squad_browser::apply_patches()
{
	if (shell_is_dedicated_server()) return;

	PatchCall(Memory::GetAddressRelative(0x619650), jmp_build_player_list);

	// overriding these calls enables writing new strings into the variant column of the network browser list
	// without having to rewrite the entire class and data members
	PatchCall(Memory::GetAddress(0x217FEC), server_browser_cache_get_session_info);
	PatchCall(Memory::GetAddress(0x21828D), server_browser_cache_session_is_valid);
	WritePointer(Memory::GetAddress(0x3CFC0C), jmp_c_network_squad_list_update_list_items);

	DETOUR_ATTACH(p_load_network_browser, Memory::GetAddress<load_network_browser_t>(0x21A238), c_screen_network_squad_browser::load_network_browser);
	DETOUR_ATTACH(p_load_live_browser, Memory::GetAddress<load_network_browser_t>(0x21A2E4), c_screen_network_squad_browser::load_live_browser);
}
