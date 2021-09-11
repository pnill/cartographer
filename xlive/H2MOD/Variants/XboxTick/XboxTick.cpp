#include "XboxTick.h"

#include "Util/Hooks/Hook.h"

int XboxTick::setTickRate(bool enable)
{
	int tickrate = 60;
	if (enable) 
	{
		tickrate = 30;
		LOG_TRACE_GAME("[h2mod] Set the game tickrate to 30");
	}
	else
	{
		LOG_TRACE_GAME("[h2mod] Set the game tickrate to 60");
	}

	WriteValue<DWORD>(Memory::GetAddress(0x264ABB + 1, 0x1DB8B + 1), tickrate);
	return tickrate;
}

