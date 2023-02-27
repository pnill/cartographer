#include "stdafx.h"
#include "object_globals.h"

s_object_globals* s_object_globals::get()
{
	return *Memory::GetAddress<s_object_globals**>(0x4E4618);
}

bool s_object_globals::objects_can_connect_to_map()
{
	return get() && get()->initialized;
}
