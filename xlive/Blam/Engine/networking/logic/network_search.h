#pragma once

/* prototypes */

int32 network_search_session_count(void);

bool __cdecl network_search_initialize(void);

void __cdecl network_search_update(void);

struct s_available_session* network_search_session(int32 session_index);
