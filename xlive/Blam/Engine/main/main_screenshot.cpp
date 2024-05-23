#include "stdafx.h"
#include "main_screenshot.h"

/* public code */

s_screenshot_globals* get_screenshot_globals(void)
{
	ASSERT(!Memory::IsDedicatedServer());
	return Memory::GetAddress<s_screenshot_globals*>(0xA55630);
}

bool screenshot_in_progress(void)
{
	return get_screenshot_globals()->taking_screenshot;
}

bool cubemap_screenshot_in_progress(void)
{
	const s_screenshot_globals* screenshot_globals = get_screenshot_globals();
	return screenshot_globals->taking_screenshot && screenshot_globals->cubemap_screenshot;
}
