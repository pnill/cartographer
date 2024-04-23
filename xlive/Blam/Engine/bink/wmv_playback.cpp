#include "stdafx.h"
#include "wmv_playback.h"

bool __cdecl media_foundation_player_running(void)
{
	return INVOKE(0x39F09, 0x0, media_foundation_player_running);
}

void __cdecl media_foundation_player_frame(void)
{
	INVOKE(0x3A264, 0x0, media_foundation_player_frame);
	return;
}
