#include "stdafx.h"

#include "Networking.h"
#include "Blam\Engine\memory\bitstream.h"
#include "OverridePackets\OverridePackets.h"

#include "Util\Hooks\Hook.h"

// stub qos lookup function in-game between peers in a network session
int __cdecl transport_qos_target_new_hook(int a1, signed int a2, int a3, int a4)
{
	static XNQOS placeholder_xnqos;

	s_data_array* qos_probes_datum_array = *Memory::GetAddress<s_data_array**>(0x526BF4, 0x991078);

	datum new_qos_datum_index = s_data_array::datum_new_in_range(qos_probes_datum_array);
	if (!DATUM_IS_NONE(new_qos_datum_index))
	{
		char* qos_probe_data = &qos_probes_datum_array->data[DATUM_INDEX_TO_ABSOLUTE_INDEX(new_qos_datum_index) * qos_probes_datum_array->datum_element_size];
		placeholder_xnqos.cxnqos = 1;
		placeholder_xnqos.cxnqosPending = 0;

		// setup some stub data
		placeholder_xnqos.axnqosinfo[0].bFlags = (0x02 | 0x01); // XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE
		placeholder_xnqos.axnqosinfo[0].cProbesRecv = 96;
		placeholder_xnqos.axnqosinfo[0].cProbesXmit = 96;
		placeholder_xnqos.axnqosinfo[0].dwDnBitsPerSec = 16384;
		placeholder_xnqos.axnqosinfo[0].dwUpBitsPerSec = 16384;
		placeholder_xnqos.axnqosinfo[0].wRttMinInMsecs = 50;
		placeholder_xnqos.axnqosinfo[0].wRttMedInMsecs = 64;
		placeholder_xnqos.axnqosinfo[0].cbData = 0;
		placeholder_xnqos.axnqosinfo[0].pbData = nullptr;

		*(short*)(qos_probe_data + 2) = (short)1;
		*(XNQOS**)(qos_probe_data + 4) = &placeholder_xnqos;
		return new_qos_datum_index;
	}
	else
		return -1; 
}

// with GFWL this abOnline/abEnet data was present on startup, but with our impl we don't have the abOnline/abEnet data on startup
// so we just patch it to always update it instead of getting it only on startup
void PatchabEnetUpdate()
{
	PatchCall(Memory::GetAddress(0x1B583F, 0x195C79), Memory::GetAddress(0x1B5DF3, 0x19622D));
	WriteJmpTo(Memory::GetAddress(0x1AC1B6, 0x1A6B6F), Memory::GetAddress(0x1B5DF3, 0x19622D));
	if (Memory::IsDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0, 0xBBCC), Memory::GetAddress(0, 0x19622D));
		PatchCall(Memory::GetAddress(0, 0xBBE3), Memory::GetAddress(0, 0x19622D));
	}
}

void CustomNetwork::ApplyPatches() {
	DWORD serializeParametersUpdatePacketOffset = 0x1EDC41;

	PatchabEnetUpdate();

	OverridePackets::ApplyGamePatches();
	NetworkMessage::ApplyGamePatches();

	// LIVE network protocol research
	s_network_observer::ApplyGamePatches();

	// stub QoS lookup function for mid-game data, our XNet QoS implementation is trash
	// we spawn one thread for each QoS lookup, for the serverlist it doesn't matter that much
	// plus the data collected isn't accurate
	PatchCall(Memory::GetAddress(0x1BDCB0, 0x1B7B8A), transport_qos_target_new_hook);
}