#include "stdafx.h"
#include "network_channel.h"

#include "Blam/Engine/networking/transport/transport_address.h"

s_network_channel* s_network_channel::Get(int channelIdx)
{
	s_network_channel* network_channels = *Memory::GetAddress<s_network_channel**>(0x4FADBC, 0x525274);
	return &network_channels[channelIdx];
}

bool s_network_channel::GetNetworkAddressFromNetworkChannel(network_address* out_addr)
{
	typedef bool(__thiscall* get_network_address_from_network_channel_t)(s_network_channel*, network_address*);
	auto p_get_network_address_from_network_channel = reinterpret_cast<get_network_address_from_network_channel_t>(Memory::GetAddress(0x1BA543, 0x1C9364));

	ZeroMemory(out_addr, sizeof(network_address));
	return p_get_network_address_from_network_channel(this, out_addr);
}