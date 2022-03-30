#pragma once

#include "H2MOD.h"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "Blam\Engine\Memory\bitstream.h"
#include "Blam\Engine\Networking\Session\NetworkSession.h"
#include "rapidjson\document.h"
#include "rapidjson\prettywriter.h"
#include <rapidjson\document.h>

class StatsHandler
{
public:
	StatsHandler::StatsHandler() = delete;
	StatsHandler::StatsHandler(const StatsHandler& other) = delete;
	StatsHandler::StatsHandler(StatsHandler&& other) = delete;

	static void sendStats()
	{
		if (Memory::IsDedicatedServer()
			&& RegisteredStatus().Registered && RegisteredStatus().StatsEnabled) 
		{
			auto token = getAPIToken();
			if (token != nullptr) {
				int verifyPlaylistResponse = verifyPlaylist(token);
				if (verifyPlaylistResponse == 201)
				{
					if (uploadPlaylist(token) == 200) {

						auto filepath = buildJSON();
						if (strcmp(filepath, ""))
						{
							if (uploadStats(filepath, token) == 200)
							{
								remove(filepath);
								LOG_TRACE_GAME("[H2MOD] Stats uploaded successfully");
							}
							else
							{
								LOG_ERROR_GAME("[H2MOD] Stats Uploading encountered an error");
							}
						}
						else
						{
							LOG_ERROR_GAME("[H2MOD] Stats Json failed to build");
						}

					}
					else
					{
						LOG_ERROR_GAME("[H2MOD] Playlist Uploading encountered an error");
					}
				}
				else if (verifyPlaylistResponse == 500 || verifyPlaylistResponse == -1)
				{
					LOG_ERROR_GAME("[H2MOD] Playlist Verification encountered a server error");
				}
			}
		}
	}

	static void verifySendPlaylist()
	{
		if (Memory::IsDedicatedServer() && RegisteredStatus().Registered)
		{
			auto token = getAPIToken();
			int verifyPlaylistResponse = verifyPlaylist(token);
			if (verifyPlaylistResponse == 500 || verifyPlaylistResponse == -1)
			{
				LOG_ERROR_GAME("[H2MOD] Playlist Verification encountered a server error");
			}
			else if (verifyPlaylistResponse == 201)
			{
				if (uploadPlaylist(token) != 200) {
					LOG_ERROR_GAME("[H2MOD] Playlist Uploading encountered an error");
				}
			}
		}
	}

	static void sendRankChangeFromDocument(rapidjson::Document* document);
	
	struct StatsAPIRegisteredStatus
	{
		bool Registered = false;
		bool StatsEnabled = false;
		bool RanksEnabled = false;
	};

	static std::string buildPlayerRankUpdateQueryStringList();
	static void StatsHandler::getPlayerRanksByStringList(std::string& playerList);

	static void init();
	static void InvalidateMatch(bool state);
	static void playerRanksUpdateTick();
	static void verifyRegistrationStatus();
	static StatsAPIRegisteredStatus RegisteredStatus();
	static char* checkServerRegistration();
	static bool serverRegistration(char* authKey);
	static char* getAPIToken();
	static bool serverLogin();
	static int verifyPlaylist(char* token);
	static int uploadPlaylist(char* token);
	typedef rapidjson::GenericDocument<rapidjson::UTF16<>> WDocument;
	typedef rapidjson::GenericValue<rapidjson::UTF16<>> WValue;
	static char* buildJSON();
	static std::string getChecksum();
	static wchar_t* getPlaylistFile();
	static int uploadStats(char* filepath, char* token);
	static void playerLeftEvent(int peerIndex);
	static void playerJoinEvent(int peerIndex);
	static void Initialize();
	static void game_life_cycle_update_event(e_game_life_cycle state);
	static void network_player_event(int peerIndex, EventHandler::NetworkPlayerEventType type);
	static void server_command_event(ServerConsole::ServerConsoleCommands command);
};
