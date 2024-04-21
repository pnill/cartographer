#include "stdafx.h"
#include "transport_qos.h"

#include "memory/data.h"
#include "memory/bitstream.h"

static XNQOS placeholder_xnqos;

s_data_array* transport_qos_attempts_array()
{
	return *Memory::GetAddress<s_data_array**>(0x526BF4, 0x991078);
}

// stub qos lookup function in-game between peers in a network session
datum __cdecl transport_qos_target_new_hook(int a1, int a2, int a3, int a4)
{
	s_data_array* qos_probes_datum_array = transport_qos_attempts_array();

	datum new_qos_datum_index = s_data_array::datum_new_in_range(qos_probes_datum_array);
	if (new_qos_datum_index != NONE)
	{
		uint8* qos_probe_data = (uint8*)datum_get(transport_qos_attempts_array(), new_qos_datum_index);
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

		*(int16*)(qos_probe_data + 2) = (int16)1;
		*(XNQOS**)(qos_probe_data + 4) = &placeholder_xnqos;
	}
	
	return new_qos_datum_index;
}