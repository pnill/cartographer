#include "stdafx.h"
#include "screen_esrb_warning.h"




void c_screen_esrb_warning::apply_patches()
{		
	//Disables the ESRB warning (only occurs for English Language).
	WriteValue<bool>(Memory::GetAddress(0x411030), 0);
	//disables the one after the intro video, by removing 0x40 flag from 0x7C6 bitmask
	WriteValue(Memory::GetAddress(0x39948 + 2), 0x7C6 & ~FLAG(6));
};
