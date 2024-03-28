#include "stdafx.h"
#include "font_cache.h"

#include "interface/hud.h"


// Multiply font size by the global text scale to fix scaling issues on resolutions higher than 1080 pixels vertically
int __cdecl get_text_size_from_font_cache(int font_cache_index)
{
	auto p_get_text_size_from_font_cache = Memory::GetAddress<int(__cdecl*)(int)>(0x31865);
	return (int)((float)p_get_text_size_from_font_cache(font_cache_index) * *get_secondary_hud_scale());
}

void font_group_apply_hooks()
{
	PatchCall(Memory::GetAddress(0x6AE0D), get_text_size_from_font_cache);
}