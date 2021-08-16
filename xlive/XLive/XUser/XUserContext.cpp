#include "XUserContext.h"

#include "XLive\xbox\xbox.h"
#include "H2MOD\Discord\DiscordInterface.h"
#include "H2MOD\Modules\Config\Config.h"

#include "H2MOD/Modules/Startup/Startup.h"
#include "H2MOD/Modules/Networking/Networking.h"

extern void Check_Overlapped(PXOVERLAPPED pOverlapped);
extern XSESSION_LOCAL_DETAILS sessionDetails;

// 5289: XUserGetContext
DWORD WINAPI XUserGetContext(DWORD dwUserIndex, XUSER_CONTEXT* pContext, PXOVERLAPPED  pOverlapped)
{
	return S_OK;
}

/* This should be cleaned up and moved to an H2MODs module instead of being defined here. */
static const std::unordered_map <std::string, std::string> singleplayer_maps
{
	{ "00a_introduction", "The Heretic" },
	{ "01a_tutorial", "The Armory" },
	{ "01b_spacestation", "Cairo Station" },
	{ "03a_oldmombasa", "Outskirts" },
	{ "03b_newmombasa", "Metropolis" },
	{ "04a_gasgiant", "The Arbiter" },
	{ "04b_floodlab", "The Oracle" },
	{ "05a_deltaapproach", "Delta Halo" },
	{ "05b_deltatowers", "Regret" },
	{ "06a_sentinelwalls", "Sacred Icon" },
	{ "06b_floodzone", "Quarantine Zone" },
	{ "07a_highcharity", "Gravemind" },
	{ "07b_forerunnership", "High Charity" },
	{ "08a_deltacliffs", "Uprising" },
	{ "08b_deltacontrol", "The Great Journey" }
};

static const std::unordered_map <int, std::pair<std::string, std::string>> campaign_difficulty_list
{
	{ 0,{ "campaign_easy", "Easy" } },
	{ 1,{ "campaign_normal", "Normal" } },
	{ 2,{ "campaign_medium", "Heroic" } },
	{ 3,{ "campaign_hard", "Legendary" } }
};

static const std::unordered_map <int, std::string> game_mode_list
{
	{ 1,"gamemode_ctf" },
	{ 2,"gamemode_slayer" },
	{ 3,"gamemode_oddball" },
	{ 4,"gamemode_KotH" },
	{ 7,"gamemode_juggernaut" },
	{ 8,"gamemode_territories" },
	{ 9,"gamemode_assault" }
};

void update_player_count()
{
	network_session* session = nullptr;
	if (NetworkSession::getCurrentNetworkSession(&session))
	{
		DiscordInterface::SetPlayerCountInfo(
			session->membership.player_count, 
			session->parameters.max_party_players);
	}
	else
	{
		DiscordInterface::SetPlayerCountInfo(0, 0);
	}
}

std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;
std::string getEnglishMapName()
{
	wchar_t* englishMapName = Memory::GetAddress<wchar_t*>(0x97737C);
	return  wstring_to_string.to_bytes(englishMapName);
}

std::string getVariantName()
{
	std::wstring variant = NetworkSession::getGameVariantName();
	variant = variant.substr(0, variant.find_last_not_of(L"\xE008\t\n ") + 1);
	return wstring_to_string.to_bytes(variant);
}

std::string gamemode_id_to_string(int id)
{
	std::string gamemode = "gamemode_unknown";
	auto it = game_mode_list.find(id);
	if (it != game_mode_list.end())
		gamemode = it->second;
	return gamemode;
}

enum class ContextPresence {
	mainmenu,
	unknown,
	server_browser,
	singleplayer,
	lobby,
	results,
	live_in_game,
	public_game,
	invite_only_game,
	network_in_game
};

DWORD diff_level;

// #5277: XUserSetContext
DWORD WINAPI XUserSetContext(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue)
{
	LOG_TRACE_XLIVE("XUserSetContext  (userIndex = {0}, contextId = {1}, contextValue = {2})",
		dwUserIndex, dwContextId, dwContextValue);

	if (Memory::isDedicatedServer() || !H2Config_discord_enable || H2GetInstanceId() > 1)
		return ERROR_SUCCESS;

	if (dwContextId == 0x00000003)
		diff_level = dwContextValue;

	if (dwContextId == X_CONTEXT_PRESENCE)
	{
		LOG_TRACE_XLIVE("- X_CONTEXT_PRESENCE = {}", dwContextValue);
		int GameGlobals = *Memory::GetAddress<int*>(0x482D3C);
		int GameEngineGlobals = *Memory::GetAddress<int*>(0x4BF8F8);

		std::wstring map_name_wide = Memory::GetAddress<wchar_t*>(0x46DAE8);
		map_name_wide = map_name_wide.substr(map_name_wide.find_last_of(L"\\") + 1);

		if(map_name_wide.empty())
		{
			std::string custom_map_wide_s = Memory::GetAddress<char*>(0x47CF0C);
			map_name_wide = std::wstring(custom_map_wide_s.begin(), custom_map_wide_s.end());
			map_name_wide = map_name_wide.substr(map_name_wide.find_last_of(L"\\") + 1);
		}

		std::string map_name = wstring_to_string.to_bytes(map_name_wide);
		LOG_TRACE_GAME(L"[Discord] map_name: {}", map_name_wide.c_str());

		switch (static_cast<ContextPresence>(dwContextValue)) {
		case ContextPresence::singleplayer:
		{
			std::wstring map_name_wide = Memory::GetAddress<wchar_t*>(0x46DD88);
			map_name_wide = map_name_wide.substr(map_name_wide.find_last_of(L"\\") + 1);

			std::string map_name = wstring_to_string.to_bytes(map_name_wide);

			std::string level_name = map_name;
			auto it = singleplayer_maps.find(map_name);
			if (it != singleplayer_maps.end())
				level_name = it->second;

			auto diff = campaign_difficulty_list.at(diff_level);
			DiscordInterface::SetGameState(
				map_name,
				"Campaign",
				level_name,
				diff.first,
				diff.second
			);
			break;
		}
		case ContextPresence::public_game:
		case ContextPresence::invite_only_game:
		case ContextPresence::network_in_game:
		case ContextPresence::live_in_game:
		{
			int game_engine_type = *reinterpret_cast<BYTE*>(GameEngineGlobals + 0xC54);

			DiscordInterface::SetGameState(
				map_name,
				"Multiplayer",
				getEnglishMapName(),
				gamemode_id_to_string(game_engine_type),
				getVariantName()
			);
			update_player_count();
			break;
		}
		case ContextPresence::mainmenu:
		{
			DiscordInterface::SetGameState("default", "In main menu");
			break;
		}
		case ContextPresence::lobby:
		{
			DiscordInterface::SetGameState("default", "In Lobby");
			update_player_count();
			break;
		}
		case ContextPresence::server_browser:
		{
			DiscordInterface::SetGameState("default", "Browsing server list");
			break;
		}
		case ContextPresence::results:
		{
			DiscordInterface::SetGameState("default", "Reading carnage report", true);
			break;
		}
		}
	}

	else if (dwContextId == X_CONTEXT_GAME_TYPE)
	{
		LOG_TRACE_XLIVE("- X_CONTEXT_GAME_TYPE = {}", dwContextValue);

		sessionDetails.dwGameType = dwContextValue;
	}

	else if (dwContextId == X_CONTEXT_GAME_MODE)
	{
		LOG_TRACE_XLIVE("- X_CONTEXT_GAME_MODE = {:x}", dwContextValue);

		sessionDetails.dwGameMode = dwContextValue;
	}



	return ERROR_SUCCESS;
}


// #5292: XUserSetContextEx
int WINAPI XUserSetContextEx(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserSetContextEx  (userIndex = {0}, contextId = {1}, contextValue = {2:x}, pOverlapped = {3:p})",
		dwUserIndex, dwContextId, dwContextValue, (void*)pOverlapped);


	//return 0;


	if (dwContextId == X_CONTEXT_PRESENCE)
	{
		LOG_TRACE_XLIVE("- X_CONTEXT_PRESENCE = {:x}", dwContextValue);
	}

	else if (dwContextId == X_CONTEXT_GAME_TYPE)
	{
		LOG_TRACE_XLIVE("- X_CONTEXT_GAME_TYPE = {:x}", dwContextValue);

		sessionDetails.dwGameType = dwContextValue;
	}

	else if (dwContextId == X_CONTEXT_GAME_MODE)
	{
		LOG_TRACE_XLIVE("- X_CONTEXT_GAME_MODE = {:x}", dwContextValue);

		sessionDetails.dwGameMode = dwContextValue;
	}



	if (pOverlapped == 0)
		return ERROR_SUCCESS;

	else
	{
		pOverlapped->InternalHigh = 0;
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped(pOverlapped);

		return ERROR_IO_PENDING;
	}
}
