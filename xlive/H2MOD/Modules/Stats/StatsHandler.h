#pragma once
#include "H2MOD/Modules/Console/ConsoleCommands.h"
#include "H2MOD.h"

class StatsHandler
{
public:
	StatsHandler();
	static void sendStats()
	{
		LOG_INFO_GAME("Starting Stats Colection");
		if (h2mod->Server) {
			int verifyPlaylistResponse = verifyPlaylist();
			if (verifyPlaylistResponse == 500 || verifyPlaylistResponse == -1)
			{
				LOG_INFO_GAME(L"[H2MOD] Playlist Verification encountered a server error");
			}
			else
			{
				LOG_INFO_GAME("Playlist Verified");
				bool result = true;
				if (verifyPlaylistResponse == 201)
				{
					if (uploadPlaylist() != 200) {
						result = true;
						LOG_INFO_GAME(L"[H2MOD] Playlist Uploading encountered an error");
					}
				}
				if (result)
				{
					LOG_INFO_GAME("Dumping Stats");
					buildJSON();
				}

			}
		}
	}
	static int verifyPlaylist();
	static int uploadPlaylist();
	static char* buildJSON();
	static std::string getChecksum();
	static wchar_t* getPlaylistFile();
};
