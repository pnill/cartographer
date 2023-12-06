#include "stdafx.h"
#include "game_state.h"

void __cdecl game_state_shell_initialize(void)
{
	INVOKE(0x30AA6, 0x53EAD, game_state_shell_initialize);
	return;
}

void* __cdecl game_state_malloc(const char* name, const char* description, uint32 size)
{
	return INVOKE(0x2FF1D, 0x53327, game_state_malloc, name, description, size);
}
