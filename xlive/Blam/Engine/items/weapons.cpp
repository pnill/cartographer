#include "stdafx.h"
#include "weapons.h"

int32 __cdecl weapon_get_rounds_total(datum object_index, int32 magazine_index, bool a3)
{
	return INVOKE(0x15F313, 0x1435D3, weapon_get_rounds_total, object_index, magazine_index, a3);
}
