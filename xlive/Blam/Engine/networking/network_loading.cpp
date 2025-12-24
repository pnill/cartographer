#include "stdafx.h"

#include "network_loading.h"

#include "main/main.h"
#include "networking/network_globals.h"
#include "simulation/simulation.h"

/* prototypes */
static void __cdecl network_load_update();

/* public code */

void network_loading_apply_patches()
{
	// replace network_send with our hook
	PatchCall(Memory::GetAddress(0x1AEABF, 0x1AD666), network_load_update);
}

/* private code */

static void __cdecl network_load_update()
{
	simulation_update();
	network_send();
}