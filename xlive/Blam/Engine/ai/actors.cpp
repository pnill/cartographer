#include "stdafx.h"
#include "actors.h"

s_data_array* get_actor_table()
{
	return *Memory::GetAddress<s_data_array**>(0xA965DC, 0x9A1C5C);
}
