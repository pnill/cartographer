#include "stdafx.h"
#include "XUserContext.h"

#include "Globals.h"
#include "xliveless.h"
#include "resource.h"
#include "XLive\xbox\xbox.h"
#include "XLive\XAM\xam.h"
#include "H2MOD\Discord\DiscordInterface.h"
#include "H2MOD\Modules\Config\Config.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <unordered_map>
#include "xlivedefs.h"

extern void Check_Overlapped(PXOVERLAPPED pOverlapped);

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
	int player_count_a = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x506974);
	int player_count_b = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x50E4FC);
	int max_player_count_a = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x50A3A0);
	int max_player_count_b = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x511F28);

	DiscordInterface::SetPlayerCountInfo(
		player_count_a ? player_count_a : player_count_b,
		max_player_count_a ? max_player_count_a : max_player_count_b
	);
}

std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;
std::string getEnglishMapName()
{
	wchar_t* englishMapName = (wchar_t*)(h2mod->GetBase() + 0x97737C);
	return  wstring_to_string.to_bytes(englishMapName);
}

std::string getVariantName()
{
	std::wstring variant = reinterpret_cast<wchar_t*>(h2mod->GetBase() + 0x97777C);
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
	TRACE("XUserSetContext  (userIndex = %d, contextId = %d, contextValue = %d)",
		dwUserIndex, dwContextId, dwContextValue);

	if (h2mod->Server || !H2Config_discord_enable || H2GetInstanceId() > 1)
		return ERROR_SUCCESS;

	if (dwContextId == 0x00000003)
		diff_level = dwContextValue;

	if (dwContextId == X_CONTEXT_PRESENCE)
	{
		TRACE("- X_CONTEXT_PRESENCE = %d", dwContextValue);
		int GameGlobals = *reinterpret_cast<int*>(h2mod->GetBase() + 0x482D3C);
		int GameEngineGlobals = *reinterpret_cast<int*>(h2mod->GetBase() + 0x4BF8F8);

		std::wstring map_name_wide = (wchar_t*)(h2mod->GetBase() + 0x46DAE8);
		map_name_wide = map_name_wide.substr(map_name_wide.find_last_of(L"\\") + 1);

		std::string map_name = wstring_to_string.to_bytes(map_name_wide);
		TRACE_GAME("[Discord] map_name: %ws", map_name_wide.c_str());

		switch (static_cast<ContextPresence>(dwContextValue)) {
		case ContextPresence::singleplayer:
		{
			std::wstring map_name_wide = (wchar_t*)(h2mod->GetBase() + 0x46DD88);
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
		TRACE("- X_CONTEXT_GAME_TYPE = %d", dwContextValue);

		sessionDetails.dwGameType = dwContextValue;
	}

	else if (dwContextId == X_CONTEXT_GAME_MODE)
	{
		TRACE("- X_CONTEXT_GAME_MODE = %X", dwContextValue);

		sessionDetails.dwGameMode = dwContextValue;
	}



	return ERROR_SUCCESS;
}


// #5292: XUserSetContextEx
int WINAPI XUserSetContextEx(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue, PXOVERLAPPED pOverlapped)
{
	TRACE("XUserSetContextEx  (userIndex = %d, contextId = %d, contextValue = %X, pOverlapped = %X)",
		dwUserIndex, dwContextId, dwContextValue, pOverlapped);


	//return 0;


	if (dwContextId == X_CONTEXT_PRESENCE)
	{
		TRACE("- X_CONTEXT_PRESENCE = %X", dwContextValue);
	}

	else if (dwContextId == X_CONTEXT_GAME_TYPE)
	{
		TRACE("- X_CONTEXT_GAME_TYPE = %X", dwContextValue);

		sessionDetails.dwGameType = dwContextValue;
	}

	else if (dwContextId == X_CONTEXT_GAME_MODE)
	{
		TRACE("- X_CONTEXT_GAME_MODE = %X", dwContextValue);

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
