#pragma once

#include "ComVar.h"
#include "CommandHandler.h"
#include "CommandCollection.h"

namespace CommandCollection
{
	extern std::vector<ConsoleCommand*> commandTable;

	void InsertCommand(ConsoleCommand* newCommand);
	ConsoleVarCommand* GetVarCommandByName(const std::string& name);
	void SetVarCommandPtr(const std::string& name, ComVar* varPtr);
	void InitializeCommandsMap();

	// commands
	int KickPeerCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int BoolVarHandler(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int DownloadMapCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int ReloadMapsCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
	int SetNetMetricStateCmd(const std::vector<std::string>& tokens, ConsoleCommandCtxData cbData);
}
