
#include "NetworkChannel.h"

#include "H2MOD.h"

network_channel* network_channel::getNetworkChannel(int channelIndex)
{
	network_channel* network_channels = *h2mod->GetAddress<network_channel**>(0x4FADBC, 0x525274);
	return &network_channels[channelIndex];
}

bool network_channel::getNetworkAddressFromNetworkChannel(network_address* out_addr)
{
	typedef bool(__thiscall* get_network_address_from_network_channel)(network_channel*, network_address*);
	auto p_get_network_address_from_network_channel = reinterpret_cast<get_network_address_from_network_channel>(h2mod->GetAddress(0x1BA543, 0x1C9364));

	SecureZeroMemory(out_addr, sizeof(network_address));
	return p_get_network_address_from_network_channel(this, out_addr);
}
