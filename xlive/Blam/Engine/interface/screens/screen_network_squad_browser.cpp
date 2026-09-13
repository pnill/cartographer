#include "stdafx.h"
#include "screen_network_squad_browser.h"

#include "interface/user_interface.h"
#include "interface/user_interface_network_search.h"
#include "memory/data.h"
#include "networking/logic/network_logic.h"
#include "networking/transport/transport_security.h"

/* constants */

enum
{
	k_screen_live_squad_browser_max_items = 2000
};

/* typedefs */

typedef c_screen_network_squad_browser* (__cdecl* load_network_browser_t)(c_screen_parameters*);

/* structures */

struct s_locator_server_data
{
	int32 indicie;
	int32 session_index;
};

/* prototypes */

static __declspec(naked) void jmp_build_player_list(void)
{
	CLASS_HOOK_JMP(c_screen_network_squad_browser__build_players_list_fix, c_screen_network_squad_browser::build_players_list_fix);
}

static __declspec(naked) void jmp_populate_servers(void)
{
	CLASS_HOOK_JMP(c_screen_network_squad_browser__populate_servers, c_network_squad_list::populate_servers);
}

static void screen_network_squad_browser_increase_server_limit_apply_patches(void);

/* globals */

static load_network_browser_t p_load_network_browser;
static load_network_browser_t p_load_live_browser;

/* public code */

void c_screen_network_squad_browser::apply_patches(void)
{
	PatchCall(Memory::GetAddressRelative(0x619650), jmp_build_player_list);

	DETOUR_ATTACH(p_load_network_browser, Memory::GetAddress<load_network_browser_t>(0x21A238), c_screen_network_squad_browser::load_network_browser);
	DETOUR_ATTACH(p_load_live_browser, Memory::GetAddress<load_network_browser_t>(0x21A2E4), c_screen_network_squad_browser::load_live_browser);
	
	screen_network_squad_browser_increase_server_limit_apply_patches();
	return;
}

void c_network_squad_list::sort_event(
	s_event_record* event, 
	int32 selected_column_index)
{
	INVOKE_TYPE(0x218EE7, 0, void(__thiscall*)(c_network_squad_list*, s_event_record*, int32), this, event, selected_column_index);
	return;
}

c_screen_network_squad_browser* c_screen_network_squad_browser::load_network_browser(
	c_screen_parameters* parameters)
{
	return p_load_network_browser(parameters);
}

c_screen_network_squad_browser* c_screen_network_squad_browser::load_live_browser(
	c_screen_parameters* parameters)
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
	return;
}

#pragma endregion

void* __cdecl c_screen_network_squad_browser::load(
	c_screen_parameters* parameters)
{
	return INVOKE(0x21A238, 0x0, c_screen_network_squad_browser::load, parameters);
}


/* private code */

static void screen_network_squad_browser_increase_server_limit_apply_patches(void)
{
	WriteValue<int32>(Memory::GetAddress(0x214F82 + 1), k_screen_live_squad_browser_max_items);
	WriteValue<int32>(Memory::GetAddress(0x21A037 + 1), k_screen_live_squad_browser_max_items);
	
	PatchCall(Memory::GetAddress(0x2190B0), jmp_populate_servers);
	PatchCall(Memory::GetAddress(0x21A0B3), jmp_populate_servers);
	return;
}

CLASS_HOOK_DECLARE_LABEL(c_network_squad_list__populate_servers, c_network_squad_list::populate_servers);
void c_network_squad_list::populate_servers(void)
{
	if (m_list_data && m_populate_server_list)
	{
		int32 sort_field_index = 0;

		s_locator_server_data data[k_screen_live_squad_browser_max_items];
		data[0].indicie = -99;


		// Iterate throughout every available squad and populate the server data
		int32 available_squad_index = user_interface_available_squads_iterate(NONE);
		for (int32 server_index = 1; server_index < k_screen_live_squad_browser_max_items && available_squad_index != NONE; ++server_index)
		{
			populate_server_data(
				data,
				0,
				NONE,
				server_index,
				available_squad_index,
				m_sort_fields,
				NUMBEROF(m_sort_fields),
				0,
				m_use_live_squad_list
			);
			available_squad_index = user_interface_available_squads_iterate(available_squad_index);
		}

		const int32 actual_count = m_list_data->actual_count;
		int32 old_index = get_old_data_index();
		if (old_index == NONE)
		{
			old_index = data_next_index(m_list_data, NONE);
		}

		// Clear the previous server list data
		data_delete_all(m_list_data);

		// If we're in the lan list menu we make sure to create a new entry specifically for the "Create Game" button at the top of the list
		if (!m_use_live_squad_list)
		{
			sort_field_index = datum_new(m_list_data);

			s_network_squad_list_sort_fields* sort_fields = (s_network_squad_list_sort_fields*)datum_get(m_list_data, sort_field_index);
			sort_fields->session_index = NONE;
			sort_fields->field_8 = true;
		}

		s_transport_secure_address address;
		bool valid_address = transport_secure_address_get(&address, NULL);
		
		// Loop through every server indicie and add the entry to the list
		for (int32 i = data[0].indicie; i != -99 && sort_field_index != NONE; i = data[i].indicie)
		{
			const int32 session_index = data[i].session_index;
			const bool squad_exists = user_interface_available_squad_exists(session_index);
			const s_network_squad_status_data* status = squad_exists ? user_interface_available_squad_get_status(session_index) : NULL;
			const s_available_session* session = squad_exists ? user_interface_available_squad_get(session_index) : NULL;
			
			bool server_and_current_address_same = false;
			if (valid_address && status)
			{
				server_and_current_address_same = transport_secure_address_compare(&status->game_details.description.host_address, &address);
			}
			
			// Add field if the session is valid and if the server isn't a game that we're hosting ourself
			if (session && session->field_44 && !server_and_current_address_same)
			{
				sort_field_index = datum_new(m_list_data);
				if (sort_field_index == NONE)
				{
					break;
				}
				s_network_squad_list_sort_fields* sort_fields = (s_network_squad_list_sort_fields*)datum_get(m_list_data, sort_field_index);
				sort_fields->field_8 = false;
				sort_fields->session_index = session_index;
			}
		}

		bool valid = m_list[0].get_last_data_index() == NONE || actual_count != m_list_data->actual_count;

		// If we don't contain an empty entry at first, loop until we do
		for (int32 i = 0; i < m_list_count && m_list[i].last_data_index_valid() && !valid; ++i)
		{
			valid = datum_try_and_get(m_list_data, m_list[i].get_last_data_index()) == NULL;
		}

		// Only adjust index values if we're in a valid state
		if (valid)
		{
			for (int32 list_index = 0; list_index < m_list_count; ++list_index)
			{
				m_list[list_index].set_last_data_index(NONE);
			}

			int32 index = data_next_index(m_list_data, NONE);
			for (int32 list_index = 0; list_index < m_list_count && index != NONE; ++list_index)
			{
				m_list[list_index].set_last_data_index(index);
				index = data_next_index(m_list_data, index);
			}

			transfer_focus_by_item_index(old_index);
		}
	}
	return;
}

void c_network_squad_list::populate_server_data(
	struct s_locator_server_data* data,
	int32 unk_2,
	int32 unk_3,
	int32 field,
	int32 available_squad_index,
	s_network_squad_list_sort_fields* sort_fields,
	int32 sort_fields_count,
	int32 sort_field_index,
	bool use_live_squad_list
)
{
	INVOKE(0x2188AC, 0x0, populate_server_data,
		data,
		unk_2,
		unk_3,
		field,
		available_squad_index,
		sort_fields,
		sort_fields_count,
		sort_field_index,
		use_live_squad_list
	);
	return;
}