#include "stdafx.h"

#include "ImGui_ConsoleImpl.h"
#include "H2MOD\Utils\Utils.h"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\MapManager\MapManager.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"

#include "Blam\Engine\Networking\Session\NetworkSession.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"

// for XNet connection logging
#include "XLive\xnet\IpManagement\XnIp.h"

#include "..\ImGui_Handler.h"

std::mutex commandInsertMtx;

bool readObjectIds = true;
std::unordered_map<std::string, unsigned int> objectIds;

const char command_error_bad_arg[] = "# exception catch (bad arg): ";

DECL_ComVarCommandPtr(d3d9ex_var, bool*, &H2Config_d3dex, 
	"var_d3d9ex", "enable/disable d3d9ex, 1 parameter(s): <bool>", 1, CommandCollection::SetD3D9ExStateCmd);
DECL_ComVarCommandPtr(network_stats_overlay_var, bool*, &ImGuiHandler::g_network_stats_overlay, 
	"var_net_metrics", "enable/disable useful net metrics, 0 parameter(s)", 1, CommandCollection::NetworkMetricsCmd);

extern bool displayXyz;
DECL_ComVarCommandPtr(display_xyz_var, bool*, &displayXyz,
	"var_display_xyz", "enable/disable players's xyz, 1 parameter(s): <bool>", 1, CommandCollection::DisplayXyzCmd);

// don't forget to add '_cmd' after the name, 
// if you add a variable command created using `DECL_ComVarCommandPtr` macro
std::vector<ConsoleCommand*> CommandCollection::commandTable = {
	&d3d9ex_var_cmd,
	&display_xyz_var_cmd,
	&network_stats_overlay_var_cmd,
	new ConsoleCommand("help", "outputs all commands, 0 parameter(s)", 0, CommandCollection::HelpCmd),
	new ConsoleCommand("logpeers", "logs all peers to console, 0 parameter(s)", 0, CommandCollection::LogPeersCmd),
	new ConsoleCommand("logplayers", "logs all players to console, 0 parameter(s)", 0, CommandCollection::LogPlayersCmd),
	new ConsoleCommand("kickpeer", "kicks peer from network session, 1 parameter(s): <int>: peer_index", 1, CommandCollection::KickPeerCmd),
	new ConsoleCommand("leavesession", "leave current session, 0 parameter(s)", 0, CommandCollection::LeaveNetworkSessionCmd),
	new ConsoleCommand("ishost", "logs if you are session host or not, 0 parameter(s)", 0, CommandCollection::IsSessionHostCmd),
	new ConsoleCommand("mapdownload", "download specified map, 1 parameter(s): <string>", 1, CommandCollection::DownloadMapCmd),
	new ConsoleCommand("reloadmaps", "re-load custom map data cache into memory, 0 parameter(s)", 0, CommandCollection::ReloadMapsCmd),
	new ConsoleCommand("logmapfilename", "logs selected map filename, 0 parameter(s)", 0, CommandCollection::LogSelectedMapFilenameCmd),
	new ConsoleCommand("requestmapfilename", "requests map file name from host, 0 parameter(s)", 0, CommandCollection::RequestFileNameCmd),
	new ConsoleCommand("maxplayers", "set maximum players that can join, 1 parameter(s): <int>", 1, CommandCollection::SetMaxPlayersCmd),
	new ConsoleCommand("deleteobject", "deletes an object, 1 parameter(s): <int>: object datum index", 1, CommandCollection::DestroyObjectCmd),
	new ConsoleCommand("warpfix", "(EXPERIMENTAL) increases client position update control threshold", 1, CommandCollection::WarpFixCmd, CommandFlags_::CommandFlag_Hidden),
	new ConsoleCommand("logxnetconnections", "logs the xnet connections for debugging purposes, 0 parameter(s)", 0, CommandCollection::LogXNetConnectionsCmd, CommandFlags_::CommandFlag_Hidden),
	new ConsoleCommand("spawn", "spawn an object from the list, 4 - 10 parameter(s): "
		"<string>: object_name <int>: count <bool>: same_team, near_player <float> : (if near_player true) xyz, (rotation optional) ijk", 4, CommandCollection::SpawnCmd),
	new ConsoleCommand("spawnreloadcommandlist", "reload object ids for spawn command from file, 0 parameter(s)", 0, CommandCollection::ReloadSpawnCommandListCmd),
	new ConsoleCommand("taginject", "injects tag into memory, 3 parameter(s): <string>: tag_name, tag_type, map_name", 3, CommandCollection::InjectTagCmd, CommandFlags_::CommandFlag_Hidden)
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

int CommandCollection::BoolVarHandlerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
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

int CommandCollection::DisplayXyzCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;
	
	if (h2mod->GetEngineType() == e_engine_type::_multiplayer 
		&& !NetworkSession::LocalPeerIsSessionHost()) 
	{
		output->Output(StringFlag_None, "# only host can see xyz for now...");
		return 0;
	}

	return BoolVarHandlerCmd(tokens, cbData);
}

int CommandCollection::NetworkMetricsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;

	if (Memory::IsDedicatedServer()) {
		output->Output(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	return BoolVarHandlerCmd(tokens, cbData);
}

int CommandCollection::SetD3D9ExStateCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;
	
	if (Memory::IsDedicatedServer()) {
		output->Output(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	return BoolVarHandlerCmd(tokens, cbData);
}

int CommandCollection::LogXNetConnectionsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;
	gXnIp.LogConnectionsToConsole(output);
	return 0;
}

int CommandCollection::LogSelectedMapFilenameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;

	if (!NetworkSession::GetCurrentNetworkSession(nullptr))
	{
		output->Output(StringFlag_None, "# not in a network session");
		return 0;
	}
	
	std::string mapFileName;
	std::wstring mapFilenameWide;
	mapManager->GetMapFilename(mapFilenameWide);
	mapFileName = std::string(mapFilenameWide.begin(), mapFilenameWide.end());
	output->OutputFmt(StringFlag_None, "# map file name: %s", mapFileName.c_str());
	return 0;
}

int CommandCollection::RequestFileNameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = (IOutput*)cbData.strOutput;

	if (!NetworkSession::GetCurrentNetworkSession(nullptr))
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
	IOutput* output = (IOutput*)cbData.strOutput;

	if (!NetworkSession::GetCurrentNetworkSession(nullptr))
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
	IOutput* output = (IOutput*)cbData.strOutput;

	s_network_session* session;
	if (!NetworkSession::GetCurrentNetworkSession(&session))
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
			output->Output(StringFlag_None, "# command unavailable on dedicated servers");
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
		if (!command_entry->Hidden())
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
	else if (!NetworkSession::LocalPeerIsSessionHost())
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

		std::string outStr = "# Peer index=" + std::to_string(peerIdx);
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

int CommandCollection::SetMaxPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;

	ComVarT<int> value; 
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
			output->OutputFmt(StringFlag_None, "	%s", exception.c_str());
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

		NetworkSession::GetCurrentNetworkSession()->parameters[0].max_party_players = value.GetVal();
		output->OutputFmt(StringFlag_None, "# maximum players set: %i", value.GetVal());
	} while (0);

	return 0;
}

int CommandCollection::WarpFixCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	ComVarT<bool> warpFixVar;

	if (Memory::IsDedicatedServer()) {
		output->Output(StringFlag_None, "# command unavailable on dedicated servers");
		return 0;
	}

	std::string exception;
	if (!warpFixVar.SetValFromStr(tokens[1], 10, exception))
	{
		output->Output(StringFlag_None, command_error_bad_arg);
		output->OutputFmt(StringFlag_None, "	%s", exception.c_str());
		return 0;
	}

	H2Tweaks::WarpFix(warpFixVar.GetVal());
	return 0;
}

int CommandCollection::DestroyObjectCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	ComVarT<int> datumIdx;

	std::string exception;
	if (!datumIdx.SetValFromStr(tokens[1], 0, exception))
	{
		output->Output(StringFlag_None, command_error_bad_arg);
		output->OutputFmt(StringFlag_None, "	%s", exception.c_str());
		return 0;
	}

	DeleteObject((datum)datumIdx.GetVal());
	return 0;
}

int CommandCollection::ReloadSpawnCommandListCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	output->Output(StringFlag_None, "# object ids reset next time spawn is used");
	readObjectIds = false;
	return 0;
}

int CommandCollection::SpawnCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;
	
	int positionArgsStartIdx = 5;

	// spawn object_name count same_team near_player x y z i j k

	datum objectDatum = DATUM_INDEX_NONE;
	ComVarT<int> count;
	ComVarT<float> pos_x, pos_y, pos_z; // position
	ComVarT<float> rot_i, rot_j, rot_k; // rotation
	ComVarT<bool> sameTeam, nearPlayerSpawn;

	int localPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0));
	datum playerUnitIdx = s_player::GetPlayerUnitDatumIndex(localPlayerIdx);

	std::string objectName = tokens[1];

	if (h2mod->GetEngineType() == e_engine_type::_main_menu) {
		output->Output(StringFlag_None, "# can only be used ingame");
		return 0;
	}
	else if (!NetworkSession::LocalPeerIsSessionHost()) {
		output->Output(StringFlag_None, "# can only be used by the session host");
		return 0;
	}
	else if (!count.SetValFromStr(tokens[2])
		|| !sameTeam.SetValFromStr(tokens[3])
		|| !nearPlayerSpawn.SetValFromStr(tokens[4])
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
			pos_x.SetVal(localPlayerPos->x + 0.5f);
			pos_y.SetVal(localPlayerPos->y + 0.5f);
			pos_z.SetVal(localPlayerPos->z + 0.5f);
		}
	}
	else
	{
		if (tokens.size() < 8
			|| !pos_x.SetValFromStr(tokens[positionArgsStartIdx++])
			|| !pos_y.SetValFromStr(tokens[positionArgsStartIdx++])
			|| !pos_z.SetValFromStr(tokens[positionArgsStartIdx++]))
		{
			output->Output(StringFlag_None, "# insufficient/invalid position spawn arguments");
			return 0;
		}
	}

	// check if rotation parameters were passed
	bool withRotation = (tokens.size() >= 12 && !nearPlayerSpawn.GetVal())
		|| (tokens.size() >= 8 && nearPlayerSpawn.GetVal());

	if (withRotation)
	{
		if (!rot_i.SetValFromStr(tokens[positionArgsStartIdx++])
			|| !rot_j.SetValFromStr(tokens[positionArgsStartIdx++])
			|| !rot_k.SetValFromStr(tokens[positionArgsStartIdx++]))
		{
			output->Output(StringFlag_None, "# insufficient/invalid rotation spawn arguments");
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
		position = { pos_x.GetVal(), pos_y.GetVal(), pos_z.GetVal() };
	}

	if (withRotation)
	{
		pRotation = &rotation;
		rotation = { rot_i.GetVal(), rot_j.GetVal(), rot_k.GetVal() };
	}

	ObjectSpawn(objectDatum, count.GetVal(), pPosition, pRotation, 1.0f, playerUnitIdx, sameTeam.GetVal());

	// output->OutputFmt(StringFlag_None, "# player xyz: %.3f %.3f %.3f", position.x, position.y, position.z);

	return 0;
}

int CommandCollection::InjectTagCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData)
{
	IOutput* output = cbData.strOutput;

	if (!NetworkSession::LocalPeerIsSessionHost() 
		&& h2mod->GetEngineType() != e_engine_type::_single_player)
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
	output->OutputFmt(StringFlag_None, "# loaded tag datum: %#X", tag_loader::ResolveNewDatum(tagDatum));

	LOG_INFO_GAME("{} - {} {} {}", tagName, tagType.as_string(), mapName);
	return 0;
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