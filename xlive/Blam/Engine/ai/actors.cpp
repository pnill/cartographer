#include "stdafx.h"
#include "actors.h"

/* public code */

data_array* get_actor_table(void)
{
	return *Memory::GetAddress<data_array**>(0xA965DC, 0x9A1C5C);
}

void __cdecl actors_initialize_for_new_map(void)
{
	INVOKE(0x315E6D, 0x2C0BD7, actors_initialize_for_new_map);
	return;
}
