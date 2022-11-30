#include "stdafx.h"
#include "aim_assist.h"
#include "game.h"

#include "Util/Hooks/Hook.h"

extern bool b_XboxTick;


namespace aim_assist
{
	// sword-flying target clear patch
	void __cdecl aim_assist_targeting_clear_hook(int target_data)
	{
		if (!s_game_globals::game_is_campaign() && !b_XboxTick)
		{
			*(DWORD*)(target_data) = -1;
			*(DWORD*)(target_data + 4) = -1;
			*(DWORD*)(target_data + 8) = -1;
			*(WORD*)(target_data + 24) = 0;
			*(DWORD*)(target_data + 28) = 0;
			*(DWORD*)(target_data + 32) = 0;
		}
	}
	
	void ApplyPatches()
	{
		PatchCall(Memory::GetAddress(0x169E59), aim_assist_targeting_clear_hook);
	}
}
