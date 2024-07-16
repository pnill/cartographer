#include "stdafx.h"
#include "transport.h"

#include "network_channel.h"
#include "networking/NetworkMessageTypeCollection.h"

/* typedefs */

typedef void(__cdecl* transport_register_transition_functions_t)(void*, void*, void*, void*);

/* globals */

transport_register_transition_functions_t p_transport_register_transition_functions;

/* prototypes */

void __cdecl transport_register_transition_functions(
	void* startup_func,
	void* shutdown_func,
	void* reset_func,
	void* reset_parameter);
bool __cdecl transport_address_equivalent(const network_address* a1, const network_address* a2, bool check_network_port);
bool __cdecl transport_address_populate_from_network_channel_and_compare(
	s_network_channel* network_channel,
	network_address* a1,
	const network_address* a2,
	bool check_network_port);
void transport_address_populate_from_network_channel_and_compare_to_cdecl(void);
void machine_id_update_patch(void);

/* public code */

void network_transport_apply_patches(void)
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

	DETOUR_ATTACH(
		p_transport_register_transition_functions,
		Memory::GetAddress<transport_register_transition_functions_t>(0x1AC0C1, 0x1A6A7A),
		transport_register_transition_functions);
	return;
}

s_transport_globals* transport_globals_get(void)
{
	return Memory::GetAddress<s_transport_globals*>(0x4FAD10, 0x520A40);
}

bool transport_available(void)
{
	s_transport_globals* transport_globals = transport_globals_get();
	return transport_globals->initialized && transport_globals->field_1;
}

/* private code */

void __cdecl transport_register_transition_functions(
	void* startup_func,
	void* shutdown_func,
	void* reset_func,
	void* reset_parameter)
{
	ASSERT(startup_func != NULL || shutdown_func != NULL || reset_func != NULL);

	s_transport_globals* transport_globals = transport_globals_get();

	ASSERT(transport_globals->transition_function_count < k_number_of_transition_functions);

	transport_globals->startup_functions[transport_globals->transition_function_count] = startup_func;
	transport_globals->shutdown_functions[transport_globals->transition_function_count] = shutdown_func;
	transport_globals->reset_functions[transport_globals->transition_function_count] = reset_func;
	transport_globals->reset_parameters[transport_globals->transition_function_count] = reset_parameter;
	transport_globals->transition_function_count++;
	return;
}

bool __cdecl transport_address_equivalent(const network_address* a1, const network_address* a2, bool check_network_port)
{
	return INVOKE(0x1F1EB1, 0x1B3BE0, transport_address_equivalent, a1, a2, check_network_port);
}

bool __cdecl transport_address_populate_from_network_channel_and_compare(
	s_network_channel* network_channel,
	network_address* a1,
	const network_address* a2,
	bool check_network_port)
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
__declspec(naked) void transport_address_populate_from_network_channel_and_compare_to_cdecl(void)
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
void machine_id_update_patch(void)
{
	PatchCall(Memory::GetAddress(0x1B583F, 0x195C79), Memory::GetAddress(0x1B5DF3, 0x19622D));
	WriteJmpTo(Memory::GetAddress(0x1AC1B6, 0x1A6B6F), Memory::GetAddress(0x1B5DF3, 0x19622D));
	if (Memory::IsDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0, 0xBBCC), Memory::GetAddress(0, 0x19622D));
		PatchCall(Memory::GetAddress(0, 0xBBE3), Memory::GetAddress(0, 0x19622D));
	}

	return;
}

