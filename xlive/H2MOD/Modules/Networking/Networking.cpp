
#include "Networking.h"
#include "Memory\bitstream.h"
#include "OverridePackets/OverridePackets.h"

#include "Util/Hooks/Hook.h"

bool decodePacketTypeAndSize(void *thisx, bitstream* stream, signed int *a3, int a4) {
	char *v4; // ebp@1
	signed int v5; // eax@2
	int v6; // eax@4
	bool result; // al@7

	v4 = (char *)thisx;
	*a3 = stream->data_decode_integer("type", 8);
	*(DWORD *)a4 = stream->data_decode_integer("size", 16);
	LOG_TRACE_NETWORK("[h2mod-network] received packet decoded type={0}, typeName={1}, size={2}", *a3, getNetworkMessageName(*a3), *(DWORD *)a4);
	if (stream->packet_is_valid()
		|| (v5 = *a3, *a3 < 0)
		|| v5 >= e_network_message_types::end
		|| (v6 = (int)&v4[32 * v5], !*(BYTE *)v6)
		|| *(DWORD *)a4 < *(DWORD *)(v6 + 12)
		|| *(DWORD *)a4 > *(DWORD *)(v6 + 16))
	{
		result = 0;
	}
	else
	{
		result = stream->packet_is_valid() == false;
	}
	return result;
}

typedef char(__stdcall *receive_packet)(void *thisx, void* a2, int packetType, unsigned int *size, void *packet_obj);
receive_packet receive_packet_method;

char __stdcall receivePacket(void *thisx, bitstream* stream, int packetType, unsigned int *size, void *packet_obj) {
	char *v5; // ebp@1
	int v6; // esi@2
	char result; // al@2
	LOG_TRACE_NETWORK("[h2mod-network] received packet");
	v5 = (char *)thisx;
	typedef bool(__thiscall* decode_type_and_size)(void* thisx, int a2, signed int* a3, int a4);
	decode_type_and_size decode_type_and_size_method = Memory::GetAddress<decode_type_and_size>(0x1E8217, 0x1CA1DA);
	if (decodePacketTypeAndSize(thisx, stream, (signed int *)packetType, (int)size))
	{
		LOG_TRACE_NETWORK("[h2mod-network] received packet succesfully decoded");
		v6 = (int)&v5[32 * *(DWORD *)packetType];
		SecureZeroMemory(packet_obj, *size);
		result = (*(int(__cdecl **)(void*, unsigned int, void *))(v6 + 24))(stream, *size, packet_obj);// calls packet read/write method
	}
	else
	{
		LOG_TRACE_NETWORK("[h2mod-network] received packet was invalid");
		result = 0;
	}
	return result;
}

typedef void(__cdecl *serialize_parameters_update_packet)(void* a1, int a2, int a3);
serialize_parameters_update_packet serialize_parameters_update_packet_method;

void __cdecl serializeParametersUpdatePacket(void* a1, int a2, int a3) {
	serialize_parameters_update_packet_method(a1, a2, a3);
}

typedef char(__stdcall *cmp_xnkid)(int thisx, int a2);
cmp_xnkid p_cmp_xnkid;

char __stdcall xnkid_cmp(int thisx, int a2) {
	return 1;
}

/* All this does is patch some checks that cause using actual ip addresses not to work. */
/* When a call to XNetXnaddrtoInaddr happens we provide the actual ip address rather than a secure key */
void removeXNetSecurity()
{
	/* XNKEY bs */
	p_cmp_xnkid = (cmp_xnkid)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1C284A, 0x199F02), (BYTE*)xnkid_cmp, 9);

	// apparently the secure address has 1 free byte 
	// after HTONL call, game is checking the al register (the lower 8 bits of eax register) if it is zero, if not everything network related will fail
	WriteValue<BYTE>(Memory::GetAddress(0x1B5DBE, 0x1961F8), 0xEB); // place a jump (jmp opcode = 0xEB)
	NopFill(Memory::GetAddress(0x1B624A, 0x196684), 2);
	NopFill(Memory::GetAddress(0x1B6201, 0x19663B), 2);
	NopFill(Memory::GetAddress(0x1B62BC, 0x1966F4), 2);
}

// stub qos lookup function in-game between peers in a network session
int __cdecl transport_qos_target_new_hook(int a1, signed int a2, int a3, int a4)
{
	static XNQOS placeholder_xnqos;

	s_data_array* qos_probes_datum_array = *Memory::GetAddress<s_data_array**>(0x526BF4, 0x991078);

	typedef int(__cdecl* datum_new_in_range)(s_data_array* datum_array);
	auto p_datum_new_in_range = Memory::GetAddress<datum_new_in_range>(0x667A0, 0x3248C);

	datum new_qos_datum_index = p_datum_new_in_range(qos_probes_datum_array);
	if (!DATUM_IS_NONE(new_qos_datum_index))
	{
		char* qos_probe_data = &qos_probes_datum_array->data[DATUM_ABSOLUTE_INDEX(new_qos_datum_index) * qos_probes_datum_array->datum_element_size];
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

// with GFWL this abOnline/abNet data was present on startup, but with our impl we don't have the abOnline/abNet data on startup
// so we just patch it to always update it instead of getting it only on startup
void patchAbNetUpdate()
{
	PatchCall(Memory::GetAddress(0x1B583F, 0x195C79), Memory::GetAddress(0x1B5DF3, 0x19622D));
	WriteJmpTo(Memory::GetAddress(0x1AC1B6, 0x1A6B6F), Memory::GetAddress(0x1B5DF3, 0x19622D));
	if (Memory::isDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0, 0xBBCC), Memory::GetAddress(0, 0x19622D));
		PatchCall(Memory::GetAddress(0, 0xBBE3), Memory::GetAddress(0, 0x19622D));
	}
}

void applyConnectionPatches()
{
	// live netcode research
	network_observer::ApplyPatches();

	// stub QoS lookup function for in-game data
	PatchCall(Memory::GetAddress(0x1BDCB0, 0x1B7B8A), transport_qos_target_new_hook);
}

void CustomNetwork::applyNetworkHooks() {
	DWORD serializeParametersUpdatePacketOffset = 0x1EDC41;

	///////////////////////////////////////////////
	//connection/player packet customizations below
	///////////////////////////////////////////////

	patchAbNetUpdate();

	// removing Xbox Secure Network (IPSec) from the game research
	//removeXNetSecurity();
	
	OverridePackets::ApplyGamePatches();
	CustomPackets::ApplyGamePatches();
	applyConnectionPatches();
}