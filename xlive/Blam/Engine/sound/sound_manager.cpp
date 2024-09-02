#include "stdafx.h"
#include "sound_manager.h"

void __cdecl sound_initialize(void)
{
	INVOKE(0x2979E, 0x0, sound_initialize);
	return;
}

void sound_idle()
{
	INVOKE(0x2D9B2, 0, sound_idle);
}
