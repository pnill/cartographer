#include "stdafx.h"
#include "network_loading.h"

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

void __cdecl network_loading_create_thread(void)
{
	INVOKE(0x1AEB5A, 0x0, network_loading_create_thread);
	return;
}

/* private code */

static void __cdecl network_load_update()
{
	simulation_update();
	network_send();
}