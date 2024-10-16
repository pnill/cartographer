#include "stdafx.h"
#include "render_visible_geometry.h"

/* public code */

uint32* visible_geometry_group_count_get(void)
{
	return Memory::GetAddress<uint32*>(0x4F4324);
}
