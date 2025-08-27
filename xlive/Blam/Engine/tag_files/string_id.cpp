#include "stdafx.h"
#include "string_id.h"

void __cdecl user_interface_global_string_get(string_id id, c_maximum_interface_text* dest)
{
	INVOKE(0x21DC38, 0x6AA91, user_interface_global_string_get, id, dest);
	return;
}

string_id string_id_number(uint8 number)
{
	return INVOKE(0xCBF9B, 0, string_id_number, number);
}
