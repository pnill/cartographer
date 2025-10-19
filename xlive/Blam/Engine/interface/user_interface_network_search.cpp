#include "stdafx.h"
#include "user_interface_network_search.h"

#include "game/player_constants.h"
#include "networking/logic/network_logic.h"
#include "networking/logic/network_search.h"
#include "networking/network_game_definitions.h"

/* public code */

int32 user_interface_available_squads_iterate(int32 available_squad_index)
{
	int32 result = NONE;

	int32 i = available_squad_index != NONE ? available_squad_index + 1 : 0;
	for (; i < network_search_session_count(); ++i)
	{
		if (network_search_session(i))
		{
			result = i;
			break;
		}
	}
	return result;
}

bool user_interface_available_squad_exists(int32 session_index)
{
	return network_search_session(session_index) != NULL;
}

s_available_session* user_interface_available_squad_get(int32 server_index)
{
	return network_search_session(server_index);
}

s_network_squad_status_data* user_interface_available_squad_get_status(int32 session_index)
{
	s_network_squad_status_data* result = NULL;
   
	s_available_session* session = network_search_session(session_index);
	if (session)
	{
		if (session->status_data.game_details.player_count > k_maximum_players)
		{
			error(_error_silent, "%s: got invalid status_data", __FUNCTION__);
		}
		else
		{
			result = &session->status_data;
		}
	}
	return result;
}

