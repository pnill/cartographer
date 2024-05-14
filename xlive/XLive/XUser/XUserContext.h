#pragma once

typedef struct _XUSER_CONTEXT
{
	DWORD                               dwContextId;
	DWORD                               dwValue;
} XUSER_CONTEXT, *PXUSER_CONTEXT;

void context_update_map_info_campaign(uint32 map_id, const utf8* scenario_name);
