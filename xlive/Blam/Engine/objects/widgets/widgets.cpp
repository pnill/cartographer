#include "stdafx.h"
#include "widgets.h"

#include "main/interpolator.h"
#include "objects/objects.h"

#include "Util/Hooks/Hook.h"

void __cdecl widgets_new(datum object_index)
{
	INVOKE(0x14FFE2, 0x14CD7A, widgets_new, object_index);
	object_initialize_for_interpolation(object_index);
	return;
}

void __cdecl widgets_update(real32 dt)
{
	INVOKE(0x14FFB5, 0x0, widgets_update, dt);
	return;
}
