#include "stdafx.h"
#include "widgets.h"

void __cdecl widgets_new(datum object_index)
{
	INVOKE(0x14FFE2, 0x14CD7A, widgets_new, object_index);
	return;
}

void __cdecl widgets_update(real32 dt)
{
	INVOKE(0x14FFB5, 0x0, widgets_update, dt);
	return;
}
