
#include "NetworkObserver.h"

#include "H2MOD.h"

void network_observer::sendNetworkMessage(int unk_index, int observer_index, e_network_message_send_protocol send_out_of_band, int type, int size, void* data)
{
	typedef void(__thiscall* observer_channel_send_message)(void*, int, int, e_network_message_send_protocol, int, int, void*);
	auto p_observer_channel_send_message = reinterpret_cast<observer_channel_send_message>(h2mod->GetAddress(0x1BED40, 0x1B8C1A));

	p_observer_channel_send_message(this, unk_index, observer_index, send_out_of_band, type, size, data);
}
