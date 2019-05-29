#include "stdafx.h"
#include "NetworkSession.h"

#include "H2MOD.h"

network_session* NetworkSession::getNetworkSession()
{
	network_session* netsession = reinterpret_cast<network_session*>(*(DWORD*)h2mod->GetAddress(0x51C474, 0x520B94));
	return netsession;
}

network_session* NetworkSession::getCurrentNetworkSession()
{
	network_session* netsession = reinterpret_cast<network_session*>(*(DWORD*)h2mod->GetAddress(0x420FE8, 0x520B94));
	return netsession;
}

bool NetworkSession::getCurrentNetworkSession(network_session** a1)
{
	typedef char(__cdecl* get_lobby_globals_ptr)(network_session** ptr);
	auto p_get_lobby_globals_ptr = reinterpret_cast<get_lobby_globals_ptr>(h2mod->GetBase() + ((h2mod->Server) ? 0x1A66B3 : 0x1AD736));

	return p_get_lobby_globals_ptr(a1) != 0;
}

bool NetworkSession::localPeerIsSessionHost()
{
	network_session_state state = getCurrentNetworkSession()->local_session_state;

	return state == _network_session_state_session_host
		|| state == _network_session_state_host_disband
		|| state == _network_session_state_host_handoff
		|| state == _network_session_state_host_reestablish;
}

signed int NetworkSession::getPeerIndexFromNetworkAddress(network_address* addr)
{
	typedef signed int(__thiscall* get_peer_index_from_network_address)(network_session *thisx, network_address *a2);
	auto p_get_peer_index_from_network_address = reinterpret_cast<get_peer_index_from_network_address>(h2mod->GetAddress(0x1C71DF, 0x19E9CF));
	return p_get_peer_index_from_network_address(getCurrentNetworkSession(), addr);
}

char NetworkSession::getMapFileLocation(network_session* thisx, wchar_t* buffer, size_t szBuffer)
{
	// host-only
	typedef char(__thiscall* get_map_file_location_impl)(network_session* thisx, wchar_t* buffer, size_t szBuffer);
	auto p_get_map_file_location_impl = reinterpret_cast<get_map_file_location_impl>(h2mod->GetBase() + ((h2mod->Server) ? 0x19CD4A : 0x1C5678));

	return p_get_map_file_location_impl(thisx, buffer, szBuffer);
}
