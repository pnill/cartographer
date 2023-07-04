#include "stdafx.h"
#include "main_screenshot.h"

s_screenshot_globals* get_screenshot_globals()
{
	if (Memory::IsDedicatedServer()) 
		return nullptr;

	return Memory::GetAddress<s_screenshot_globals*>(0xA55630);
}
