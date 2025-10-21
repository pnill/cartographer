#include "stdafx.h"
#include "network_search.h"

#include "network_logic.h"

/* structures */

struct s_network_search_criteria
{
	int8 gap[676];
};

struct s_network_search_globals
{
	int32 reference_count;
	bool xbox_live;
	int32 maximum_sessions;
	int32 maximum_sessions_1;
	uint32 time_since;
	bool search_active;
	s_network_search_criteria criteria;
	int32 available_sessions_count;
	int32 maximum_sessions_2;
	struct s_available_session* available_sessions;
	void* qos_status;
	int32 qos_pending_count;
	int32 field_2D0;
	int32 field_2D4;
	int32 field_2D8;
	void* field_2DC;
	int32 field_2E0;
	bool field_2E4;
};
ASSERT_STRUCT_SIZE(s_network_search_globals, 744);

/* prototypes */

static s_network_search_globals* network_search_globals_get(void);

static bool network_search_session_valid(const s_available_session* session);

/* public code */

int32 network_search_session_count(void)
{
	return network_search_globals_get()->available_sessions_count;
}

bool __cdecl network_search_initialize(void)
{
	return INVOKE(0x1E9D0A, 0x1A9410, network_search_initialize);
}

void __cdecl network_search_update(void)
{
	return INVOKE(0x1EA00A, 0x1A9710, network_search_update);
}

s_available_session* network_search_session(int32 session_index)
{
	s_available_session* result = NULL;

	s_network_search_globals* g_network_search_globals = network_search_globals_get();
	if (VALID_INDEX(session_index, g_network_search_globals->available_sessions_count))
	{
		s_available_session* session = &g_network_search_globals->available_sessions[session_index];
		if (network_search_session_valid(session))
		{
			result = session;
		}
	}
	return result;
}

/* private code */

static s_network_search_globals* network_search_globals_get(void)
{
	return Memory::GetAddress<s_network_search_globals*>(0x967440);
}

static bool network_search_session_valid(const s_available_session* session)
{
	ASSERT(session);
	return session->session_valid;
}
