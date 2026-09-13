#include "stdafx.h"
#include "CommandCollection.h"

#ifdef TERMINAL_ENABLED

#include "CommandHandler.h"
#include "CommandsUtil.h"
#include "ComVar.h"

#include "game/game.h"
#include "game/players.h"
#include "main/main.h"
#include "main/main_time.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/messages/network_messages_cartographer.h"
#include "networking/session/network_session.h"
#include "networking/session/network_observer.h"
#include "networking/network_event.h"
#include "shell/shell.h"
#include "text/unicode.h"

#include "H2MOD.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/Shell/Config.h"

// for XNet connection logging
#include "interface/user_interface_networking.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "XLive/xnet/IpManagement/XnIp.h"

static const char command_error_bad_arg[] = "# exception catch (bad arg): ";

namespace CommandCollection
{
	static ConsoleCommand* GetCommandByName(const std::string& name);
	static void SetVarCommandPtr(const std::string& name, ComVarBase* varPtr);

	// commands
	static int BoolVarHandlerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int SetAddressIpv4HandlerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx);

	static int RumbleScaleCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int NetworkMetricsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int HelpCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int LogPeersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int LogPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int LeaveNetworkSessionCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int IsSessionHostCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int DownloadMapCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int LogXNetConnectionsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int LogSelectedMapFilenameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int RequestFileNameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int ReloadMapsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int SetMaxPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int WarpFixCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int KickPeerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int InjectTagCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);

	static int invite(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int connect(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int change_player_team(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx);
	static int quit(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	static int SetAddressLANIpv4(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx);
	static int SetAddressBroadcastIpv4(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx);
	static int SetPortNumber(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx);

	TEST_N_DEF(CC5);
}

ComVarFromPtr(network_stats_overlay_var_cmd, int, &ImGuiHandler::g_network_stats_overlay,
	"var_net_metrics", "displays various network parameters, 1 parameter(s): <int>(1 - basic debug info, 2 - complete debug)", 0, 1, CommandCollection::NetworkMetricsCmd);

ComVarFromPtr(og_frame_limiter_var_cmd, bool, &g_main_game_time_frame_limiter_enabled,
	"var_og_frame_limiter", "enabled/disable original h2 frame limiter", 0, 1, CommandCollection::BoolVarHandlerCmd);

extern real32 g_rumble_factor;
ComVarFromPtr(rumble_var_cmd, real32, &g_rumble_factor,
	"var_rumble_scale", "change controller vibration strength (0.0 to 1.0), 1 parameter(s): <float>", 1, 1, CommandCollection::RumbleScaleCmd);

ComVarFromPtr(h2config_set_base_port, unsigned short, &H2Config_base_port,
	"var_network_port", "change the network port used to connect over the network", 1, 1, CommandCollection::SetPortNumber);

ComVarFromPtrIpv4(h2config_set_lan_ipv4_address, &H2Config_ip_lan,
	"var_lan_ip_address_override", "sets the LAN override address of the local machine", 1, 1, CommandCollection::SetAddressLANIpv4);

ComVarFromPtrIpv4(h2config_set_broadcast_ipv4_address, &H2Config_ip_broadcast_override,
	"var_broadcast_ip_address_override", "sets the broadcast override address", 1, 1, CommandCollection::SetAddressBroadcastIpv4);

// don't forget to add '_cmd' after the name, 
// if you add a variable command created using `DECL_ComVarCommandPtr` macro
std::vector<ConsoleCommand*> CommandCollection::commandTable;

CRITICAL_SECTION g_command_insert_section;

void CommandCollection::InitializeCommands()
{
	InitializeCriticalSection(&g_command_insert_section);

	static bool InitializeCommandsMap_initialized = false;
	if (InitializeCommandsMap_initialized) return;
	InitializeCommandsMap_initialized = true;

	TEST_N_DEF(CC2);
	InsertCommand(new ConsoleCommand(network_stats_overlay_var_cmd));
	InsertCommand(new ConsoleCommand(og_frame_limiter_var_cmd));
	InsertCommand(new ConsoleCommand(rumble_var_cmd));
	InsertCommand(new ConsoleCommand(h2config_set_lan_ipv4_address));
	InsertCommand(new ConsoleCommand(h2config_set_broadcast_ipv4_address));
	InsertCommand(new ConsoleCommand(h2config_set_base_port));
	InsertCommand(new ConsoleCommand("help", "outputs all commands, 0 - 1 parameter(s): <string>(optional): command name", 0, 1, CommandCollection::HelpCmd));
	InsertCommand(new ConsoleCommand("log_peers", "logs all peers to console, 0 parameter(s)", 0, 0, CommandCollection::LogPeersCmd));
	InsertCommand(new ConsoleCommand("log_players", "logs all players to console, 0 parameter(s)", 0, 0, CommandCollection::LogPlayersCmd));
	InsertCommand(new ConsoleCommand("kick_peer", "kicks peer from network session, 1 parameter(s): <int>: peer index", 1, 1, CommandCollection::KickPeerCmd));
	InsertCommand(new ConsoleCommand("leave_session", "leave current session, 0 parameter(s)", 0, 0, CommandCollection::LeaveNetworkSessionCmd));
	InsertCommand(new ConsoleCommand("is_host", "logs if you are session host or not, 0 parameter(s)", 0, 0, CommandCollection::IsSessionHostCmd));
	InsertCommand(new ConsoleCommand("map_download", "download specified map, 1 parameter(s): <string>", 1, 1, CommandCollection::DownloadMapCmd));
	InsertCommand(new ConsoleCommand("reload_maps", "re-load custom map data cache into memory, 0 parameter(s)", 0, 0, CommandCollection::ReloadMapsCmd));
	InsertCommand(new ConsoleCommand("log_map_file_name", "logs selected map filename, 0 parameter(s)", 0, 0, CommandCollection::LogSelectedMapFilenameCmd));
	InsertCommand(new ConsoleCommand("request_map_file", "requests map file name from host, 0 parameter(s)", 0, 0, CommandCollection::RequestFileNameCmd));
	InsertCommand(new ConsoleCommand("max_players", "set maximum players that can join, 1 parameter(s): <int>", 1, 1, CommandCollection::SetMaxPlayersCmd));
	InsertCommand(new ConsoleCommand("warp_fix", "(EXPERIMENTAL) increases client position update control threshold", 1, 1, CommandCollection::WarpFixCmd, CommandFlags_::CommandFlag_Hidden));
	InsertCommand(new ConsoleCommand("log_xnet_connections", "logs the xnet connections for debugging purposes, 0 parameter(s)", 0, 0, CommandCollection::LogXNetConnectionsCmd, CommandFlags_::CommandFlag_Hidden));
	InsertCommand(new ConsoleCommand("tag_inject", "injects tag into memory, 3 parameter(s): <string>: tag_name, tag_type, map_name", 3, 3, CommandCollection::InjectTagCmd, CommandFlags_::CommandFlag_Hidden));
	InsertCommand(new ConsoleCommand("invite", "creates a invite code that you can send to people for direct connecting", 0, 0, CommandCollection::invite));
	InsertCommand(new ConsoleCommand("connect", "lets you directly connect to a session with an invite code", 1, 1, CommandCollection::connect));
	InsertCommand(new ConsoleCommand("sv_change_player_team", "changes the player team to the specivied team", 2, 2, CommandCollection::change_player_team));
	InsertCommand(new ConsoleCommand("quit", "quits the game to desktop", 0, 0, CommandCollection::quit));


	atexit([]() -> void {
		for (auto command : commandTable)
		{
			delete command;
		}

		commandTable.clear();
	});

	DeleteCriticalSection(&g_command_insert_section);
	return;
}

void CommandCollection::InsertCommand(ConsoleCommand* newCommand)
{
	EnterCriticalSection(&g_command_insert_section);

	for (auto command : commandTable)
	{
		if (!strcmp(newCommand->GetName(), command->GetName()))
		{
			event(_event_error, "h2mod:commands: %s - command %s already present!", __FUNCTION__, newCommand->GetName());
			LeaveCriticalSection(&g_command_insert_section);
			return;
		}
	}

	commandTable.emplace_back(newCommand);

	LeaveCriticalSection(&g_command_insert_section);
	return;
}

static ConsoleCommand* CommandCollection::GetCommandByName(const std::string& name)
{
	EnterCriticalSection(&g_command_insert_section);

	ConsoleCommand* result = nullptr;
	for (auto command : commandTable)
	{
		if (!strcmp(name.c_str(), command->GetName()))
		{
			result = command;
			break;
		}
	}

	LeaveCriticalSection(&g_command_insert_section);
	return result;
}

// in case your variable needs to be set/updated
static void CommandCollection::SetVarCommandPtr(const std::string& name, ComVarBase* varPtr)
{
	ConsoleCommand* commandPtr = GetCommandByName(name);
	if (commandPtr != nullptr)
	{
		commandPtr->SetCommandVarPtr(varPtr);
	}
}

//////////////////////////////////////////////////////////////////////////
//	commands
//////////////////////////////////////////////////////////////////////////

static int CommandCollection::BoolVarHandlerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;
	auto booleanCmdVar = ctx.consoleCommand->GetVar<ComVar<bool>>();

	if (!ctx.consoleCommand->SetsVariable())
	{
		outputCb(StringFlag_None, "# command does not set a variable");
		return 0;
	}

	if (tokens.size() - 1 < 1)
	{
		booleanCmdVar->SetVal(!booleanCmdVar->GetVal());
		outputCb(StringFlag_None, booleanCmdVar->AsString().c_str());
		return 0;
	}

	std::string exception;
	if (!booleanCmdVar->SetFromStr(tokens[1], exception))
	{
		outputCb(StringFlag_None, command_error_bad_arg);
		outputCb(StringFlag_None, "\t%s", exception.c_str());
	}
	return 0;
}

static int CommandCollection::SetAddressIpv4HandlerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	auto address = ctx.consoleCommand->GetVar<ComVarAddrIpv4>();
	int result = -1;

	std::string exception;
	if (address->SetFromStr(tokens[1], exception))
	{
		result = 0;
	}
	else
	{
		outputCb(StringFlag_None, command_error_bad_arg);
		outputCb(StringFlag_None, "\t%s", exception.c_str());
	}

	return result;
}

static int CommandCollection::RumbleScaleCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;
	real32 rumbleScale;
	std::string exception;

	if (!ComVar(&rumbleScale).SetFromStr(tokens[1], exception))
	{
		outputCb(StringFlag_None, command_error_bad_arg);
		outputCb(StringFlag_None, "\t%s", exception.c_str());
	}
	else
	{
		g_rumble_factor = PIN(rumbleScale, 0.f, 1.f);
	}

	return 0;
}

static int CommandCollection::NetworkMetricsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;
	int32 network_debug_display_type;
	std::string exception;

	if (shell_is_dedicated_server()) {
		outputCb(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	if (tokens.size() - 1 < 1)
	{
		ImGuiHandler::g_network_stats_overlay = !ImGuiHandler::g_network_stats_overlay;
		return 0;
	}
	else if (!ComVar(&network_debug_display_type).SetFromStr(tokens[1], 0, exception))
	{
		outputCb(StringFlag_None, command_error_bad_arg);
		outputCb(StringFlag_None, "\t%s", exception.c_str());
	}
	else
	{
		switch (network_debug_display_type)
		{
		case _network_stats_display_basic:
		case _network_stats_display_complete:
			ImGuiHandler::g_network_stats_overlay = network_debug_display_type;
			break;
		case _network_stats_display_none:
		default:
			ImGuiHandler::g_network_stats_overlay = _network_stats_display_none;
			break;
		}
	}

	return 0;
}

static int CommandCollection::LogXNetConnectionsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;
	gXnIpMgr.LogConnectionsToConsole(outputCb);
	return 0;
}

static int CommandCollection::LogSelectedMapFilenameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (!network_life_cycle_in_squad_session(NULL))
	{
		outputCb(StringFlag_None, "# not in a network session");
		return 0;
	}

	std::wstring mapFilenameWide;
	MapManager::GetMapFilename(mapFilenameWide);

	utf8 map_name[k_maximum_filename_length];
	wchar_string_to_utf8_string(mapFilenameWide.c_str(), map_name, NUMBEROF(map_name));
	outputCb(StringFlag_None, "# map file name: %s", map_name);
	return 0;
}

static int CommandCollection::RequestFileNameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (!network_life_cycle_in_squad_session(NULL))
	{
		outputCb(StringFlag_None, "# not in a network session");
		return 0;
	}
	else if (NetworkSession::LocalPeerIsSessionHost())
	{
		outputCb(StringFlag_None, "# only clients can request map file name");
		return 0;
	}

	network_message_cartographer_send_request_map_filename(NONE);
	return 0;
}

static int CommandCollection::LeaveNetworkSessionCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (!network_life_cycle_in_squad_session(NULL))
	{
		outputCb(StringFlag_None, "# not in a network session");
		return 0;
	}
	else if (shell_is_dedicated_server()) {
		outputCb(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	NetworkSession::LeaveSession();
	return 0;
}

static int CommandCollection::IsSessionHostCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	c_network_session* session = NULL;
	if (!network_life_cycle_in_squad_session(&session))
	{
		outputCb(StringFlag_None, "# not in a network session");
		return 0;
	}

	std::string isHostStr;
	isHostStr += "# Session host: ";
	isHostStr += (session->is_host() ? "yes" : "no");
	isHostStr += ", value=" + std::to_string(session->m_local_state);
	outputCb(StringFlag_None, isHostStr.c_str());
	return 0;
}

static int CommandCollection::KickPeerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;
	int32 peer_index;
	std::string exception;

	do
	{
		if (!ComVar(&peer_index).SetFromStr(tokens[1], 0, exception))
		{
			outputCb(StringFlag_None, command_error_bad_arg);
			outputCb(StringFlag_None, "\t%s", exception.c_str());
			break;
		}
		else if (shell_is_dedicated_server()) {
			outputCb(StringFlag_None, "# command unavailable on dedicated servers");
			break;
		}
		else if (!NetworkSession::LocalPeerIsSessionHost()) {
			outputCb(StringFlag_None, "# only the host can kick players");
			break;
		}
		else if (NetworkSession::GetLocalPeerIndex() == peer_index) {
			outputCb(StringFlag_None, "# don't kick yourself");
			break;
		}
		else if (peer_index >= NetworkSession::GetPeerCount()) {
			outputCb(StringFlag_None, "# peer at the specified index doesn't exist");
			break;
		}

		NetworkSession::KickPeer(peer_index);
	} while (0);

	return 0;
}

static int CommandCollection::DownloadMapCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (!NetworkSession::LocalPeerIsSessionHost())
	{
		outputCb(StringFlag_None, "# cannot download map using command while not being the session host!");
		return 0;
	}

	std::wstring fileNameToDownload(std::wstring(tokens[1].begin(), tokens[1].end()));
	auto downloadQuery = mapManager->AddDownloadQuery(fileNameToDownload.c_str());
	downloadQuery->StartMapDownload(); // since we have the map name, start the download
	return 0;
}

static int CommandCollection::ReloadMapsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	mapManager->ReloadAllMaps();
	return 0;
}

static int CommandCollection::HelpCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	const std::string* commandToHelp = nullptr;

	int parameterCount = tokens.size() - 1; // only parameters
	bool singleCommandHelp = parameterCount >= 1;
	bool singleCommandHelpFound = false;

	if (singleCommandHelp)
		commandToHelp = &tokens[1];

	if (!singleCommandHelp)
		outputCb(StringFlag_None, "# available commands: ");

	for (auto command_entry : CommandCollection::commandTable)
	{
		if (singleCommandHelp && _strnicmp(command_entry->GetName(), commandToHelp->c_str(), commandToHelp->length()) != 0)
			continue;

		if (!command_entry->Hidden())
		{
			outputCb(StringFlag_None, "# %s ", command_entry->GetName());
			if (command_entry->GetDescription() != NULL)
			{
				outputCb(StringFlag_None, "    # command description: %s", command_entry->GetDescription());
			}
			else
			{
				outputCb(StringFlag_None, "	# command has no description");
			}

			if (singleCommandHelp)
			{
				singleCommandHelpFound = true;
				break;
			}
		}
	}

	if (singleCommandHelp && !singleCommandHelpFound)
		outputCb(StringFlag_None, "	# unknown command: %s", commandToHelp->c_str());

	return 0;
}

static int CommandCollection::LogPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	c_network_session* session = NULL;
	if (!network_life_cycle_in_squad_session(&session))
	{
		outputCb(StringFlag_None, "# not in a network session");
		return 0;
	}
#ifdef CC3
	TEST_N_DEF(CC3)
#else
	else if (!session->is_host())
#endif
	{
		outputCb(StringFlag_None, "# must be network session host");
		return 0;
	}

	outputCb(StringFlag_None, "# %i players: ", session->get_player_count());

	for (int32 player_index = 0; player_index < k_maximum_players; player_index++)
	{
		if (session->is_session_player_active(player_index))
		{
			char player_name[XUSER_NAME_SIZE * 2];
			wchar_string_to_utf8_string(session->get_player_name(player_index), player_name, NUMBEROF(player_name));
			
			c_static_string<512> string;
			string.set("# Player index=");
			string.append(std::to_string(player_index).c_str());

			string.append(", Peer index=");
			string.append(std::to_string(NetworkSession::GetPeerIndex(player_index)).c_str());
			
			string.append(", PlayerName=");
			string.append(player_name);

			const player_datum* player = (player_datum*)datum_get_absolute(player_data_get(), player_index);
			wchar_string_to_utf8_string(player->configuration.name, player_name, NUMBEROF(player_name));

			string.append(", Name from game player state=");
			string.append(player_name);

			string.append(", Team=");
			string.append(std::to_string(NetworkSession::GetPlayerTeam(player_index)).c_str());
			
			string.append(", Identifier=");

			int64 id;
			csmemcpy(&id, NetworkSession::GetPlayerId(player_index).identifier, sizeof(id));

			string.append_print(", Identifier=%llX", id);

			outputCb(StringFlag_None, string.get_string());
		}
	}

	return 0;
}

static int CommandCollection::LogPeersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	c_network_session* session = NULL;
	if (!network_life_cycle_in_squad_session(&session))
	{
		outputCb(StringFlag_None, "# not in a network session");
		return 0;
	}
#ifdef CC3
	TEST_N_DEF(CC3)
#else
	else if (!session->is_host())
#endif
	{
		outputCb(StringFlag_None, "# must be network session host");
		return 0;
	}

	c_network_observer* observer = session->m_network_observer;

	outputCb(StringFlag_None, "# %i peers: ", session->get_peer_count());

	for (int32 peer_index = 0; peer_index < session->get_peer_count(); peer_index++)
	{
		const s_observer_channel* peer_observer_channel = &observer->m_observer_channels[session->m_session_peers[peer_index].observer_channel_index];

		char name[XUSER_NAME_SIZE * 2];
		wchar_string_to_utf8_string(session->m_session_membership.peers[peer_index].name, name, NUMBEROF(name));

		c_static_string<512> string;
		string.set("# Peer index=");
		string.append(std::to_string(peer_index).c_str());
		string.append(", Peer Name=");
		string.append(name);
		string.append(", Connection Status=");
		string.append(std::to_string(peer_observer_channel->state).c_str());
		string.append(", Peer map state: ");
		string.append(std::to_string(session->m_session_membership.peers[peer_index].map_status).c_str());
		
		const datum player_index = session->m_session_membership.peers[peer_index].local_players_indexes[0];
		player_datum const* player = player_get(player_index);
		
		if (player)
		{
			wchar_string_to_utf8_string(session->get_player_membership(player_index)->configuration.name, name, NUMBEROF(name));

			string.append(", Player index=");
			string.append(std::to_string(player_index).c_str());
			string.append(", Player name=");
			string.append(name);

			wchar_string_to_utf8_string(player->configuration.name, name, NUMBEROF(name));
			string.append(", Name from game player state=");
			string.append(name);
		}
		outputCb(StringFlag_None, string.get_string());
	}

	return 0;
}

static int CommandCollection::SetMaxPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	int max_players;
	std::string exception;

	do
	{
		c_network_session* session = NULL;
		if (!network_life_cycle_in_squad_session(&session))
		{
			outputCb(StringFlag_None, "# not in a network session");
			break;
		}
		else if (!session->is_host()) {
			outputCb(StringFlag_None, "# can be only used by host");
			break;
		}
		else if (!ComVar(&max_players).SetFromStr(tokens[1], 0, exception))
		{
			outputCb(StringFlag_None, command_error_bad_arg);
			outputCb(StringFlag_None, "\t%s", exception.c_str());
			break;
		}
		else if (max_players < 1 || max_players > 16) {
			outputCb(StringFlag_None, "# the value needs to be between 1 and 16");
			break;
		}
		else if (max_players < session->get_player_count()) {
			outputCb(StringFlag_None, "# you can't set a value of max players smaller than the actual number of players on the server");
			break;
		}

		session->m_session_parameters.max_party_players = max_players;
		outputCb(StringFlag_None, "# maximum players set: %i", max_players);
	} while (0);

	return 0;
}

static int CommandCollection::WarpFixCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;
	bool warpFixVar;

	if (shell_is_dedicated_server()) {
		outputCb(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	std::string exception;
	if (!ComVar(&warpFixVar).SetFromStr(tokens[1], exception))
	{
		outputCb(StringFlag_None, command_error_bad_arg);
		outputCb(StringFlag_None, "\t%s", exception.c_str());
		return 0;
	}

	H2MOD::player_position_increase_client_position_margin_of_error(warpFixVar);
	return 0;
}

static int CommandCollection::InjectTagCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (!NetworkSession::LocalPeerIsSessionHost()
		&& !game_is_campaign())
	{
		outputCb(StringFlag_None, "# can only be used by the session host");
		return 0;
	}

	std::string tagName = tokens[1];
	std::wstring mapName(tokens[3].begin(), tokens[3].end());

	const char* p_string = tokens[2].c_str();

	tag_group tag_type = p_string[0] << 24 | p_string[1] << 16 | p_string[2] << 8 | p_string[3];

	tag_injection_set_active_map(mapName.c_str());
	int32 tag_datum = tag_injection_load(tag_type, tagName.c_str(), true);
	tag_injection_inject();

	outputCb(StringFlag_None, "# loaded tag datum: %#X", tag_datum);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//	commands end
//////////////////////////////////////////////////////////////////////////

static int CommandCollection::invite(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;
	c_network_session* session = NULL;
	int result = 0;
	if (network_life_cycle_in_squad_session(&session))
	{
		bool session_host = session->is_host();

		XSESSION_INFO x_session_info;
		csmemcpy(&x_session_info.sessionID, &session->m_session_id, sizeof(x_session_info.sessionID));
		x_session_info.keyExchangeKey = session->m_session_key;
		x_session_info.hostAddress = (session_host ? session->m_session_virtual_couch.xsession_info.hostAddress :
			session->m_network_observer->m_observer_channels[session->get_session_peer(session->m_session_host_peer_index)->observer_channel_index].xnaddr);

		uint8* session_bytes = (uint8*)&x_session_info;
		char connect_string[sizeof(XSESSION_INFO) * 2 + 1];

		// Encode the data into hex string
		for (uint32 i = 0; i < sizeof(XSESSION_INFO); i++)
		{
			csprintf(&connect_string[2 * i], 3, "%02hhX", session_bytes[i]);
		}

		outputCb(StringFlag_None, "Invite code generated:");
		outputCb(StringFlag_CopyToClipboard, connect_string);
		outputCb(StringFlag_None, "Invite code has been copied to your clipboard.");
	}
	else
	{
		result = 1;
		outputCb(StringFlag_None, "Invite code failed to generate. Not in a squad session");
	}
	return result;
}

static int CommandCollection::connect(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	XSESSION_INFO session;
	uint8* session_bytes = (uint8*)&session;

	// Decode the data from hex string
	for (uint32 i = 0; i < sizeof(XSESSION_INFO); i++)
	{
		(void)sscanf_s(&tokens[1].c_str()[2 * i], "%02hhX", &session_bytes[i]);
	}

	user_interface_networking_join_game_direct(session.sessionID, session.keyExchangeKey, &session.hostAddress, EXECUTABLE_TYPE, EXECUTABLE_VERSION, COMPATIBLE_VERSION);

	return 0;
}

static int CommandCollection::change_player_team(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	int32 player_index = NONE;
	int32 new_team_index = NONE;

	if (ComVar(&player_index).SetFromStr(tokens[1])
		&& ComVar(&new_team_index).SetFromStr(tokens[2]))
	{
		c_network_session* session = NULL;
		if (!network_life_cycle_in_squad_session(&session))
		{
			outputCb(StringFlag_None, "# not in a network session");
			return 0;
		}

		session->switch_player_team(player_index, (e_game_team)new_team_index);
	}

	return 0;
}


static int CommandCollection::quit(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	main_quit();
	return 0;
}

static int CommandCollection::SetAddressLANIpv4(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (gXnIpMgr.GetLocalUserXn()->m_valid)
	{
		outputCb(StringFlag_None, "# set the LAN address override before LOGIN, during the \"PRESS ANY KEY\" dialog, when signed-out!");
		return -1;
	}

	if (network_life_cycle_in_squad_session(NULL))
	{
		outputCb(StringFlag_None, "# LAN address override cannot be updated during a game session!");
		return -1;
	}

	return SetAddressIpv4HandlerCmd(tokens, ctx);
}

static int CommandCollection::SetAddressBroadcastIpv4(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (network_life_cycle_in_squad_session(NULL))
	{
		outputCb(StringFlag_None, "# broadcast address override cannot be updated during a game session!");
		return -1;
	}

	return SetAddressIpv4HandlerCmd(tokens, ctx);
}

static int CommandCollection::SetPortNumber(const std::vector<std::string>& tokens, ConsoleCommandCtxData ctx)
{
	TextOutputCb* outputCb = ctx.outputCb;

	if (gXnIpMgr.GetLocalUserXn()->m_valid)
	{
		outputCb(StringFlag_None, "# set the port number before LOGIN, during the \"PRESS ANY KEY\" dialog, when signed-out!");
		return -1;
	}

	if (network_life_cycle_in_squad_session(NULL))
	{
		outputCb(StringFlag_None, "# port number configuration cannot be updated during a game session!");
		return -1;
	}

	const unsigned short port_min = 2000;
	const unsigned short port_max = 65534 - 34;

	unsigned short port;
	if (ComVar(&port).SetFromStr(tokens[1])
		&& (port >= port_min && port <= port_max))
	{
		H2Config_base_port = port;
	}
	else
	{
		outputCb(StringFlag_None, "# invalid port number, a number between %u and %u is expected", port_min, port_max);
	}

	return 0;
}

TEST_N_DEF(CC4);

#endif
