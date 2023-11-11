#include "stdafx.h"
#include "object_globals.h"

s_object_globals* object_globals_get(void)
{
	return *Memory::GetAddress<s_object_globals**>(0x4E4618, 0x50C8E8);
}

bool objects_can_connect_to_map(void)
{
	return object_globals_get() && object_globals_get()->initialized;
}