#include "stdafx.h"
#include <string>
#include <unordered_set>
#include <codecvt>

#include "Util\Hooks\Hook.h"

#include "H2MOD\Modules\UI\UI.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"


// Fixes pulse resetting the radar
void UI::Patch::RadarPulse() 
{
	if (H2IsDediServer)
		return;

	//g_D3DDevice9->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA ); to g_D3DDevice9->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA ); 
	 
	WriteValue<BYTE>(H2BaseAddr + 0x2849C4, (BYTE)4);
	addDebugText("Radar patched successfully.");
}