#include "stdafx.h"
#include "panorama_presence.h"

void __cdecl networking_panorama_presence_set_presence(e_context_presence context)
{
	INVOKE(0x1B07A0, 0x0, networking_panorama_presence_set_presence, context);
}
