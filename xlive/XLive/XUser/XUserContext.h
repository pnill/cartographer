#pragma once

enum e_context_presence : uint32
{
	_context_presence_mainmenu = 0,
	_context_presence_settings = 1,
	_context_presence_server_browser = 2,
	_context_presence_singleplayer = 3,
	_context_presence_lobby = 4,
	_context_presence_results = 5,
	_context_presence_live_in_game = 6,
	_context_presence_public_game = 7,
	_context_presence_invite_only_game = 8,
	_context_presence_network_in_game = 9
};

typedef struct _XUSER_CONTEXT
{
	DWORD                               dwContextId;
	DWORD                               dwValue;
} XUSER_CONTEXT, *PXUSER_CONTEXT;

void context_update_map_info_campaign(uint32 map_id, const utf8* scenario_name);
