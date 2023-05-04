#include "stdafx.h"
#include "hud_definitions.h"

hud_globals_definition* get_hud_globals()
{
	return Memory::GetAddress<hud_globals_definition*>(0x9765C8, 0x99FBB0);
}