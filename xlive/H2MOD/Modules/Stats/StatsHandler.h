#pragma once
#include "H2MOD/Modules/Console/ConsoleCommands.h"
#include "H2MOD.h"
#include <rapidjson/document.h>
#include "H2MOD/Modules/Networking/NetworkSession/NetworkSession.h"
#include "H2MOD/Modules/Networking/CustomPackets/CustomPackets.h"

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
						if(uploadStats(filepath) == 200)
						{
							//Stats upload success
						} 
						else
						{
							LOG_ERROR_GAME(L"[H2MOD] Stats Uploading encountered an error");
						}
					} 
					else
					{
						LOG_ERROR_GAME(L"[H2MOD] Stats Json failed to build");
					}
				}

			}
		}
	}
	static void sendRankChange(bool forceAll = false)
	{
		if (NetworkSession::localPeerIsSessionHost())
		{
			auto document = getPlayerRanks(forceAll);
			if (document.MemberCount() == 0)
			{
				LOG_ERROR_GAME(L"[H2MOD] failed to retrieve player ranks");
			}
			else
			{
				for (auto i = 0; i < document.MemberCount(); i++)
				{
					//LOG_INFO_GAME(document[i]["XUID"].GetString());
					//LOG_INFO_GAME(document[i]["Rank"].GetString());
					std::string::size_type sz = 0;
					long long xuid = std::stoll(document[i]["XUID"].GetString(), &sz, 0);
					int peer = NetworkSession::getPeerIndexFromXUID(xuid);
					LOG_INFO_GAME(std::to_string(peer));
					if(peer != NetworkSession::getLocalPeerIndex())
					{
						byte rank = std::stoi(document[i]["Rank"].GetString(), nullptr);
						LOG_INFO_GAME(std::to_string(rank));
						CustomPackets::sendRankChange(peer, rank);
					}
				}
			}
		}
	}
	static rapidjson::Document getPlayerRanks(bool forceAll = false);
	static int verifyPlaylist();
	static int uploadPlaylist();
	static char* buildJSON();
	static std::string getChecksum();
	static wchar_t* getPlaylistFile();
	static int uploadStats(char* filepath);
};
