#include "stdafx.h"

#include "NetworkSession.h"
#include "Blam\Engine\Game\GameGlobals.h"

bool NetworkSession::PlayerIsActive(int playerIdx)
{
	return (NetworkSession::GetActiveNetworkSession()->membership[0].players_active_mask & FLAG(playerIdx)) != 0;
}

std::vector<unsigned long long> NetworkSession::GetActivePlayerIdList()
{
	std::vector<unsigned long long> activePlayerIdList;
	if (NetworkSession::GetPlayerCount() > 0)
	{
		for (int playerIdx = 0; playerIdx < ENGINE_MAX_PLAYERS; playerIdx++)
		{
			if (NetworkSession::PlayerIsActive(playerIdx))
			{
				unsigned long long playerId = NetworkSession::GetPlayerId(playerIdx);
				activePlayerIdList.emplace_back(playerId);
			}
		}
	}

	return activePlayerIdList;
}

std::vector<int> NetworkSession::GetActivePlayerIndicesList()
{
	std::vector<int> activePlayersIndices;
	if (NetworkSession::GetPlayerCount() > 0)
	{
		for (int playerIndex = 0; playerIndex < ENGINE_MAX_PLAYERS; playerIndex++)
		{
			if (NetworkSession::PlayerIsActive(playerIndex))
				activePlayersIndices.emplace_back(playerIndex);
		}
	}

	return activePlayersIndices;
}

s_network_session* NetworkSession::GetNetworkSessions()
{
	return *Memory::GetAddress<s_network_session**>(0x51C474, 0x520B94);
}

s_network_session* NetworkSession::GetActiveNetworkSession()
{
	return *Memory::GetAddress<s_network_session**>(0x420FE8, 0x3C40D0);
}

bool NetworkSession::GetActiveNetworkSession(s_network_session** outSession)
{
	typedef bool(__cdecl* get_lobby_globals_t)(s_network_session**);
	return Memory::GetAddress<get_lobby_globals_t>(0x1AD736, 0x1A66B3)(outSession);
}

e_network_session_state NetworkSession::GetLocalSessionState()
{
	return GetActiveNetworkSession()->local_session_state;
}

bool NetworkSession::LocalPeerIsSessionHost()
{
	e_network_session_state state = GetActiveNetworkSession()->local_session_state;

	return state == _network_session_state_host_established
		|| state == _network_session_state_host_disband
		|| state == _network_session_state_host_handoff
		|| state == _network_session_state_host_reestablish;
}

bool NetworkSession::LocalPeerIsSessionLeader()
{
	return GetActiveNetworkSession()->membership[0].session_leader_peer_index == GetLocalPeerIndex();
}

bool NetworkSession::LocalPeerIsEstablished()
{
	e_network_session_state state = GetActiveNetworkSession()->local_session_state;

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
	get_peer_index_from_network_address_t p_get_peer_index_from_network_address = Memory::GetAddress<get_peer_index_from_network_address_t>(0x1C71DF, 0x19E9CF);
	return p_get_peer_index_from_network_address(GetActiveNetworkSession(), address);
}

// host-only
bool NetworkSession::GetMapFileLocation(wchar_t* buffer, size_t size)
{
	typedef bool(__thiscall* get_map_file_location_t)(s_network_session* session, wchar_t* buffer, size_t size);
	get_map_file_location_t p_get_map_file_location = Memory::GetAddress<get_map_file_location_t>(0x1C5678, 0x19CD4A);
	return p_get_map_file_location(GetActiveNetworkSession(), buffer, size);
}

int NetworkSession::GetPeerCount()
{
	return GetActiveNetworkSession()->membership[0].peer_count;
}

int NetworkSession::GetLocalPeerIndex()
{
	return GetActiveNetworkSession()->local_peer_index;
}

bool NetworkSession::IsPeerIndexLocal(int peerIdx)
{
	return GetLocalPeerIndex() == peerIdx;
}

IN_ADDR NetworkSession::GetLocalNetworkAddress()
{
	return GetActiveNetworkSession()->membership[0].peers[GetLocalPeerIndex()].secure_address.inaOnline;
}

int NetworkSession::GetPeerIndex(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->peer_index;
}

int NetworkSession::GetPlayerCount()
{
	return GetActiveNetworkSession()->membership[0].player_count;
}

s_membership_player* NetworkSession::GetPlayerInformation(int playerIdx)
{
	return &GetActiveNetworkSession()->membership[0].players[playerIdx];
}

const wchar_t* NetworkSession::GetPlayerName(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->properties[0].player_name;
}

unsigned long long NetworkSession::GetPlayerId(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->identifier;
}

int NetworkSession::GetPlayerTeam(int playerIdx)
{
	return GetPlayerInformation(playerIdx)->properties[0].player_team;
}

int NetworkSession::GetPeerIndexFromId(unsigned long long xuid)
{
	if (GetPlayerCount() > 0)
	{
		for (int playerIdx = 0; playerIdx < ENGINE_MAX_PLAYERS; playerIdx++)
		{
			if (PlayerIsActive(playerIdx) && GetPlayerId(playerIdx) == xuid)
				return GetPeerIndex(playerIdx);
		}
	}
	return NONE;
}

void NetworkSession::KickPeer(int peerIdx)
{
	typedef void(__thiscall* game_session_boot_t)(s_network_session*, int, bool);
	game_session_boot_t p_game_session_boot = Memory::GetAddress<game_session_boot_t>(0x1CCE9B);

	if (peerIdx < GetPeerCount())
	{
		LOG_TRACE_GAME("{} - about to kick peer index = {}", __FUNCTION__, peerIdx);
		p_game_session_boot(NetworkSession::GetActiveNetworkSession(), peerIdx, true);
	}
}

void NetworkSession::EndGame()
{
	typedef void(__cdecl* end_game_t)();
	end_game_t p_end_game = Memory::GetAddress<end_game_t>(0x215470, 0x197F32);
	p_end_game();
}

s_session_observer_channel* NetworkSession::GetPeerObserverChannel(int peerIdx)
{
	return &GetActiveNetworkSession()->observer_channels[peerIdx];
}

wchar_t* NetworkSession::GetGameVariantName()
{
	return GetActiveNetworkSession()->parameters[0].game_variant.variant_name;
}

bool NetworkSession::IsVariantTeamPlay()
{
	return GetActiveNetworkSession()->parameters[0].game_variant.is_team_play();
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
		load_main_menu_with_context_t p_load_main_menu_with_context = Memory::GetAddress<load_main_menu_with_context_t>(0x08EAF);
		p_load_main_menu_with_context(0);
	}

	typedef int(__cdecl* leave_game_type_t)(int a1);
	leave_game_type_t p_leave_session = Memory::GetAddress<leave_game_type_t>(0x216388);
	p_leave_session(0);
}
