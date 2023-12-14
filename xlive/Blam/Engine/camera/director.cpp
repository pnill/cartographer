#include "stdafx.h"
#include "director.h"

int32 __cdecl director_get_perspective(int32 user_index)
{
	return INVOKE(0x59E2B, 0x47E58, director_get_perspective, user_index);
}
