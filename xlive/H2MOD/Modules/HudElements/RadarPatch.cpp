#include "stdafx.h"

#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "Util\Hooks\Hook.h"

void RadarPatch() {

	if (H2IsDediServer)
		return;

	/* g_D3DDevice9->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA );
       to
	   g_D3DDevice9->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA ); */
	
	// 4 = D3DBLEND_INVSRCALPHA
	WriteValue<BYTE>(H2BaseAddr + 0x2849C4, (BYTE)4);
	addDebugText("Motion sensor patched successfully.");
}