#include "stdafx.h"
#include "console.h"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "Util/Hooks/Hook.h"

namespace console
{
	// hook the print command to redirect the output to our console
	void __cdecl print_to_console(const char* output)
	{
		std::string finalOutput("[HSC Print] "); finalOutput += output;
		addDebugText(finalOutput.c_str());
	}

	void ApplyPatches()
	{
		PatchCall(Memory::GetAddress(0xE9E50), print_to_console);
	}
}
