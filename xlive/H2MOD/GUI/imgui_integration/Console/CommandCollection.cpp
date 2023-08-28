#include "stdafx.h"
#include "ImGui_ConsoleImpl.h"

#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/main/main_game.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"

#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/MainLoopPatches/MainGameTime/MainGameTime.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Utils/Utils.h"

// for XNet connection logging
#include "XLive/xnet/IpManagement/XnIp.h"


std::mutex commandInsertMtx;

bool readObjectIds = true;
std::map<std::string, unsigned int> objectIds;

const char command_error_bad_arg[] = "# exception catch (bad arg): ";

ComVarFromPtr(d3d9ex_var, bool*, &H2Config_d3d9ex, 
	"var_d3d9ex", "enable/disable d3d9ex, 1 parameter(s): <bool>", 1, 1, CommandCollection::SetD3D9ExStateCmd);
ComVarFromPtr(network_stats_overlay_var, bool*, &ImGuiHandler::g_network_stats_overlay, 
	"var_net_metrics", "enable/disable useful net metrics, 0 parameter(s)", 1, 1, CommandCollection::NetworkMetricsCmd);
ComVarFromPtr(og_frame_limiter_var, bool*, &MainGameTime::fps_limiter_enabled,
	"var_og_frame_limiter", "enabled/disable original h2 frame limiter", 1, 1, CommandCollection::BoolVarHandlerCmd);

extern bool displayXyz;
ComVarFromPtr(display_xyz_var, bool*, &displayXyz,
	"var_display_xyz", "enable/disable players's xyz, 1 parameter(s): <bool>", 1, 1, CommandCollection::DisplayXyzCmd);

// don't forget to add '_cmd' after the name, 
// if you add a variable command created using `DECL_ComVarCommandPtr` macro
std::vector<ConsoleCommand*> CommandCollection::commandTable = {
	&d3d9ex_var_cmd,
	&display_xyz_var_cmd,
	&network_stats_overlay_var_cmd,
	new ConsoleCommand("help", "outputs all commands, 0 - 1 parameter(s): <string>(optional): command name", 0, 1, CommandCollection::HelpCmd),
	new ConsoleCommand("log_peers", "logs all peers to console, 0 parameter(s)", 0, 0, CommandCollection::LogPeersCmd),
	new ConsoleCommand("log_players", "logs all players to console, 0 parameter(s)", 0, 0,CommandCollection::LogPlayersCmd),
	new ConsoleCommand("kick_peer", "kicks peer from network session, 1 parameter(s): <int>: peer index", 1, 1, CommandCollection::KickPeerCmd),
	new ConsoleCommand("leave_session", "leave current session, 0 parameter(s)", 0, 0, CommandCollection::LeaveNetworkSessionCmd),
	new ConsoleCommand("is_host", "logs if you are session host or not, 0 parameter(s)", 0, 0, CommandCollection::IsSessionHostCmd),
	new ConsoleCommand("map_download", "download specified map, 1 parameter(s): <string>", 1, 1, CommandCollection::DownloadMapCmd),
	new ConsoleCommand("reload_maps", "re-load custom map data cache into memory, 0 parameter(s)", 0, 0, CommandCollection::ReloadMapsCmd),
	new ConsoleCommand("log_map_file_name", "logs selected map filename, 0 parameter(s)", 0, 0, CommandCollection::LogSelectedMapFilenameCmd),
	new ConsoleCommand("request_map_file", "requests map file name from host, 0 parameter(s)", 0, 0, CommandCollection::RequestFileNameCmd),
	new ConsoleCommand("max_players", "set maximum players that can join, 1 parameter(s): <int>", 1, 1, CommandCollection::SetMaxPlayersCmd),
	new ConsoleCommand("delete_object", "deletes an object, 1 parameter(s): <int>: object datum index", 1, 1, CommandCollection::DestroyObjectCmd),
	new ConsoleCommand("warp_fix", "(EXPERIMENTAL) increases client position update control threshold", 1, 1, CommandCollection::WarpFixCmd, CommandFlags_::CommandFlag_Hidden),
	new ConsoleCommand("log_xnet_connections", "logs the xnet connections for debugging purposes, 0 parameter(s)", 0, 0, CommandCollection::LogXNetConnectionsCmd, CommandFlags_::CommandFlag_Hidden),
	new ConsoleCommand("spawn", "spawn an object from the list, 4 - 10 parameter(s): "
		"<string>: object name <int>: count <bool>: same team, near player <float3>: (only if near player false) position xyz, rotation (optional) ijk", 4, 10, CommandCollection::SpawnCmd),
	new ConsoleCommand("spawn_reload_command_list", "reload object ids for spawn command from file, 0 parameter(s)", 0, 0, CommandCollection::ReloadSpawnCommandListCmd),
	new ConsoleCommand("tag_inject", "injects tag into memory, 3 parameter(s): <string>: tag_name, tag_type, map_name", 3, 3, CommandCollection::InjectTagCmd, CommandFlags_::CommandFlag_Hidden),
	new ConsoleCommand("crash", "crashes the game", 0, 0, CommandCollection::Crash),
	new ConsoleCommand("map_name", "load a map with the following name, 1 parameter(s): <string>", 1, 1, CommandCollection::map_name),
	new ConsoleCommand("game_difficulty", "set the difficulty when using the map_load command, 1 parameter(s): <int>", 1, 1, CommandCollection::game_difficulty),
	new ConsoleCommand("game_coop_players", "set the coop player count when using the map_load command, 1 parameter(s): <int>", 1, 1, CommandCollection::game_coop_players),
	new ConsoleCommand("game_multiplayer", "sets the multiplayer variant for the next map, 1 parameter(s): <string>", 1, 1, CommandCollection::game_multiplayer),
	new ConsoleCommand("game_splitscreen", "sets the number of multiplayer splitscreen players for the next map, 1 parameter(s): <int>", 1, 1, CommandCollection::game_splitscreen),
	new ConsoleCommand("game_mode", "sets the game mode for the next map, 1 parameter(s): <int>", 1, 1, CommandCollection::game_mode)
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
void CommandCollection::SetVarCommandPtr(const std::string& name, IComVar* varPtr)
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

int CommandCollection::BoolVarHandlerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;
	auto var = reinterpret_cast<ComVarT<bool*>*>(cbData.commandVar);

	std::string exception;
	if (!var->SetValFromStr(tokens[1], 10, exception))
	{
		output->Output(StringFlag_None, command_error_bad_arg);
		output->Output(StringFlag_None, "	%s", exception.c_str());
	}
	return 0;
}

int CommandCollection::DisplayXyzCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;
	
	if (s_game_globals::game_is_multiplayer()
		&& !NetworkSession::LocalPeerIsSessionHost()) 
	{
		output->Output(StringFlag_None, "# only host can see xyz for now...");
		return 0;
	}

	return BoolVarHandlerCmd(tokens, cbData);
}

int CommandCollection::NetworkMetricsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;

	if (Memory::IsDedicatedServer()) {
		output->Output(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	return BoolVarHandlerCmd(tokens, cbData);
}

int CommandCollection::SetD3D9ExStateCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;
	
	if (Memory::IsDedicatedServer()) {
		output->Output(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	return BoolVarHandlerCmd(tokens, cbData);
}

int CommandCollection::LogXNetConnectionsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;
	gXnIpMgr.LogConnectionsToConsole(output);
	return 0;
}

int CommandCollection::LogSelectedMapFilenameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;

	if (!NetworkSession::GetActiveNetworkSession(nullptr))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	
	std::string mapFileName;
	std::wstring mapFilenameWide;
	MapManager::GetMapFilename(mapFilenameWide);
	mapFileName = std::string(mapFilenameWide.begin(), mapFilenameWide.end());
	output->Output(StringFlag_None, "# map file name: %s", mapFileName.c_str());
	return 0;
}

int CommandCollection::RequestFileNameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;

	if (!NetworkSession::GetActiveNetworkSession(nullptr))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	else if (NetworkSession::LocalPeerIsSessionHost())
	{
		output->Output(StringFlag_None, "# only clients can request map file name");
		return 0;
	}

	NetworkMessage::SendRequestMapFilename(NONE);
	return 0;
}

int CommandCollection::LeaveNetworkSessionCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;

	if (!NetworkSession::GetActiveNetworkSession(nullptr))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	else if (Memory::IsDedicatedServer()) {
		output->Output(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	NetworkSession::LeaveSession();
	return 0;
}

int CommandCollection::IsSessionHostCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;

	s_network_session* session;
	if (!NetworkSession::GetActiveNetworkSession(&session))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}

	std::string isHostStr;
	isHostStr += "# Session host: ";
	isHostStr += (NetworkSession::LocalPeerIsSessionHost() ? "yes" : "no");
	isHostStr += ", value=" + std::to_string(session->local_session_state);
	output->Output(StringFlag_None, isHostStr.c_str());
	return 0;
}

int CommandCollection::KickPeerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;
	ComVar<int> peerIdxVar;
	std::string exception;

	do
	{
		if (!peerIdxVar.SetValFromStr(tokens[1], 10, exception))
		{
			output->Output(StringFlag_None, command_error_bad_arg);
			output->Output(StringFlag_None, "	%s", exception.c_str());
			break;
		}
		else if (Memory::IsDedicatedServer()) {
			output->Output(StringFlag_None, "# command unavailable on dedicated servers");
			break;
		}
		else if (!NetworkSession::LocalPeerIsSessionHost()) {
			output->Output(StringFlag_None, "# only the host can kick players");
			break;
		}
		else if (NetworkSession::GetLocalPeerIndex() == peerIdxVar.GetVal()) {
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
	ConsoleLog* output = (ConsoleLog*)cbData.strOutput;
	
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
	ConsoleLog* output = cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;

	const std::string* commandToHelp = nullptr;

	int parameterCount = tokens.size() - 1; // only parameters
	bool singleCommandHelp = parameterCount >= 1;
	bool singleCommandHelpFound = false;

	if (singleCommandHelp)
		commandToHelp = &tokens[1];

	if (!singleCommandHelp)
		output->Output(StringFlag_None, "# available commands: ");

	for (auto command_entry : CommandCollection::commandTable)
	{
		if (singleCommandHelp && _strnicmp(command_entry->GetName(), commandToHelp->c_str(), commandToHelp->length()) != 0)
			continue;

		if (!command_entry->Hidden())
		{
			output->Output(StringFlag_None, "# %s ", command_entry->GetName());
			if (command_entry->GetDescription() != NULL)
			{
				output->Output(StringFlag_None, "    # command description: %s", command_entry->GetDescription());
			}
			else
			{
				output->Output(StringFlag_None, "	# command has no description");
			}

			if (singleCommandHelp)
			{
				singleCommandHelpFound = true;
				break;
			}
		}
	}

	if (singleCommandHelp && !singleCommandHelpFound)
		output->Output(StringFlag_None, "	# unknown command: %s", commandToHelp->c_str());

	return 0;
}

int CommandCollection::LogPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;

	if (!NetworkSession::GetActiveNetworkSession(nullptr))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	else if (!NetworkSession::LocalPeerIsSessionHost())
	{
		output->Output(StringFlag_None, "# must be network session host");
		return 0;
	}

	output->Output(StringFlag_None, "# %i players: ", NetworkSession::GetPlayerCount());

	for (int playerIdx = 0; playerIdx < k_maximum_players; playerIdx++)
	{
		if (NetworkSession::PlayerIsActive(playerIdx))
		{
			std::wstring playerNameWide(NetworkSession::GetPlayerName(playerIdx));
			std::string playerName(playerNameWide.begin(), playerNameWide.end());
			std::string outStr = "# Player index=" + std::to_string(playerIdx);
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
	ConsoleLog* output = cbData.strOutput;
	const ConsoleCommand* command_data = cbData.consoleCommandData;

	s_network_session* session;

	if (!NetworkSession::GetActiveNetworkSession(&session))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	else if (!NetworkSession::LocalPeerIsSessionHost())
	{
		output->Output(StringFlag_None, "# must be network session host");
		return 0;
	}

	s_network_observer* observer = NetworkSession::GetActiveNetworkSession()->p_network_observer;

	output->Output(StringFlag_None, "# %i peers: ", NetworkSession::GetPeerCount());

	for (int peerIdx = 0; peerIdx < NetworkSession::GetPeerCount(); peerIdx++)
	{
		auto peer_observer_channel = &observer->observer_channels[session->observer_channels[peerIdx].observer_index];

		std::wstring peerNameWide(session->membership[0].peers[peerIdx].name);
		std::string peerName(peerNameWide.begin(), peerNameWide.end());

		std::string outStr = "# Peer index=" + std::to_string(peerIdx);
		outStr += ", Peer Name=" + peerName;
		outStr += ", Connection Status=" + std::to_string(peer_observer_channel->state);
		outStr += ", Peer map state: " + std::to_string(session->membership[0].peers[peerIdx].map_status);
		int playerIdx = session->membership[0].peers[peerIdx].player_index[0];
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

int CommandCollection::SetMaxPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = cbData.strOutput;

	ComVar<int> value; 
	std::string exception;

	do 
	{
		if (!NetworkSession::LocalPeerIsSessionHost()) {
			output->Output(StringFlag_None, "# can be only used by host");
			break;
		}
		else if (!value.SetValFromStr(tokens[1], 10, exception))
		{
			output->Output(StringFlag_None, command_error_bad_arg);
			output->Output(StringFlag_None, "	%s", exception.c_str());
			break;
		}
		else if (value.GetVal() < 1 || value.GetVal() > 16) {
			output->Output(StringFlag_None, "# the value needs to be between 1 and 16");
			break;
		}
		else if (value.GetVal() < NetworkSession::GetPlayerCount()) {
			output->Output(StringFlag_None, "# you can't set a value of max players smaller than the actual number of players on the server");
			break;
		}

		NetworkSession::GetActiveNetworkSession()->parameters[0].max_party_players = value.GetVal();
		output->Output(StringFlag_None, "# maximum players set: %i", value.GetVal());
	} while (0);

	return 0;
}

int CommandCollection::WarpFixCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = cbData.strOutput;
	ComVar<bool> warpFixVar;

	if (Memory::IsDedicatedServer()) {
		output->Output(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	std::string exception;
	if (!warpFixVar.SetValFromStr(tokens[1], 10, exception))
	{
		output->Output(StringFlag_None, command_error_bad_arg);
		output->Output(StringFlag_None, "	%s", exception.c_str());
		return 0;
	}

	H2Tweaks::WarpFix(warpFixVar.GetVal());
	return 0;
}

int CommandCollection::DestroyObjectCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = cbData.strOutput;
	ComVar<datum> datumIdx;

	std::string exception;
	if (!datumIdx.SetValFromStr(tokens[1], 0, exception))
	{
		output->Output(StringFlag_None, command_error_bad_arg);
		output->Output(StringFlag_None, "	%s", exception.c_str());
		return 0;
	}

	void* object_data = (void*)object_try_and_get_and_verify_type(datumIdx.GetVal(), -1);
	if (object_data != NULL)
	{
		DeleteObject(datumIdx.GetVal());
		output->Output(StringFlag_None, "# deleted object idx: 0x%X", datumIdx.GetVal());
	}
	else
	{
		output->Output(StringFlag_None, "# failed to delete object idx: 0x%X", datumIdx.GetVal());
	}

	return 0;
}

int CommandCollection::ReloadSpawnCommandListCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = cbData.strOutput;
	output->Output(StringFlag_None, "# object ids reset next time spawn is used");
	readObjectIds = false;
	return 0;
}

int CommandCollection::SpawnCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = cbData.strOutput;
	
	int tokenArgPos = 1;

	// spawn object_name count same_team near_player x y z i j k

	datum objectDatum = DATUM_INDEX_NONE;
	ComVar<int> count;
	ComVar<float> varPos[3];
	ComVar<float> varRotation[3];
	ComVar<bool> sameTeam, nearPlayerSpawn;
	int parameterCount = tokens.size() - 1; // only parameters

	int localPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0));
	datum playerUnitIdx = s_player::GetPlayerUnitDatumIndex(localPlayerIdx);

	std::string objectName = tokens[tokenArgPos++];

	if (s_game_globals::game_is_mainmenu()) {
		output->Output(StringFlag_None, "# can only be used ingame");
		return 0;
	}
	else if (!NetworkSession::LocalPeerIsSessionHost()) {
		output->Output(StringFlag_None, "# can only be used by the session host");
		return 0;
	}
	else if (!count.SetValFromStr(tokens[tokenArgPos++])
		|| !sameTeam.SetValFromStr(tokens[tokenArgPos++])
		|| !nearPlayerSpawn.SetValFromStr(tokens[tokenArgPos++])
		)
	{
		output->Output(StringFlag_None, "# one or more invalid spawn arguments");
		return 0;
	}

	if (nearPlayerSpawn.GetVal())
	{
		real_point3d* localPlayerPos = h2mod->get_player_unit_coords(localPlayerIdx);
		if (localPlayerPos != nullptr)
		{
			varPos[0].SetVal(localPlayerPos->x + 0.5f);
			varPos[1].SetVal(localPlayerPos->y + 0.5f);
			varPos[2].SetVal(localPlayerPos->z + 0.5f);
		}
	}
	else
	{
		if (parameterCount < 7
			|| !varPos[0].SetValFromStr(tokens[tokenArgPos++])
			|| !varPos[1].SetValFromStr(tokens[tokenArgPos++])
			|| !varPos[2].SetValFromStr(tokens[tokenArgPos++]))
		{
			output->Output(StringFlag_None, "# insufficient/invalid xyz position spawn arguments");
			return 0;
		}
	}

	// check if rotation parameters were passed
	// even if invalid, error handled after
	bool withRotation = (parameterCount >= 5 && nearPlayerSpawn.GetVal())
		|| (parameterCount >= 8 && !nearPlayerSpawn.GetVal());

	if (withRotation)
	{
		if ((parameterCount < 7 && nearPlayerSpawn.GetVal())
			|| (parameterCount < 10 && !nearPlayerSpawn.GetVal())
			|| !varRotation[0].SetValFromStr(tokens[tokenArgPos++])
			|| !varRotation[1].SetValFromStr(tokens[tokenArgPos++])
			|| !varRotation[2].SetValFromStr(tokens[tokenArgPos++]))
		{
			output->Output(StringFlag_None, "# insufficient/invalid ijk rotation spawn arguments");
			return 0;
		}
	}

	//lookup a commands.txt file that contain string->object_datums
	if (readObjectIds)
		ReadObjectDatumIdx();

	if (objectIds.find(objectName) == objectIds.end()) {
		objectDatum = strtoul(objectName.c_str(), NULL, 0);
	}
	else {
		objectDatum = objectIds[objectName];
	}

	real_vector3d rotation;
	real_vector3d* pRotation = nullptr;
	real_point3d position;
	real_point3d* pPosition = nullptr;

	if (!nearPlayerSpawn.GetVal())
	{
		pPosition = &position;
		position = { varPos[0].GetVal(), varPos[1].GetVal(), varPos[2].GetVal() };
	}

	if (withRotation)
	{
		pRotation = &rotation;
		rotation = { varRotation[0].GetVal(), varRotation[1].GetVal(), varRotation[2].GetVal() };
	}

	ObjectSpawn(objectDatum, count.GetVal(), pPosition, pRotation, 1.0f, playerUnitIdx, sameTeam.GetVal());

	// output->OutputFmt(StringFlag_None, "# spawned: %s, near player: %s with rotation: %i", objectName.c_str(), nearPlayerSpawn.GetValStr().c_str(), withRotation);

	return 0;
}

int CommandCollection::InjectTagCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ConsoleLog* output = cbData.strOutput;

	if (!NetworkSession::LocalPeerIsSessionHost() 
		&& !s_game_globals::game_is_campaign())
	{
		output->Output(StringFlag_None, "# can only be used by the session host");
		return 0;
	}

	std::string tagName = tokens[1];
	blam_tag tagType = blam_tag::from_string(tokens[2]);
	std::string mapName = tokens[3];

	auto tagDatum = tag_loader::Get_tag_datum(tagName, tagType, mapName);
	tag_loader::Load_tag(tagDatum, true, mapName);
	tag_loader::Push_Back();
	output->Output(StringFlag_None, "# loaded tag datum: %#X", tag_loader::ResolveNewDatum(tagDatum));

	LOG_INFO_GAME("{} - {} {} {}", tagName, tagType.as_string(), mapName);
	return 0;
}

int CommandCollection::Crash(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	int* test = nullptr;
#pragma warning( push )
#pragma warning( disable : 6011 )
	return *test++;
#pragma warning( pop )
}

//////////////////////////////////////////////////////////////////////////
//	commands end
//////////////////////////////////////////////////////////////////////////

void CommandCollection::ObjectSpawn(datum object_idx, int count, const real_point3d* position, const real_vector3d* rotation, datum playerIdx, float randomMultiplier, bool sameTeam) 
{
	typedef void(__cdecl* set_orientation_t)(real_vector3d* forward, real_vector3d* up, const real_point3d* orient);
	auto p_vector3d_from_euler_angles3d = Memory::GetAddress<set_orientation_t>(0x3347B);

	for (int i = 0; i < count; i++) 
	{
		try 
		{
			s_object_placement_data nObject;
			int localPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0));
			datum playerUnitIdx = s_player::GetPlayerUnitDatumIndex(localPlayerIdx);
			real_point3d* localPlayerPos = h2mod->get_player_unit_coords(localPlayerIdx);
			
			if (!DATUM_IS_NONE(object_idx)) 
			{
				Engine::Objects::create_new_placement_data(&nObject, object_idx, -1, 0);

				if (position)
				{
					nObject.position = *position;
				}
				else if (localPlayerPos)
				{
					nObject.position.x = localPlayerPos->x * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
					nObject.position.y = localPlayerPos->y * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
					nObject.position.z = (localPlayerPos->z + 5.0f) * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
				}

				if (rotation)
				{
					p_vector3d_from_euler_angles3d(&nObject.orientation, &nObject.up, rotation);
				}

				if (!sameTeam)
					nObject.team_index = NONE;

				LOG_TRACE_GAME("object_datum = {0:#x}, x={1:f}, y={2:f}, z={3:f}", object_idx, nObject.position.x, nObject.position.y, nObject.position.z);
				datum object_gamestate_datum = Engine::Objects::object_new(&nObject);
				Engine::Objects::simulation_action_object_create(object_gamestate_datum);
			}
		}
		catch (...) {
			LOG_TRACE_GAME("{} - error running spawn command", __FUNCTION__);
		}
	}
}

void CommandCollection::ReadObjectDatumIdx() {
	std::ifstream infile("commands.txt");
	if (infile.good()) {
		std::string line;
		while (std::getline(infile, line)) {
			std::vector<std::string> command;
			if (tokenize(line.c_str(), line.length(), ",", command))
			{
				if (command.size() != 2) {
					LOG_TRACE_GAME("Found line not comma separated right, {}", line);
				}
				else {
					objectIds[command[0]] = strtoul(command[1].c_str(), NULL, 0);
				}
			}
		}
	}
}

void CommandCollection::DeleteObject(datum objectDatumIdx)
{
	if (!DATUM_IS_NONE(objectDatumIdx))
	{
		Engine::Objects::object_destroy(objectDatumIdx);
	}
}

int CommandCollection::map_name(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	main_game_launch(tokens[1].c_str());
	return 0;
}

int CommandCollection::game_difficulty(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ComVar<int> difficulty;
	std::string exception;
	difficulty.SetValFromStr(tokens[1], 10, exception);

	main_game_launch_set_difficulty(difficulty.GetVal());
	return 0;
}

int CommandCollection::game_coop_players(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ComVar<int> player_count;
	std::string exception;
	player_count.SetValFromStr(tokens[1], 10, exception);

	main_game_launch_set_coop_player_count(player_count.GetVal());
	return 0;
}

int CommandCollection::game_multiplayer(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	main_game_launch_set_multiplayer_variant(tokens[1].c_str());
	return 0;
}

int CommandCollection::game_splitscreen(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ComVar<int> player_count;
	std::string exception;
	player_count.SetValFromStr(tokens[1], 10, exception);

	main_game_launch_set_multiplayer_splitscreen_count(player_count.GetVal());
	return 0;
}

int CommandCollection::game_mode(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	ComVar<int> game_mode;
	std::string exception;
	game_mode.SetValFromStr(tokens[1], 10, exception);

	main_game_launch_set_game_mode(game_mode.GetVal());
	return 0;
}
