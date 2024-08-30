#include "stdafx.h"
#include "async.h"

#include "sound/sound_manager.h"

/* prototypes */

void __cdecl async_yield_until_done_function(s_async_completion* completion, bool(*yeild_function)(s_async_completion*), bool play_sound);
bool simple_yield_function(s_async_completion* completion);

/* public code */

void __cdecl async_initialize(void)
{
	INVOKE(0x32CE5, 0x26284, async_initialize);
	return;
}

void __cdecl async_yield_until_done(s_async_completion* completion, bool play_sound)
{
	async_yield_until_done_function(completion, simple_yield_function, play_sound);
	return;
}

/* private code */

void __cdecl async_yield_until_done_function(s_async_completion* completion, bool(*yeild_function)(s_async_completion*), bool play_sound)
{
	while(!yeild_function(completion))
	{
		SwitchToThread();
		if (play_sound)
		{
			sound_idle();
		}
	}
}

bool simple_yield_function(s_async_completion* completion)
{
	return completion->completed;
}
