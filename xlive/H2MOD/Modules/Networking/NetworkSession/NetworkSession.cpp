#include "stdafx.h"
#include "NetworkSession.h"
#include "H2MOD.h"

network_session* NetworkSession::getNetworkSession()
{
	return reinterpret_cast<network_session*>(*h2mod->GetPointer<DWORD*>(0x51C474, 0x520B94));
}

network_session* NetworkSession::getCurrentNetworkSession()
{
	return reinterpret_cast<network_session*>(*h2mod->GetPointer<DWORD*>(0x420FE8, 0x520B94));
}

bool NetworkSession::getCurrentNetworkSession(network_session** a1)
{
	typedef char(__cdecl* get_lobby_globals_ptr)(network_session** ptr);
	return reinterpret_cast<get_lobby_globals_ptr>(h2mod->GetPointer(0x1AD736, 0x1A66B3))(a1);
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
	return reinterpret_cast<get_peer_index_from_network_address>(h2mod->GetPointer(0x1C71DF, 0x19E9CF))(getCurrentNetworkSession(), addr);
}

char NetworkSession::getMapFileLocation(network_session* thisx, wchar_t* buffer, size_t szBuffer)
{
	// host-only
	typedef char(__thiscall* get_map_file_location_impl)(network_session* thisx, wchar_t* buffer, size_t szBuffer);
	return reinterpret_cast<get_map_file_location_impl>(h2mod->GetPointer(0x1C5678, 0x19CD4A))(thisx, buffer, szBuffer);
}

void NetworkSession::kick_player(int peerIndex) {
	typedef void(__thiscall* game_session_boot)(network_session* session, int peer_index, bool a3);
	auto p_game_session_boot = reinterpret_cast<game_session_boot>(h2mod->GetAddress(0x1CCE9B));

	LOG_TRACE_GAME("about to kick player index = {}", peerIndex);
	p_game_session_boot(NetworkSession::getCurrentNetworkSession(), peerIndex, true);
}
