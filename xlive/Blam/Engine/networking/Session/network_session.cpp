#include "stdafx.h"
#include "network_session.h"

#include "network_observer.h"

#include "game/game.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "interface/user_interface_controller.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/network_event.h"
#include "networking/network_time.h"
#include "networking/messages/network_message_handler.h"
#include "shell/shell.h"

/* constants */

const char* const k_network_protocols_text[] =
{
	"<disconnected>",
	"System-Link",
	"LIVE",
};

/* gloabls */

static char g_network_session_peer_description[2][35] = {};
static int32 g_network_session_peer_description_index = 0;

c_network_session_cartographer g_cartographer_network_session;


CLASS_HOOK_DECLARE_LABEL(c_network_session__update_countdown_timer_internal, c_network_session::update_countdown_timer_internal);
__declspec(naked) void update_countdown_timer_internal_hook()
{
	CLASS_HOOK_JMP(c_network_session__update_countdown_timer_internal, c_network_session::update_countdown_timer_internal);
}

/* public code */

/* PRIVATE NAMESPACE, acts like static keyword */
// 
namespace NetworkSession
{
	c_network_session* GetNetworkSessions();
	c_network_session* GetActiveNetworkSession();
}

bool NetworkSession::PlayerIsActive(datum player_index)
{
	return GetActiveNetworkSession()->is_session_player_active(player_index);
}

std::vector<int32> NetworkSession::GetActivePlayerIndicesList()
{
	std::vector<int32> activePlayersIndices;
	if (GetPlayerCount() > 0)
	{
		for (int32 playerIndex = 0; playerIndex < k_maximum_players; playerIndex++)
		{
			if (PlayerIsActive(playerIndex))
				activePlayersIndices.emplace_back(playerIndex);
		}
	}

	return activePlayersIndices;
}

c_network_session* NetworkSession::GetNetworkSessions()
{
	return *Memory::GetAddress<c_network_session**>(0x51C474, 0x520B94);
}

c_network_session* NetworkSession::GetActiveNetworkSession()
{
	// ### FIXME replace this function wherever this is still used
	// for now, return the pointer to the active squad session regardless to avoid crashing the game
	/*c_network_session* session = NULL;
	network_life_cycle_in_squad_session(&session);*/
	return c_game_life_cycle_manager::get()->m_active_squad_session;
}

bool NetworkSession::LocalPeerIsSessionHost()
{
	return GetActiveNetworkSession()->is_host();
}

bool NetworkSession::LocalPeerIsEstablished()
{
	return GetActiveNetworkSession()->established();
}

bool NetworkSession::GetMapFileLocation(wchar_t* buffer, size_t size)
{
	// host-only
	typedef bool(__thiscall* get_map_file_location_t)(c_network_session* session, wchar_t* buffer, size_t size);
	auto p_get_map_file_location = Memory::GetAddress<get_map_file_location_t>(0x1C5678, 0x19CD4A);
	return p_get_map_file_location(GetActiveNetworkSession(), buffer, size);
}

int32 NetworkSession::GetPeerCount()
{
	return GetActiveNetworkSession()->m_session_membership.peer_count;
}

int32 NetworkSession::GetLocalPeerIndex()
{
	return GetActiveNetworkSession()->get_local_peer_index();
}

int32 NetworkSession::GetPeerIndex(datum player_index)
{
	return GetActiveNetworkSession()->get_player_membership(player_index)->peer_index;
}

int32 NetworkSession::GetPlayerCount()
{
	return GetActiveNetworkSession()->get_player_count();
}

const wchar_t* NetworkSession::GetPlayerName(datum player_index)
{
	return GetActiveNetworkSession()->get_player_name(player_index);
}

s_player_identifier NetworkSession::GetPlayerId(datum player_index)
{
	return GetActiveNetworkSession()->get_player_id(player_index);
}

int8 NetworkSession::GetPlayerTeam(datum player_index)
{
	return GetActiveNetworkSession()->get_player_membership(player_index)->configuration.team_index;
}

void NetworkSession::KickPeer(int32 peer_index)
{
	typedef void(__thiscall* game_session_boot_t)(c_network_session*, int, bool);
	auto p_game_session_boot = Memory::GetAddress<game_session_boot_t>(0x1CCE9B);

	if (peer_index < GetPeerCount())
	{
		event(_event_message, "h2mod:network_session: %s - about to kick peer index = %d", __FUNCTION__, peer_index);
		p_game_session_boot(NetworkSession::GetActiveNetworkSession(), peer_index, true);
	}
}

void NetworkSession::EndGame()
{
	INVOKE(0x215470, 0x197F32, NetworkSession::EndGame);
}

wchar_t* NetworkSession::GetGameVariantName()
{
	return GetActiveNetworkSession()->m_session_parameters.game_variant.variant_name;
}

void NetworkSession::LeaveSession()
{
	if (shell_is_dedicated_server())
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


void network_session_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x1C9DBC, 0x1A1344), update_countdown_timer_internal_hook);
	return;
}

void __cdecl network_globals_switch_environment(int32 a1, bool a2)
{
	INVOKE(0x1B54CF, 0x1A922D, network_globals_switch_environment, a1, a2);
}

bool network_squad_session_can_set_game_settings(void)
{
	bool result = false;
	c_network_session* session = NULL;

	if (
		network_life_cycle_in_squad_session(&session) &&
		session->established() &&
		session->is_local_peer_session_leader() &&
		session->session_mode() == _network_session_mode_idle)
	{
		result = true;
	}

	return result;
}

void network_session_membership_update_local_players_teams()
{
	c_network_session* session = NULL;
	if (network_life_cycle_in_squad_session(&session))
	{
		if (session->established() || session->peer_joining())
		{
			int32 local_peer_index = session->get_local_peer_index();

			if (session->get_session_parameters()->game_variant.game_engine_flags.test(_game_engine_teams_bit))
			{
				for (int32 i = 0; i < k_number_of_users; i++)
				{
					datum player_index = session->get_peer_membership(local_peer_index)->local_players_indexes[i];
					if (player_index != NONE)
					{
						const s_network_session_player* membership_player = session->get_player_membership(player_index);
						ASSERT(session->is_session_player_active(player_index));
						ASSERT(membership_player->controller_index != NONE);
						user_interface_controller_set_desired_team_index((e_controller_index)membership_player->controller_index, (e_game_team)membership_player->configuration.team_index);
						user_interface_controller_update_network_properties((e_controller_index)membership_player->controller_index);
					}
				}
			}
		}
	}
}

void network_session_set_player_team(datum player_index, e_game_team team)
{
	c_network_session* session = NULL;
	if (network_life_cycle_in_squad_session(&session))
	{
		if (session->established() 
			&& session->is_host())
		{
			s_network_session_player* membership_player = session->get_player_membership(player_index);
			membership_player->configuration.team_index = (int8)team;

			if (session->peer_index_local_peer(membership_player->peer_index))
			{
				user_interface_controller_set_desired_team_index((e_controller_index)membership_player->controller_index, (e_game_team)membership_player->configuration.team_index);
				user_interface_controller_update_network_properties((e_controller_index)membership_player->controller_index);
			}
		}
	}
}

bool c_network_session::initialize_session(
	int32 session_index,
	e_network_session_type session_type,
	int32 session_transport_index,
	c_network_message_gateway* message_gateway,
	c_network_observer* observer,
	c_network_session_manager* session_manager,
	c_network_text_chat_manager* text_chat_manager)
{
	return INVOKE_TYPE(0x1C1E65, 0x199578, bool(__thiscall*)(c_network_session*, int32, e_network_session_type, int32, c_network_message_gateway*, c_network_observer*, c_network_session_manager*, c_network_text_chat_manager*),
		this,
		session_index,
		session_type,
		session_transport_index,
		message_gateway,
		observer,
		session_manager,
		text_chat_manager);
}

s_session_membership const* c_network_session::get_session_membership(
	int32* out_local_peer_index,
	int32* out_session_host_peer_index) const
{
	ASSERT(established());
	ASSERT(m_session_membership.update_number!=NONE);
	ASSERT(m_local_peer_index>=0 && m_local_peer_index<m_session_membership.peer_count);
	ASSERT(m_session_host_peer_index>=0 && m_session_host_peer_index<m_session_membership.peer_count);

	if (out_local_peer_index)
	{
		*out_local_peer_index = m_local_peer_index;
	}

	if (out_session_host_peer_index)
	{
		*out_session_host_peer_index = m_session_host_peer_index;
	}
	
	return &m_session_membership;
}

s_session_membership const* c_network_session::get_session_membership_unsafe(
	int32* out_local_peer_index,
	int32* out_session_host_peer_index) const
{
	int32 local_peer_index = NONE;
	int32 session_host_peer_index = NONE;
	const s_session_membership* membership = NULL;

	if (current_local_state() && m_session_membership.update_number != NONE)
	{
		ASSERT(m_local_peer_index>=0 && m_local_peer_index<m_session_membership.peer_count);
		ASSERT(m_session_host_peer_index>=0 && m_session_host_peer_index<m_session_membership.peer_count);

		local_peer_index = m_local_peer_index;
		session_host_peer_index = m_session_host_peer_index;
		membership = &m_session_membership;
	}

	if (out_local_peer_index)
	{
		*out_local_peer_index = local_peer_index;
	}

	if (out_session_host_peer_index)
	{
		*out_session_host_peer_index = session_host_peer_index;
	}

	return membership;
}

int32 c_network_session::get_session_membership_update_number(void) const
{
	ASSERT(established());
	ASSERT(m_session_membership.update_number!=NONE);

	return m_session_membership.update_number;
}

int32 c_network_session::get_local_session_membership_update_number(void) const
{
	return m_local_membership_update_number;
}

bool c_network_session::channel_is_authoritative(
	int32 network_channel_index) const
{
	bool result = false;

	if ((established() || peer_joining()) && !is_host())
	{
		int32 observer_index = m_network_observer->observer_channel_find_by_network_channel(m_session_index, network_channel_index);
		int32 peer_index = get_peer_index_by_observer_index(observer_index);

		if (peer_index != NONE && is_peer_session_host(peer_index))
		{
			result = true;
		}
	}

	return result;
}

void c_network_session::switch_players_to_teams(datum* player_indexes, int32 player_count, e_game_team* team_indexes)
{
	if (is_host())
	{
		for (int32 i = 0; i < player_count; i++)
		{
			s_network_session_player* player_membership = get_player_membership(player_indexes[i]);
			player_membership->configuration.team_index = (int8)team_indexes[i];
		}
		request_membership_update();
		network_session_membership_update_local_players_teams();
	}
}

bool c_network_session::get_secure_key(s_transport_secure_identifier* out_session_id, XNKEY* out_session_key, int32* out_session_key_index, e_transport_platform* transport_platform) const
{
	bool result = false;

	if (!disconnected() && m_field_48)
	{
		if (out_session_id != NULL)
		{
			*out_session_id = m_session_id;
		}

		if (out_session_key != NULL)
		{
			*out_session_key = m_session_key;
		}

		if (out_session_key_index != NULL)
		{
			*out_session_key_index = m_session_transport_index;
		}

		if (transport_platform != NULL)
		{
			*transport_platform = m_session_transport_platform;
		}

		result = true;
	}

	return result;
}

bool c_network_session::get_transport_session_id(s_transport_secure_identifier* out_session_id) const
{
	return get_secure_key(out_session_id, NULL, NULL, NULL);
}

uint32 c_network_session::time_get(void) const
{
	ASSERT(m_time_exists);
	return m_time + network_time_get_exact();
}

bool c_network_session::leader_request_delegate_leadership(
	s_transport_secure_address const* boot_peer_address)
{
	return INVOKE_TYPE(0x1C8693, 0x0, bool(__thiscall*)(c_network_session*, s_transport_secure_address const*), this, boot_peer_address);
}

bool c_network_session::leader_request_boot_machine(
	s_transport_secure_address const* boot_peer_address)
{
	return INVOKE_TYPE(0x1CDF84, 0x0, bool(__thiscall*)(c_network_session*, s_transport_secure_address const*), this, boot_peer_address);
}

bool c_network_session::handle_leave_request(const transport_address* incoming_address)
{
	const int32 peer = get_peer_from_incoming_address(incoming_address);
	ASSERT(is_host());

	bool result;
	if (peer == NONE || peer == m_local_peer_index)
	{
		event(
			_event_warning,
			"networking:session:membership: [%s] leave-request received from an incorrect peer [%s] (invalid or local)",
			managed_session_get_id_string(&m_session_id),
			get_peer_description(peer)
		);
		result = false;
	}
	else
	{
		event(
			_event_message,
			"session:membership: [%s] leave-request received from peer [%s]",
			managed_session_get_id_string(&m_session_id),
			get_peer_description(peer)
		);
		result = handle_leave_internal(peer);
	}

	return result;
}

bool c_network_session::handle_leave_internal(int32 peer_index)
{
	return INVOKE_TYPE(0x1CC7B4, 0x1A3D34, bool(__thiscall*)(c_network_session*, int32), this, peer_index);
}

s_session_parameters const* c_network_session::get_session_parameters(
	void) const
{
	return &m_session_parameters;
}

s_session_parameters* c_network_session::get_session_parameters(
	void)
{
	return &m_session_parameters;
}

bool c_network_session::parameters_game_variant_request_change(
	s_game_variant* variant)
{
	return INVOKE_TYPE(0x1C903B, 0x1A06FB, bool(__thiscall*)(c_network_session*, s_game_variant*), this, variant);
}

bool c_network_session::parameters_simulation_protocol_request_change(
	e_network_game_simulation_protocol protocol)
{
	return INVOKE_TYPE(0x1C8F03, 0x1A05C3, bool(__thiscall*)(c_network_session*, e_network_game_simulation_protocol), this, protocol);
}

bool c_network_session::parameters_countdown_timer_request_change(
	int8 mode,
	int32 countdown_timer, 
	int8 dedicated_server_host,
	uint32 delay_reason,
	s_player_identifier* responsible_player)
{
	return INVOKE_TYPE(0x1C96FB, 0x1A0C83, bool(__thiscall*)(c_network_session*, int8, int32, int8, uint32, s_player_identifier*), this, mode, countdown_timer, dedicated_server_host, delay_reason, responsible_player);
}

/* private code */

const char* c_network_session::get_peer_description(int32 peer_index) const
{
	char* result = g_network_session_peer_description[g_network_session_peer_description_index];
	g_network_session_peer_description_index = (g_network_session_peer_description_index + 1) % 2;

	if (established() && VALID_INDEX(peer_index, m_session_membership.peer_count) && m_session_membership.peers[peer_index].description[0] != '\0')
	{
		const char* mac_string = transport_secure_address_get_mac_string(&m_session_membership.peers[peer_index].secure_address);
		csprintf(result, ARRAYSIZE(g_network_session_peer_description[0]), "#%02d:%S:%s", peer_index, m_session_membership.peers[peer_index].description[0], mac_string);
	}
	else
	{
		csprintf(result, ARRAYSIZE(g_network_session_peer_description[0]), "#%02d", peer_index);
	}
	return result;
}

int32 c_network_session::get_peer_from_incoming_address(const transport_address* incoming_address) const
{
	ASSERT(incoming_address);

	int32 result = NONE;
	if (!disconnected() && m_field_48)
	{
		s_transport_secure_address secure_address;
		if (transport_secure_identifier_retrieve(incoming_address, m_session_transport_platform, NULL, NULL, NULL, &secure_address))
		{
			result = get_peer_from_secure_address(&secure_address);
		}
	}
	return result;
}

int32 c_network_session::get_peer_from_secure_address(const s_transport_secure_address* secure_address) const
{
	ASSERT(secure_address);
	
	int32 result = NONE;
	if (!disconnected() && has_membership())
	{
		for (int32 i = 0; i < m_session_membership.peer_count; ++i)
		{
			if (transport_secure_address_compare(secure_address, &m_session_membership.peers[i].secure_address))
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

void c_network_session::update_countdown_timer_internal(uint32 peer_index, bool countdown_running,
	int32 countdown_timer, bool current_host, int32 countdown_delay_reason, s_player_identifier* responsible_address)
{
	if (H2Config_xDelay)
	{
		INVOKE_TYPE(0x1C9D3F, 0x19E043, void(__thiscall*)(c_network_session*, uint32, bool, int32, bool, int32, s_player_identifier*),
			this, peer_index, countdown_running, countdown_timer, current_host, countdown_delay_reason, responsible_address);
	}
}
