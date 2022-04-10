#include "stdafx.h"

#include "NetworkSession.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"

bool NetworkSession::PlayerIsActive(int playerIdx)
{
	return (NetworkSession::GetCurrentNetworkSession()->membership[0].players_active_mask & FLAG(playerIdx)) != 0;
}

s_network_session* NetworkSession::GetNetworkSessions()
{
	return *Memory::GetAddress<s_network_session**>(0x51C474, 0x520B94); sizeof(s_membership_information);
}

s_network_session* NetworkSession::GetCurrentNetworkSession()
{
	return *Memory::GetAddress<s_network_session**>(0x420FE8, 0x3C40D0);
}

bool NetworkSession::GetCurrentNetworkSession(s_network_session** outSession)
{
	typedef bool(__cdecl* get_lobby_globals_t)(s_network_session**);
	return Memory::GetAddress<get_lobby_globals_t>(0x1AD736, 0x1A66B3)(outSession);
}

e_network_session_state NetworkSession::GetLocalSessionState()
{
	return GetCurrentNetworkSession()->local_session_state;
}

bool NetworkSession::LocalPeerIsSessionHost()
{
	e_network_session_state state = GetCurrentNetworkSession()->local_session_state;

	return state == _network_session_state_host_established
		|| state == _network_session_state_host_disband
		|| state == _network_session_state_host_handoff
		|| state == _network_session_state_host_reestablish;
}

bool NetworkSession::LocalPeerIsEstablished()
{
	e_network_session_state state = GetCurrentNetworkSession()->local_session_state;

	switch (state)
	{
	case _network_session_state_none:
	case _network_session_state_peer_joining:
	case _network_session_state_peer_join_abort:
	case _network_session_state_election:
	case _network_session_state_dead:
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

// returns NONE (-1) if fails
int NetworkSession::GetPeerIndexFromNetworkAddress(network_address* address)
{
	typedef int(__thiscall* get_peer_index_from_network_address_t)(s_network_session* session, network_address* address);
	auto p_get_peer_index_from_network_address = Memory::GetAddress<get_peer_index_from_network_address_t>(0x1C71DF, 0x19E9CF);
	return p_get_peer_index_from_network_address(GetCurrentNetworkSession(), address);
}

char NetworkSession::GetMapFileLocation(wchar_t* buffer, size_t size)
{
	// host-only
	typedef char(__thiscall* get_map_file_location_t)(s_network_session* session, wchar_t* buffer, size_t size);
	auto p_get_map_file_location = Memory::GetAddress<get_map_file_location_t>(0x1C5678, 0x19CD4A);
	return p_get_map_file_location(GetCurrentNetworkSession(), buffer, size);
}

int NetworkSession::GetPeerCount()
{
	return GetCurrentNetworkSession()->membership[0].peer_count;
}

int NetworkSession::GetLocalPeerIndex()
{
	return GetCurrentNetworkSession()->local_peer_index;
}

bool NetworkSession::PeerIndexLocal(int peerIdx)
{
	return GetLocalPeerIndex() == peerIdx;
}

IN_ADDR NetworkSession::GetLocalNetworkAddress()
{
	return GetCurrentNetworkSession()->membership[0].peer_data[GetLocalPeerIndex()].address.inaOnline;
}

int NetworkSession::GetPeerIndex(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->peer_index;
}

const wchar_t* NetworkSession::GetPeerPlayerName(int peerIdx)
{
	if (GetPeerCount() > 0)
	{
		for (int playerIdx = 0; playerIdx < ENGINE_PLAYER_MAX; playerIdx++)
		{
			if (GetPeerIndex(playerIdx) == peerIdx)
				return GetPlayerName(playerIdx);
		}
	}
	return nullptr;
}

int NetworkSession::GetPlayerCount()
{
	return GetCurrentNetworkSession()->membership[0].player_count;
}

s_player_information* NetworkSession::GetPlayerInformation(int playerIdx)
{
	return &GetCurrentNetworkSession()->membership[0].player_data[playerIdx];
}

int NetworkSession::GetPlayerIdByName(const wchar_t* name)
{
	if (GetPlayerCount() > 0)
	{
		for (int playerIdx = 0; playerIdx < ENGINE_PLAYER_MAX; playerIdx++)
		{
			if (PlayerIsActive(playerIdx) && wcscmp(GetPlayerName(playerIdx), name) == 0)
				return playerIdx;
		}
	}
	return NONE;
}

const wchar_t* NetworkSession::GetPlayerName(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->properties.player_name;
}

unsigned long long NetworkSession::GetPlayerXuid(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->identifier;
}

int NetworkSession::GetPlayerTeam(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->properties.player_team;
}

int NetworkSession::GetPlayerTeamFromXuid(long long xuid)
{
	if (GetPlayerCount() > 0)
	{
		int playerIdx = 0;
		do
		{
			if (PlayerIsActive(playerIdx) && GetPlayerXuid(playerIdx) == xuid)
				return GetPlayerTeam(playerIdx);

			playerIdx++;
		} while (playerIdx < 16);
	}
	return NONE;
}

int NetworkSession::GetPeerIndexFromId(long long xuid)
{
	if (GetPlayerCount() > 0)
	{
		int playerIdx = 0;
		do
		{
			if (PlayerIsActive(playerIdx) && GetPlayerXuid(playerIdx) == xuid)
				return GetPeerIndex(playerIdx);
			playerIdx++;
		} while (playerIdx < 16);
	}
	return NONE;
}

void NetworkSession::KickPeer(int peerIdx)
{
	typedef void(__thiscall* game_session_boot_t)(s_network_session*, int, bool);
	auto p_game_session_boot = Memory::GetAddress<game_session_boot_t>(0x1CCE9B);

	if (peerIdx < GetPeerCount())
	{
		LOG_TRACE_GAME("{} - about to kick peer index = {}", __FUNCTION__, peerIdx);
		p_game_session_boot(NetworkSession::GetCurrentNetworkSession(), peerIdx, true);
	}
}

void NetworkSession::EndGame()
{
	typedef void(__cdecl* end_game_t)();
	auto p_end_game = Memory::GetAddress<end_game_t>(0x215470, 0x197F32);
	p_end_game();
}

s_peer_observer_channel* NetworkSession::GetPeerObserverChannel(int peerIdx)
{
	return &GetCurrentNetworkSession()->peer_observer_channels[peerIdx];
}

wchar_t* NetworkSession::GetGameVariantName()
{
	return GetCurrentNetworkSession()->parameters[0].game_variant_name;
}

void NetworkSession::LeaveSession()
{
	if (Memory::IsDedicatedServer())
		return;

	if (s_game_globals::game_is_mainmenu())
	{
		// request_squad_browser
		*Memory::GetAddress<bool*>(0x978BAC) = true;

		typedef void(__cdecl* load_main_menu_with_context_t)(int context);
		auto p_load_main_menu_with_context = Memory::GetAddress<load_main_menu_with_context_t>(0x08EAF);
		p_load_main_menu_with_context(0);
	}

	typedef int(__cdecl* leave_game_type_t)(int a1);
	auto p_leave_session = Memory::GetAddress<leave_game_type_t>(0x216388);
	p_leave_session(0);
}

void NetworkSession::LogPlayersToConsole() {
	int playerIdx = 0;
	do
	{
		if (PlayerIsActive(playerIdx))
		{
			std::wstring outStr = L"Player index=" + std::to_wstring(playerIdx);
			outStr += L", Peer index=" + std::to_wstring(GetPeerIndex(playerIdx));
			outStr += L", PlayerName=";
			outStr += GetPlayerName(playerIdx);
			outStr += L", Name from game player state=";
			outStr += s_player::GetName(playerIdx);
			outStr += L", Team=" + std::to_wstring(GetPlayerTeam(playerIdx));
			outStr += L", Identifier=" + std::to_wstring(GetPlayerXuid(playerIdx));

			commands->output(outStr);
		}
		playerIdx++;
	} while (playerIdx < ENGINE_PLAYER_MAX);

	std::wstring total_players = L"Total players: " + std::to_wstring(GetPlayerCount());
	commands->output(total_players);
}

void NetworkSession::LogPeersToConsole() {
	if (GetPeerCount() > 0)
	{
		s_network_observer* observer = GetCurrentNetworkSession()->p_network_observer;

		for (int peerIdx = 0; peerIdx < GetPeerCount(); peerIdx++)
		{
			auto peer_observer_channel = &observer->observer_channels[GetCurrentNetworkSession()->peer_observer_channels[peerIdx].observer_index];

			std::wstring outStr = L"Peer index=" + std::to_wstring(peerIdx);
			outStr += L", Peer Name=";
			outStr += GetCurrentNetworkSession()->membership[0].peer_data[peerIdx].name;
			outStr += L", Connection Status=";
			outStr += std::to_wstring(peer_observer_channel->state);
			outStr += L", Peer map state: " + std::to_wstring(GetCurrentNetworkSession()->membership[0].peer_data[peerIdx].map_status);
			int playerIdx = GetCurrentNetworkSession()->membership[0].peer_data[peerIdx].player_index[0];
			if (playerIdx != -1)
			{
				outStr += L", Player index=" + std::to_wstring(playerIdx);
				outStr += L", Player name=";
				outStr += GetPlayerName(playerIdx);

				outStr += L", Name from game player state=";
				outStr += s_player::GetName(playerIdx);

			}
			commands->output(outStr);
		}
	}

	std::wstring total_players = L"Total peers: " + std::to_wstring(GetPeerCount());
	commands->output(total_players);
}

void NetworkSession::LogStructureOffsets() {

	std::wostringstream outStr;
	outStr << L"Offset of local_peer_index=" << std::hex << offsetof(s_network_session, local_peer_index);
	outStr << L", Offset of peer_observer_channels=" << std::hex << offsetof(s_network_session, peer_observer_channels);
	outStr << L", Offset of local_session_state=" << std::hex << offsetof(s_network_session, local_session_state);
	outStr << L", Offset of membership=" << std::hex << offsetof(s_network_session, membership);
	outStr << L", Offset of session_host_peer_index=" << std::hex << offsetof(s_network_session, session_host_peer_index);

	commands->output(outStr.str());
}

