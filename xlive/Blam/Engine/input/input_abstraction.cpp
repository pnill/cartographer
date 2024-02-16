#include "stdafx.h"
#include "input_abstraction.h"

bool __cdecl input_abstraction_get_key_state(int16 key)
{
	return INVOKE(0x2EF86, 0x0, input_abstraction_get_key_state, key);
}
