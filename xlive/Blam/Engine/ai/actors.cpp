#include "stdafx.h"
#include "actors.h"

namespace actors
{
	s_data_array* get_actor_table()
	{
		return *Memory::GetAddress<s_data_array**>(0xA965DC, 0x9A1C5C);
	}

	/* This looks at the actors table to get the character datum which is assigned to the specific actor. */
	int get_char_datum_from_actor(int actor_datum)
	{
		__int16 actor_index = actor_datum & 0xFFFF;
		DWORD actor_table_ptr = *Memory::GetAddress<DWORD*>(0xA965DC);
		DWORD actor_table = *(DWORD*)((BYTE*)actor_table_ptr + 0x44);
		auto const* actor = (s_actor*)((BYTE*)actor_table + (actor_index * 0x898));

		return actor->character_datum;
	}
}
