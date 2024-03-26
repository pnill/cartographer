#include "stdafx.h"
#include "NetworkSession.h"

#include "Blam/Engine/game/game.h"

// ### TODO Cleanup

bool NetworkSession::PlayerIsActive(datum player_index)
{
	return (NetworkSession::GetActiveNetworkSession()->membership[0].players_active_mask & FLAG(DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index))) != 0;
}

std::vector<uint64> NetworkSession::GetActivePlayerIdList()
{
	std::vector<uint64> activePlayerIdList;
	if (NetworkSession::GetPlayerCount() > 0)
	{
		for (int playerIdx = 0; playerIdx < k_maximum_players; playerIdx++)
		{
			if (NetworkSession::PlayerIsActive(playerIdx))
			{
				uint64 playerId = NetworkSession::GetPlayerId(playerIdx);
				activePlayerIdList.emplace_back(playerId);
			}
		}
	}

	return activePlayerIdList;
}

std::vector<int32> NetworkSession::GetActivePlayerIndicesList()
{
	std::vector<int32> activePlayersIndices;
	if (NetworkSession::GetPlayerCount() > 0)
	{
		for (int32 playerIndex = 0; playerIndex < k_maximum_players; playerIndex++)
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
int32 NetworkSession::GetPeerIndexFromNetworkAddress(network_address* address)
{
	typedef int(__thiscall* get_peer_index_from_network_address_t)(s_network_session* session, network_address* address);
	auto p_get_peer_index_from_network_address = Memory::GetAddress<get_peer_index_from_network_address_t>(0x1C71DF, 0x19E9CF);
	return p_get_peer_index_from_network_address(GetActiveNetworkSession(), address);
}

bool NetworkSession::GetMapFileLocation(wchar_t* buffer, size_t size)
{
	// host-only
	typedef bool(__thiscall* get_map_file_location_t)(s_network_session* session, wchar_t* buffer, size_t size);
	auto p_get_map_file_location = Memory::GetAddress<get_map_file_location_t>(0x1C5678, 0x19CD4A);
	return p_get_map_file_location(GetActiveNetworkSession(), buffer, size);
}

int32 NetworkSession::GetPeerCount()
{
	return GetActiveNetworkSession()->membership[0].peer_count;
}

int32 NetworkSession::GetLocalPeerIndex()
{
	return GetActiveNetworkSession()->local_peer_index;
}

bool NetworkSession::IsPeerIndexLocal(int32 peer_index)
{
	return GetLocalPeerIndex() == peer_index;
}

IN_ADDR NetworkSession::GetLocalNetworkAddress()
{
	return GetActiveNetworkSession()->membership[0].peers[GetLocalPeerIndex()].secure_address.inaOnline;
}

int32 NetworkSession::GetPeerIndex(datum player_index)
{
	return GetPlayerInformation(player_index)->peer_index;
}

int32 NetworkSession::GetPlayerCount()
{
	return GetActiveNetworkSession()->membership[0].player_count;
}

s_membership_player* NetworkSession::GetPlayerInformation(datum player_index)
{
	return &GetActiveNetworkSession()->membership[0].players[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];
}

const wchar_t* NetworkSession::GetPlayerName(datum player_index)
{
	return GetPlayerInformation(player_index)->properties[0].player_name;
}

uint64 NetworkSession::GetPlayerId(datum player_index)
{
	return GetPlayerInformation(player_index)->identifier;
}

int NetworkSession::GetPlayerTeam(datum player_index)
{
	return GetPlayerInformation(player_index)->properties[0].team_index;
}

int32 NetworkSession::GetPeerIndexFromId(uint64 xuid)
{
	if (GetPlayerCount() > 0)
	{
		for (int32 player_index = 0; player_index < k_maximum_players; player_index++)
		{
			if (PlayerIsActive(player_index) && GetPlayerId(player_index) == xuid)
				return GetPeerIndex(player_index);
		}
	}
	return NONE;
}

void NetworkSession::KickPeer(int32 peer_index)
{
	typedef void(__thiscall* game_session_boot_t)(s_network_session*, int, bool);
	auto p_game_session_boot = Memory::GetAddress<game_session_boot_t>(0x1CCE9B);

	if (peer_index < GetPeerCount())
	{
		LOG_TRACE_GAME("{} - about to kick peer index = {}", __FUNCTION__, peer_index);
		p_game_session_boot(NetworkSession::GetActiveNetworkSession(), peer_index, true);
	}
}

void NetworkSession::EndGame()
{
	typedef void(__cdecl* end_game_t)();
	auto p_end_game = Memory::GetAddress<end_game_t>(0x215470, 0x197F32);
	p_end_game();
}

s_session_observer_channel* NetworkSession::GetPeerObserverChannel(int32 peer_index)
{
	return &GetActiveNetworkSession()->observer_channels[peer_index];
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

	if (game_is_ui_shell())
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

s_membership_peer* network_session_get_peer(int32 peer_index)
{
	return &NetworkSession::GetActiveNetworkSession()->membership[0].peers[peer_index];
}

s_session_interface_globals* s_session_interface_globals::get()
{
	return Memory::GetAddress<s_session_interface_globals*>(0x51A590, 0x520408);
}

s_session_interface_user* network_session_interface_get_local_user_properties(int user_index)
{
	return &s_session_interface_globals::get()->users[user_index];
}

bool network_session_interface_set_local_user_character_type(int user_index, e_character_type character_type)
{
	s_session_interface_user* user_properties = network_session_interface_get_local_user_properties(user_index);
	
	// Don't change the character type if the user doesn't exist
	if (user_properties->user_exists)
	{
		user_properties->properties.profile_traits.profile.player_character_type = character_type;
		return true;
	}

	return false;
}

bool network_session_interface_get_local_user_identifier(int user_index, s_player_identifier* out_identifier)
{
	s_session_interface_user* user_properties = network_session_interface_get_local_user_properties(user_index);
	if(user_properties->user_exists)
	{
		*out_identifier = user_properties->network_user_identifier;
		return true;
	}
	return false;
}

void network_session_interface_set_local_user_rank(int32 user_index, int8 rank)
{
	s_session_interface_user* user_properties = network_session_interface_get_local_user_properties(user_index);
	user_properties->properties.player_displayed_skill = rank;
	user_properties->properties.player_overall_skill = rank;
	return;
}

bool __cdecl network_session_interface_get_local_user_properties_out(int32 user_index, int32* out_controller_index, s_player_properties* out_properties, int32* out_player_voice, int32* out_player_text_chat)
{
	return INVOKE(0x1B10E0, 0x1970A8, network_session_interface_get_local_user_properties_out, user_index, out_controller_index, out_properties, out_player_voice, out_player_text_chat);
}

void __cdecl network_session_init_session(int unk, char unk_2)
{
	INVOKE(0x1B54CF, 0x1A922D, network_session_init_session, unk, unk_2);
}
