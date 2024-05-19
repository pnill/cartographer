#include "stdafx.h"
#include "string_id.h"

void global_string_resolve_stringid_to_value(string_id id, wchar_t* dest)
{
	INVOKE(0x21DC38, 0x6AA91, global_string_resolve_stringid_to_value, id, dest);
}
