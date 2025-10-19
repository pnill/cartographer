#pragma once

/* prototypes */

int32 user_interface_available_squads_iterate(int32 available_squad_index);

bool user_interface_available_squad_exists(int32 session_index);

struct s_available_session* user_interface_available_squad_get(int32 server_index);

struct s_network_squad_status_data* user_interface_available_squad_get_status(int32 session_index);
