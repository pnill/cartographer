#include "stdafx.h"
#include "transport.h"

#include "network_channel.h"
#include "Networking/NetworkMessageTypeCollection.h"

bool __cdecl transport_address_equivalent(const network_address* a1, const network_address* a2, bool check_network_port)
{
	return INVOKE(0x1F1EB1, 0x1B3BE0, transport_address_equivalent, a1, a2, check_network_port);
}

bool __cdecl transport_address_populate_from_network_channel_and_compare(s_network_channel* network_channel, network_address* a1, const network_address* a2, bool check_network_port)
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

// with GFWL the abEnet address could be provided on startup, 
// not so much with our impl that requires you to log in to populate the data
// and H2v populates some data with it just on startup
// thus apply a patch to always update it
void machine_id_update_patch()
{
	PatchCall(Memory::GetAddress(0x1B583F, 0x195C79), Memory::GetAddress(0x1B5DF3, 0x19622D));
	WriteJmpTo(Memory::GetAddress(0x1AC1B6, 0x1A6B6F), Memory::GetAddress(0x1B5DF3, 0x19622D));
	if (Memory::IsDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0, 0xBBCC), Memory::GetAddress(0, 0x19622D));
		PatchCall(Memory::GetAddress(0, 0xBBE3), Memory::GetAddress(0, 0x19622D));
	}
}

void network_transport_apply_patches()
{
	PatchCall(Memory::GetAddress(0x1BFEA7, 0x1B9D87), transport_address_populate_from_network_channel_and_compare_to_cdecl);

	machine_id_update_patch();
	NetworkMessage::ApplyGamePatches();

	// LIVE network protocol research
	c_network_observer::apply_patches();

	// stub QoS lookup function for mid-game data, our XNet QoS implementation is trash
	// we spawn one thread for each QoS lookup, for the serverlist it doesn't matter that much
	// plus the data collected isn't accurate
	PatchCall(Memory::GetAddress(0x1BDCB0, 0x1B7B8A), transport_qos_target_new_hook);
}