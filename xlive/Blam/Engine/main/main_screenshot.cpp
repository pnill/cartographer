#include "stdafx.h"
#include "main_screenshot.h"

s_screenshot_globals* get_screenshot_globals(void)
{
	ASSERT(!Memory::IsDedicatedServer());
	return Memory::GetAddress<s_screenshot_globals*>(0xA55630);
}

bool screenshot_in_progress(void)
{
	return get_screenshot_globals()->taking_screenshot;
}
