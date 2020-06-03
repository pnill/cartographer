#include "stdafx.h"
#include "Globals.h"
#include "XboxTick.h"

int XboxTick::setTickRate(bool enable)
{
	int tickrate = 60;
	if (enable) 
	{
		tickrate = 30;

		BYTE mov_al_1_retn[] = { 0xB0, 0x01, 0xC3 };
		if (!h2mod->Server)
			WriteBytes(h2mod->GetAddress(0x3A938, 0x8DCE5), mov_al_1_retn, sizeof(mov_al_1_retn));

		LOG_TRACE_GAME("[h2mod] Set the game tickrate to 30");
	}
	else
	{
		// disables
		BYTE push_ebp_xor_bl_bl[] = { 0x53, 0x32, 0xDB };
		if (!h2mod->Server)
			WriteBytes(h2mod->GetAddress(0x3A938, 0x8DCE5), push_ebp_xor_bl_bl, sizeof(push_ebp_xor_bl_bl));

		LOG_TRACE_GAME("[h2mod] Set the game tickrate to 60");
	}

	WriteValue<DWORD>(h2mod->GetAddress(0x264ABB + 1, 0x1DB8B + 1), tickrate);
	return tickrate;
}

