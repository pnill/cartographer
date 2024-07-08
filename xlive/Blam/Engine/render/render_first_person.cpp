#include "stdafx.h"
#include "render_first_person.h"

bool* rendering_fp_weapon_get(void)
{
	return Memory::GetAddress<bool*>(0xA3E3E6);
}

void __cdecl render_first_person(void)
{
	INVOKE(0x195E72, 0x0, render_first_person);
	return;
}
