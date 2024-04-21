#include "stdafx.h"

#include "Networking.h"
#include "Networking/Transport/transport_qos.h"

// with GFWL the abEnet address could be provided on startup, 
// not so much with our impl that requires you to log in to populate the data
// and H2v populates some data with it just on startup
// thus apply a patch to always update it
void MachineIDUpdatePatch()
{
	PatchCall(Memory::GetAddress(0x1B583F, 0x195C79), Memory::GetAddress(0x1B5DF3, 0x19622D));
	WriteJmpTo(Memory::GetAddress(0x1AC1B6, 0x1A6B6F), Memory::GetAddress(0x1B5DF3, 0x19622D));
	if (Memory::IsDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0, 0xBBCC), Memory::GetAddress(0, 0x19622D));
		PatchCall(Memory::GetAddress(0, 0xBBE3), Memory::GetAddress(0, 0x19622D));
	}
}

void CustomNetwork::ApplyPatches() 
{
	MachineIDUpdatePatch();
	NetworkMessage::ApplyGamePatches();

	// LIVE network protocol research
	c_network_observer::apply_patches();

	// stub QoS lookup function for mid-game data, our XNet QoS implementation is trash
	// we spawn one thread for each QoS lookup, for the serverlist it doesn't matter that much
	// plus the data collected isn't accurate
	PatchCall(Memory::GetAddress(0x1BDCB0, 0x1B7B8A), transport_qos_target_new_hook);
}