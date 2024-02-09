#include "stdafx.h"
#include "transport.h"

#include "NetworkChannel.h"

bool __cdecl transport_address_equivalent(const network_address* a1, const network_address* a2, bool check_network_port)
{
	return INVOKE(0x1F1EB1, 0x1B3BE0, transport_address_equivalent, a1, a2, check_network_port);
}

bool transport_address_populate_from_network_channel_and_compare(s_network_channel* network_channel, network_address* a1, const network_address* a2, bool check_network_port)
{
	bool equal = false;
	if (network_channel->get_network_address(a1))
	{
		equal = transport_address_equivalent(a1, a2, check_network_port);
	}
	return equal;
}

// we're creating an "usercall" here, to get the pointer to the network channel, which holds 
// the network address we need
__declspec(naked) void transport_address_populate_from_network_channel_and_compare_to_cdecl()
{
	__asm
	{
		// currently edi holds the pointer to the newtork channel
		mov eax, [esp+4+8] // +4 ret address +8 last arg and so on
		push eax
		mov eax, [esp+8+4]
		push eax
		mov eax, [esp+0xC+0]
		push eax
		// push network channel
		push edi
		call transport_address_populate_from_network_channel_and_compare
		add esp, 4 * 4 // 4 args
		retn
	}
}

void network_transport_apply_patches()
{
	PatchCall(Memory::GetAddress(0x1BFEA7, 0x1B9D87), transport_address_populate_from_network_channel_and_compare_to_cdecl);
}