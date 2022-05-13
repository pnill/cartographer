#include "stdafx.h"

#include "ImGui_ConsoleImpl.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\MapManager\MapManager.h"
#include "Blam\Engine\Networking\Session\NetworkSession.h"
#include "..\ImGui_Handler.h"

std::mutex commandInsertMtx;

const char command_error_bad_arg[] = "# exception catch (bad arg): ";

DECL_ComVarCommandPtr(d3d9ex_var, bool*, &H2Config_d3dex, 
	"var_d3d9ex", "enable/disable d3d9ex, 1 parameter(s): <bool>", 1, CommandCollection::BoolVarHandler);
DECL_ComVarCommandPtr(network_stats_overlay_var, bool*, &ImGuiHandler::g_network_stats_overlay, 
	"var_net_metrics", "enable/disable useful net metrics, 0 parameter(s)", 1, CommandCollection::BoolVarHandler);

// don't forget to add '_cmd' after the name, 
// if you add a variable command created using `DECL_ComVarCommandPtr` macro
std::vector<ConsoleCommand*> CommandCollection::commandTable = {
	&d3d9ex_var_cmd,
	&network_stats_overlay_var_cmd,
	new ConsoleCommand("help", "outputs all commands, 0 parameter(s)", 0, CommandCollection::HelpCmd),
	new ConsoleCommand("logpeers", "logs all peers to console, 0 parameter(s)", 0, CommandCollection::LogPeersCmd),
	new ConsoleCommand("logplayers", "logs all players to console, 0 parameter(s)", 0, CommandCollection::LogPlayersCmd),
	new ConsoleCommand("mapdownload", "download specified map, 1 parameter(s): <string>", 1, CommandCollection::DownloadMapCmd),
	new ConsoleCommand("reloadmaps", "re-load custom map data cache into memory, 0 parameter(s)", 0, CommandCollection::ReloadMapsCmd),
};

void CommandCollection::InitializeCommandsMap()
{
	static bool InitializeCommandsMap_initialized = false;
	if (InitializeCommandsMap_initialized) return;
	InitializeCommandsMap_initialized = true;

	atexit([]() -> void {
		for (auto command : commandTable)
		{
		}

		commandTable.clear();
		}
	);
}

void CommandCollection::InsertCommand(ConsoleCommand* newCommand)
{
	std::scoped_lock(commandInsertMtx);

	for (auto command : commandTable)
	{
		if (!strcmp(newCommand->GetName(), command->GetName()))
		{
			LOG_ERROR_GAME("{} - command {} already present!", __FUNCTION__, newCommand->GetName());
			return;
		}
	}

	commandTable.emplace_back(newCommand);
}

ConsoleVarCommand* CommandCollection::GetVarCommandByName(const std::string& name)
{
	std::scoped_lock(commandInsertMtx);

	for (auto command : commandTable)
	{
		if (!strcmp(name.c_str(), command->GetName()))
			return dynamic_cast<ConsoleVarCommand*>(command);
	}

	return nullptr;
}

// in case your variable needs to be set/updated
void CommandCollection::SetVarCommandPtr(const std::string& name, ComVar* varPtr)
{
	ConsoleVarCommand* varCmdPtr = GetVarCommandByName(name);
	if (varCmdPtr != nullptr)
	{
		varCmdPtr->UpdateVarPtr(varPtr);
	}
}

//////////////////////////////////////////////////////////////////////////
//	commands
//////////////////////////////////////////////////////////////////////////

int CommandCollection::BoolVarHandler(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;
	auto var = reinterpret_cast<ComVarTPtr<bool*>*>(cbData.commandVar);

	std::string exception;
	if (!var->SetValFromStr(tokens[1], 10, exception))
	{
		output->Output(StringFlag_None, command_error_bad_arg);
		output->OutputFmt(StringFlag_None, "	%s", exception.c_str());
	}
	return 0;
}

int CommandCollection::KickPeerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;
	ComVarT<int> peerIdxVar;
	std::string exception;

	do
	{
		if (!peerIdxVar.SetValFromStr(tokens[1], 10, exception))
		{
			output->Output(StringFlag_None, command_error_bad_arg);
			output->OutputFmt(StringFlag_None, "	%s", exception.c_str());
			break;
		}
		else if (Memory::IsDedicatedServer()) {
			output->Output(StringFlag_None, "# don't use this on dedis");
			break;
		}
		else if (!NetworkSession::LocalPeerIsSessionHost()) {
			output->Output(StringFlag_None, "# only the host can kick players");
			break;
		}
		else if (NetworkSession::GetCurrentNetworkSession()->session_host_peer_index == peerIdxVar.GetVal()) {
			output->Output(StringFlag_None, "# don't kick yourself");
			break;
		}
		else if (peerIdxVar.GetVal() >= NetworkSession::GetPeerCount()) {
			output->Output(StringFlag_None, "# peer at the specified index doesn't exist");
			break;
		}

		NetworkSession::KickPeer(peerIdxVar.GetVal());
	} while (0);

	return 0;
}

int CommandCollection::DownloadMapCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;
	
	if (!NetworkSession::LocalPeerIsSessionHost())
	{
		output->Output(StringFlag_None, "# cannot download map using command while not being the session host!");
		return 0;
	}

	std::wstring fileNameToDownload(std::wstring(tokens[1].begin(), tokens[1].end()));
	auto downloadQuery = mapManager->AddDownloadQuery(fileNameToDownload);
	downloadQuery->StartMapDownload(); // since we have the map name, start the download
	return 0;
}

int CommandCollection::ReloadMapsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	mapManager->ReloadAllMaps();
	return 0;
}

int CommandCollection::HelpCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;

	output->Output(StringFlag_None, "# available commands: ");

	for (auto& command_entry : CommandCollection::commandTable)
	{
		if ((command_entry->GetFlags() & CommandFlag_Hidden) == 0)
		{
			output->OutputFmt(StringFlag_None, "# %s ", command_entry->GetName());
			if (command_entry->GetDescription() != NULL)
			{
				output->OutputFmt(StringFlag_None, "    # command description: %s", command_entry->GetDescription());
			}
		}
	}
	return 0;
}

int CommandCollection::LogPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;

	if (!NetworkSession::GetCurrentNetworkSession(nullptr))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	if (!NetworkSession::LocalPeerIsSessionHost())
	{
		output->Output(StringFlag_None, "# must be network session host");
		return 0;
	}

	output->OutputFmt(StringFlag_None, "# %i players: ", NetworkSession::GetPlayerCount());

	for (int playerIdx = 0; playerIdx < ENGINE_PLAYER_MAX; playerIdx++)
	{
		if (NetworkSession::PlayerIsActive(playerIdx))
		{
			std::wstring playerNameWide(NetworkSession::GetPlayerName(playerIdx));
			std::string playerName(playerNameWide.begin(), playerNameWide.end());
			std::string outStr = "Player index=" + std::to_string(playerIdx);
			outStr += ", Peer index=" + std::to_string(NetworkSession::GetPeerIndex(playerIdx));
			outStr += ", PlayerName=" + playerName;

			playerNameWide = s_player::GetName(playerIdx);
			playerName = std::string(playerNameWide.begin(), playerNameWide.end());

			outStr += ", Name from game player state=" + playerName;
			outStr += ", Team=" + std::to_string(NetworkSession::GetPlayerTeam(playerIdx));
			outStr += ", Identifier=" + std::to_string(NetworkSession::GetPlayerId(playerIdx));

			output->Output(StringFlag_None, outStr.c_str());
		}
	}

	return 0;
}

int CommandCollection::LogPeersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;

	s_network_session* session;

	if (!NetworkSession::GetCurrentNetworkSession(&session))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	if (!NetworkSession::LocalPeerIsSessionHost())
	{
		output->Output(StringFlag_None, "# must be network session host");
		return 0;
	}

	s_network_observer* observer = NetworkSession::GetCurrentNetworkSession()->p_network_observer;

	output->OutputFmt(StringFlag_None, "# %i peers: ", NetworkSession::GetPeerCount());

	for (int peerIdx = 0; peerIdx < NetworkSession::GetPeerCount(); peerIdx++)
	{
		auto peer_observer_channel = &observer->observer_channels[session->peer_observer_channels[peerIdx].observer_index];

		std::wstring peerNameWide(session->membership[0].peer_data[peerIdx].name);
		std::string peerName(peerNameWide.begin(), peerNameWide.end());

		std::string outStr = "Peer index=" + std::to_string(peerIdx);
		outStr += ", Peer Name=" + peerName;
		outStr += ", Connection Status=" + std::to_string(peer_observer_channel->state);
		outStr += ", Peer map state: " + std::to_string(session->membership[0].peer_data[peerIdx].map_status);
		int playerIdx = session->membership[0].peer_data[peerIdx].player_index[0];
		if (playerIdx != -1)
		{
			std::wstring playerNameWide(NetworkSession::GetPlayerName(playerIdx));
			std::string playerName(playerNameWide.begin(), playerNameWide.end());
			outStr += ", Player index=" + std::to_string(playerIdx);
			outStr += ", Player name=" + playerName;

			playerNameWide = s_player::GetName(playerIdx);
			playerName = std::string(playerNameWide.begin(), playerNameWide.end());
			outStr += ", Name from game player state=" + playerName;
		}
		output->Output(StringFlag_None, outStr.c_str());
	}

	return 0;
}



