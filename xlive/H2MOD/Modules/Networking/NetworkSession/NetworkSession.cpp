#include "stdafx.h"
#include "NetworkSession.h"

#include "H2MOD/Modules/Console/ConsoleCommands.h"

network_session* NetworkSession::getNetworkSessions()
{
	return *h2mod->GetAddress<network_session**>(0x51C474, 0x520B94);
}

network_session* NetworkSession::getCurrentNetworkSession()
{
	return *h2mod->GetAddress<network_session**>(0x420FE8, 0x3C40D0);
}

bool NetworkSession::getCurrentNetworkSession(network_session** outSession)
{
	typedef char(__cdecl* get_lobby_globals_ptr)(network_session** ptr);
	return h2mod->GetAddress<get_lobby_globals_ptr>(0x1AD736, 0x1A66B3)(outSession);
}

eNetwork_session_state NetworkSession::getLocalSessionState()
{
	return getCurrentNetworkSession()->local_session_state;
}

bool NetworkSession::localPeerIsSessionHost()
{
	eNetwork_session_state state = getCurrentNetworkSession()->local_session_state;

	return state == network_session_state_session_host
		|| state == network_session_state_host_disband
		|| state == network_session_state_host_handoff
		|| state == network_session_state_host_reestablish;
}

signed int NetworkSession::getPeerIndexFromNetworkAddress(network_address* address)
{
	typedef signed int(__thiscall* get_peer_index_from_network_address)(network_session* session, network_address* address);
	return h2mod->GetAddress<get_peer_index_from_network_address>(0x1C71DF, 0x19E9CF)(getCurrentNetworkSession(), address);
}

char NetworkSession::getMapFileLocation(wchar_t* buffer, size_t size)
{
	// host-only
	typedef char(__thiscall* get_map_file_location_impl)(network_session* session, wchar_t* buffer, size_t size);
	return h2mod->GetAddress<get_map_file_location_impl>(0x1C5678, 0x19CD4A)(getCurrentNetworkSession(), buffer, size);
}

int NetworkSession::getPeerCount()
{
	return getCurrentNetworkSession()->membership.peer_count;
}

int NetworkSession::getLocalPeerIndex()
{
	return getCurrentNetworkSession()->local_peer_index;
}

int NetworkSession::getPeerIndex(int playerIndex)
{
	return getPlayerInformation(playerIndex)->peer_index;
}

/* Use this to verify if a player is currently active in the network session */
/* Otherwise you will wonder why you don't get the right data/player index etc. */
bool NetworkSession::playerIsActive(int playerIndex)
{
	return (1 << playerIndex) & NetworkSession::getCurrentNetworkSession()->membership.players_active_mask;
}

int NetworkSession::getPlayerCount()
{
	return getCurrentNetworkSession()->membership.player_count;
}

player_information* NetworkSession::getPlayerInformation(int playerIndex)
{
	return &getCurrentNetworkSession()->membership.player_info[playerIndex];
}

wchar_t* NetworkSession::getPlayerName(int playerIndex)
{
	return getPlayerInformation(playerIndex)->properties.player_name;
}

long long NetworkSession::getPlayerXuid(int playerIndex)
{
	return getPlayerInformation(playerIndex)->identifier;
}

int NetworkSession::getPlayerTeam(int playerIndex)
{
	return getPlayerInformation(playerIndex)->properties.player_team;
}

int NetworkSession::getPlayerTeamFromXuid(long long xuid) 
{
	if (getPlayerCount() > 0) 
	{
		int playerIndex = 0;
		do 
		{
			if (playerIsActive(playerIndex) && getPlayerXuid(playerIndex) == xuid)
				return getPlayerTeam(playerIndex);

			playerIndex++;
		} 
		while (playerIndex < 16);
	}
	return -1;
}

void NetworkSession::kickPeer(int peerIndex) 
{
	typedef void(__thiscall* game_session_boot)(network_session* session, int peer_index, bool unk);
	auto p_game_session_boot = h2mod->GetAddress<game_session_boot>(0x1CCE9B);

	if (peerIndex < getPeerCount()) 
	{
		LOG_TRACE_GAME("NetworkSession::kickPeer() - about to kick peer index = {}", peerIndex);
		p_game_session_boot(NetworkSession::getCurrentNetworkSession(), peerIndex, true);
	}
}

peer_observer_channel* NetworkSession::getPeerObserverChannel(int peerIndex)
{
	return &getCurrentNetworkSession()->peer_observer_channels[peerIndex];
}

void NetworkSession::logPlayersToConsole() {
	int playerIndex = 0;
	do 
	{
		if (playerIsActive(playerIndex)) 
		{
			std::wstring outStr = L"Player index=" + std::to_wstring(playerIndex);
			outStr += L", Peer index=" + std::to_wstring(getPeerIndex(playerIndex));
			outStr += L", PlayerName=";
			outStr += getPlayerName(playerIndex);
			outStr += L", Name from game player state=";
			outStr += h2mod->get_player_name_from_player_index(playerIndex);
			outStr += L", Team=" + std::to_wstring(getPlayerTeam(playerIndex));
			outStr += L", Identifier=" + std::to_wstring(getPlayerXuid(playerIndex));

			commands->output(outStr);
		}
		playerIndex++;
	} 
	while (playerIndex < 16);

	std::wstring total_players = L"Total players: " + std::to_wstring(getPlayerCount());
	commands->output(total_players);
}

void NetworkSession::logPeersToConsole() {
	if (getPeerCount() > 0)
	{
		network_observer* observer = getCurrentNetworkSession()->network_observer_ptr;

		int peerIndex = 0;
		do 
		{
			std::wstring outStr = L"Peer index=" + std::to_wstring(peerIndex);
			outStr += L", Peer Name=";
			outStr += getCurrentNetworkSession()->membership.peer_info[peerIndex].name;
			outStr += L", Unknown state=" + std::to_wstring(observer->getObserverState(getCurrentNetworkSession()->peer_observer_channels[peerIndex].observer_index));
			int playerIndex = getCurrentNetworkSession()->membership.peer_info[peerIndex].player_index[0];
			if (playerIndex != -1) 
			{
				outStr += L", Player index=" + std::to_wstring(playerIndex);
				outStr += L", Player name=";
				outStr += getPlayerName(playerIndex);
				outStr += L", Name from game player state=";
				outStr += h2mod->get_player_name_from_player_index(playerIndex);
				outStr += L", Identifier=" + std::to_wstring(getPlayerXuid(playerIndex));
			}
			commands->output(outStr);

			peerIndex++;
		} 
		while (peerIndex < getPeerCount());
	}

	std::wstring total_players = L"Total peers: " + std::to_wstring(getPeerCount());
	commands->output(total_players);
}

void NetworkSession::logStructureOffsets() {
	
	std::wostringstream outStr;
	outStr << L"Offset of local_peer_index=" << std::hex << offsetof(network_session, local_peer_index);
	outStr << L", Offset of peer_observer_channels=" << std::hex << offsetof(network_session, peer_observer_channels);
	outStr << L", Offset of local_session_state=" << std::hex << offsetof(network_session, local_session_state);
	outStr << L", Offset of membership=" << std::hex << offsetof(network_session, membership);
	outStr << L", Offset of session_host_peer_index=" << std::hex << offsetof(network_session, session_host_peer_index);
			
	commands->output(outStr.str());
}

