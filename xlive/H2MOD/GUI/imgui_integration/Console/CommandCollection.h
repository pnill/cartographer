#pragma once

#include "ComVar.h"
#include "CommandHandler.h"

#include "Blam/Math/BlamMath.h"

namespace CommandCollection
{
	extern std::vector<ConsoleCommand*> commandTable;

	void InsertCommand(ConsoleCommand* newCommand);
	ConsoleVarCommand* GetVarCommandByName(const std::string& name);
	void SetVarCommandPtr(const std::string& name, IComVar* varPtr);
	void InitializeCommandsMap();

	// commands
	int DisplayXyzCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int NetworkMetricsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int SetD3D9ExStateCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int HelpCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int LogPeersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int LogPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int LeaveNetworkSessionCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int IsSessionHostCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int DownloadMapCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int LogXNetConnectionsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int LogSelectedMapFilenameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int RequestFileNameCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int ReloadMapsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int SetMaxPlayersCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int WarpFixCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int DestroyObjectCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int ReloadSpawnCommandListCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int KickPeerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int SpawnCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int InjectTagCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int BoolVarHandlerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int Crash(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int map_name(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int game_difficulty(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int game_coop_players(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int game_multiplayer(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int game_splitscreen(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int game_mode(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);

	// misc
	void ObjectSpawn(datum object_idx, int count, const real_point3d* position, const real_vector3d* rotation, datum playerIdx, float randomMultiplier, bool sameTeam);
	void ReadObjectDatumIdx();
	void DeleteObject(datum objectDatumIdx);
}
