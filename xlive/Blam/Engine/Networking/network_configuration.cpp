#include "stdafx.h"
#include "network_configuration.h"

void __cdecl network_configuration_initialize(void)
{
	// modifies esi need to check what the caller sets that too
	// Update 1/13/2022:
	// initializes some interface that's used to download the network config from the bungie's website
	// but just a stub in release
	INVOKE(0x001A9DE6, 0x0, network_configuration_initialize);
	return;
}
