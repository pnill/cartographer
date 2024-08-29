#include "stdafx.h"
#include "async.h"

#include "sound/sound_manager.h"

void __cdecl async_initialize(void)
{
	INVOKE(0x32CE5, 0x26284, async_initialize);
	return;
}

void __cdecl async_yield_until_done(s_async_completion* completion, bool play_sound)
{
	while(!completion->completed)
	{
		SwitchToThread();
		if (play_sound)
			sound_idle();
	}
}
