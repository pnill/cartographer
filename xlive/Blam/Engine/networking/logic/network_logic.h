#pragma once
#include "networking/network_game_definitions.h"

/* structures */

struct s_available_session
{
	bool session_valid;
	int8 gap[67];
	bool field_44;
	int8 gap1[36];
	bool status_data_valid;
	int32 field_6C;
	s_network_squad_status_data status_data;
};
