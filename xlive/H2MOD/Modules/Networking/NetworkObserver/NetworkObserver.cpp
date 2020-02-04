
#include "NetworkObserver.h"

#include "H2MOD.h"

void network_observer::sendNetworkMessage(int unk_index, int observer_index, bool send_out_of_band, int type, size_t size, void* data)
{
	typedef void(__thiscall* observer_channel_send_message)(void *observer, int unk_index, int observer_index, bool send_out_of_band, int type, size_t size, void* data);
	auto p_dynamic_packet_check = reinterpret_cast<observer_channel_send_message>(h2mod->GetAddress(0x1BED40, 0x1B8C1A));
	p_dynamic_packet_check(this, unk_index, observer_index, send_out_of_band, type, size, data);
}
