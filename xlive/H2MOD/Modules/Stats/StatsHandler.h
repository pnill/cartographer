#pragma once

#include "Blam/Engine/Game/memory/bitstream.h"
#include "Blam/Engine/Game/networking/logic/network_life_cycle.h"
#include "Blam/Engine/Game/networking/session/network_session.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <rapidjson/document.h>

namespace StatsHandler
{
	using WValue = rapidjson::GenericValue<rapidjson::UTF16<>>;
	using WDocument = rapidjson::GenericDocument<rapidjson::UTF16<>>;

	// TODO NOTE: initalizing is disabled 
	void Initialize();
	void sendStats();
	void verifySendPlaylist();
	void sendRankChangeFromDocument(std::shared_ptr<rapidjson::Document> doc);
	
	struct StatsAPIRegisteredStatus
	{
		bool Registered = false;
		bool StatsEnabled = false;
		bool RanksEnabled = false;
	};

	StatsAPIRegisteredStatus getRegisteredStatus();

	void getPlayerRanksByStringList(std::shared_ptr<std::vector<unsigned long long>> playerList);

	void invalidateMatch(bool state);
	void playerRanksUpdateTick();
	void verifyRegistrationStatus();
	char* checkServerRegistration();
	bool serverRegistration(const char* authKey);
	const char* getAPIToken();
	int verifyPlaylist(const char* token);
	int uploadPlaylist(const char* token);
	const char* buildPostGameCarnageReportJson();
	std::string getChecksum();
	const wchar_t* getPlaylistFile();
	int uploadStats(const char* filepath, const char* token);
	void playerLeftEvent(int peerIndex);
	void playerJoinEvent(int peerIndex);
	void game_life_cycle_update_event(e_game_life_cycle state);
	void network_player_event(int peerIndex, EventHandler::NetworkPlayerEventType type);
	void server_command_event(ServerConsole::e_server_console_commands command);
};
