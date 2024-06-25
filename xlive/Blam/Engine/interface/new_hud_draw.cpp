#include "stdafx.h"
#include "new_hud_draw.h"

/* public code */

datum hud_bitmap_tag_index_get(void)
{
	return *Memory::GetAddress<datum*>(0x976678);
}

int32 hud_bitmap_data_index_get(void)
{
	return *Memory::GetAddress<int32*>(0x97667C);
}

void __cdecl draw_hud_layer(void)
{
	INVOKE(0x22657B, 0x0, draw_hud_layer);
	return;
}
