#include "stdafx.h"
#include "cinematics.h"

#include "Blam/Engine/game/game.h"
#include "Util/Hooks/Hook.h"

s_cinematic_globals* get_cinematic_globals(void)
{
	return *Memory::GetAddress<s_cinematic_globals**>(0x48227C, 0x4F3DB0);
}

bool cinematic_is_running()
{
	bool result = false;

	if (game_in_progress() && game_is_campaign()) 
	{ 
		s_cinematic_globals* cinematic_globals = get_cinematic_globals();
		if (cinematic_globals)
		{
			result = cinematic_globals->flags.cinematic_in_progress;
		}
	}

	return result;
}

// Same as above but hook used to disable framerate cap when playing cinematics
bool __cdecl cinematics_in_progress_disable_framerate_cap_hook(void)
{
	bool result = false;
	// don't limit the game framerate if we're single player and playing cinematics
	if (game_is_campaign())
	{
		result = false;
	}
	else
	{
		result = cinematic_is_running();
	}

	return result;
}

bool cinematic_in_progress(void)
{
	bool result = false;
	s_cinematic_globals* cinematic_globals = get_cinematic_globals();

	if (cinematic_globals)
	{
		result = cinematic_globals->flags.cinematic_in_progress;
	}

	return result;
}

void cinematics_apply_patches()
{
	// allow cinematics to run at 60 fps
	PatchCall(Memory::GetAddress(0x97774), cinematics_in_progress_disable_framerate_cap_hook);
	PatchCall(Memory::GetAddress(0x7C378), cinematics_in_progress_disable_framerate_cap_hook);
}