#include "NetworkSession.h"

#include "H2MOD/Modules/Console/ConsoleCommands.h"

network_session* NetworkSession::getNetworkSessions()
{
	return *Memory::GetAddress<network_session**>(0x51C474, 0x520B94);
}

network_session* NetworkSession::getCurrentNetworkSession()
{
	return *Memory::GetAddress<network_session**>(0x420FE8, 0x3C40D0);
}

bool NetworkSession::getCurrentNetworkSession(network_session** outSession)
{
	typedef bool(__cdecl* get_lobby_globals_ptr)(network_session**);
	return Memory::GetAddress<get_lobby_globals_ptr>(0x1AD736, 0x1A66B3)(outSession);
}

e_network_session_state NetworkSession::getLocalSessionState()
{
	return getCurrentNetworkSession()->local_session_state;
}

bool NetworkSession::localPeerIsSessionHost()
{
	e_network_session_state state = getCurrentNetworkSession()->local_session_state;

	return state == _network_session_state_host_established
		|| state == _network_session_state_host_disband
		|| state == _network_session_state_host_handoff
		|| state == _network_session_state_host_reestablish;
}

bool NetworkSession::localPeerIsEstablished()
{
	e_network_session_state state = getCurrentNetworkSession()->local_session_state;

	switch (state)
	{
	case _network_session_state_none:
	case _network_session_state_peer_joining:
	case _network_session_state_peer_join_abort:
	case _network_session_state_election:
	case _network_session_state_unk_2:
		return false;

	case _network_session_state_peer_established:
	case _network_session_state_peer_leaving:
	case _network_session_state_host_established:
	case _network_session_state_host_disband:
	case _network_session_state_host_handoff:
	case _network_session_state_host_reestablish:
		return true;

	default:
		return false;
	}
}

signed int NetworkSession::getPeerIndexFromNetworkAddress(network_address* address)
{
	typedef signed int(__thiscall* get_peer_index_from_network_address)(network_session* session, network_address* address);
	return Memory::GetAddress<get_peer_index_from_network_address>(0x1C71DF, 0x19E9CF)(getCurrentNetworkSession(), address);
}

char NetworkSession::getMapFileLocation(wchar_t* buffer, size_t size)
{
	// host-only
	typedef char(__thiscall* get_map_file_location_impl)(network_session* session, wchar_t* buffer, size_t size);
	return Memory::GetAddress<get_map_file_location_impl>(0x1C5678, 0x19CD4A)(getCurrentNetworkSession(), buffer, size);
}

int NetworkSession::getPeerCount()
{
	return getCurrentNetworkSession()->membership.peer_count;
}



int NetworkSession::getLocalPeerIndex()
{
	return getCurrentNetworkSession()->local_peer_index;
}
IN_ADDR NetworkSession::getLocalNetworkAddress()
{
	return getCurrentNetworkSession()->membership.peer_info[getLocalPeerIndex()].address.inaOnline;
}

int NetworkSession::getPeerIndex(int playerIndex)
{
	return getPlayerInformation(playerIndex)->peer_index;
}

wchar_t* NetworkSession::getPeerPlayerName(int peerIndex)
{
	if (getPeerCount() > 0)
	{
		int playerIndex = 0;
		do
		{
			if (getPeerIndex(playerIndex) == peerIndex)
				return getPlayerName(playerIndex);
			playerIndex++;
		} while (playerIndex < 16);
	}
	return L"";
}
/* Use this to verify if a player is currently active in the network session */
/* Otherwise you will wonder why you don't get the right data/player index etc. */
bool NetworkSession::playerIsActive(int playerIndex)
{
	return (NetworkSession::getCurrentNetworkSession()->membership.players_active_mask & FLAG(playerIndex)) != 0;
}

int NetworkSession::getPlayerCount()
{
	return getCurrentNetworkSession()->membership.player_count;
}

player_information* NetworkSession::getPlayerInformation(int playerIndex)
{
	return &getCurrentNetworkSession()->membership.player_info[playerIndex];
}


int NetworkSession::getPlayerIdByName(wchar_t* name)
{
	if (getPlayerCount() > 0)
	{
		int playerIndex = 0;
		do
		{
			if (playerIsActive(playerIndex) && wcscmp(getPlayerName(playerIndex), name) == 0)
				return playerIndex;

			playerIndex++;
		} while (playerIndex < 16);
	}
	return NONE;
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
	return NONE;
}

int NetworkSession::getPeerIndexFromXUID(long long xuid)
{
	if(getPlayerCount() > 0)
	{
		int playerIndex = 0;
		do
		{
			if (playerIsActive(playerIndex) && getPlayerXuid(playerIndex) == xuid)
				return getPeerIndex(playerIndex);
			playerIndex++;
		} while (playerIndex < 16);
	}
	return NONE;
}

void NetworkSession::kickPeer(int peerIndex) 
{
	typedef void(__thiscall* game_session_boot)(network_session* session, int peer_index, bool unk);
	auto p_game_session_boot = Memory::GetAddress<game_session_boot>(0x1CCE9B);

	if (peerIndex < getPeerCount()) 
	{
		LOG_TRACE_GAME("NetworkSession::kickPeer() - about to kick peer index = {}", peerIndex);
		p_game_session_boot(NetworkSession::getCurrentNetworkSession(), peerIndex, true);
	}
}

void NetworkSession::endGame()
{
	typedef void(__cdecl* end_game)();
	auto p_end_game = Memory::GetAddress<end_game>(0x215470, 0x197F32);
	p_end_game();
}

peer_observer_channel* NetworkSession::getPeerObserverChannel(int peerIndex)
{
	return &getCurrentNetworkSession()->peer_observer_channels[peerIndex];
}

wchar_t* NetworkSession::getGameVariantName()
{
	return getCurrentNetworkSession()->parameters.game_variant_name;
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
			outStr += Player::getName(playerIndex);
			outStr += L", Team=" + std::to_wstring(getPlayerTeam(playerIndex));
			outStr += L", Identifier=" + std::to_wstring(getPlayerXuid(playerIndex));

			commands->output(outStr);
		}
		playerIndex++;
	} 
	while (playerIndex < ENGINE_PLAYER_MAX);

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
			auto peer_observer = &observer->observers[getCurrentNetworkSession()->peer_observer_channels[peerIndex].observer_index];

			std::wstring outStr = L"Peer index=" + std::to_wstring(peerIndex);
			outStr += L", Peer Name=";
			outStr += getCurrentNetworkSession()->membership.peer_info[peerIndex].name;
			outStr += L", Connection Status=";
			outStr += std::to_wstring(peer_observer->state);
			int playerIndex = getCurrentNetworkSession()->membership.peer_info[peerIndex].player_index[0];
			outStr += L", Peer map state: " + std::to_wstring(getCurrentNetworkSession()->membership.peer_info[peerIndex].map_status);
			if (playerIndex != -1) 
			{
				outStr += L", Player index=" + std::to_wstring(playerIndex);
				outStr += L", Player name=";
				outStr += getPlayerName(playerIndex);

				outStr += L", Name from game player state=";
				outStr += Player::getName(playerIndex);

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

