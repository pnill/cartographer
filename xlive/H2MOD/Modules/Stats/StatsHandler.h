#pragma once
#include "H2MOD/Modules/Console/ConsoleCommands.h"
#include "H2MOD.h"

class StatsHandler
{
public:
	StatsHandler();
	static void sendStats()
	{
		if (h2mod->Server) {
			int verifyPlaylistResponse = verifyPlaylist();
			if (verifyPlaylistResponse == 500 || verifyPlaylistResponse == -1)
			{
				LOG_ERROR_GAME(L"[H2MOD] Playlist Verification encountered a server error");
			}
			else
			{
				bool result = true;
				if (verifyPlaylistResponse == 201)
				{
					if (uploadPlaylist() != 200) {
						result = false;
						LOG_ERROR_GAME(L"[H2MOD] Playlist Uploading encountered an error");
					}
				}
				if (result)
				{
					auto filepath = buildJSON();
					if (strcmp(filepath, ""))
					{
							
					} 
					else
					{
						LOG_INFO_GAME(L"[H2MOD] Stats Json failed to build");
					}
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
