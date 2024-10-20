#include "stdafx.h"
#include "main.h"

/* public code */

bool __cdecl cinematic_sound_sync_complete(void)
{
	return INVOKE(0x39480, 0x40FA1, cinematic_sound_sync_complete);
}

void __cdecl main_loop(void)
{
	INVOKE(0x39E2C, 0xC668, main_loop);
	return;
}

void main_reset_map(void)
{
	*Memory::GetAddress<bool*>(0x48224E, 0x4A70C6) = true;
	return;
}

void main_quit()
{
	*Memory::GetAddress<bool*>(0x48220b, 0x4a7083) = true;
	return;
}

void __cdecl main_loop_pregame(int32 a1, int32 a2)
{
	INVOKE(0x3948C, 0x0, main_loop_pregame, a1, a2);
	return;
}
