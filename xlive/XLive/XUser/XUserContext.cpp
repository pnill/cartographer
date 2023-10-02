#include "stdafx.h"

#include "XUserContext.h"
#include "Networking/NetworkMessageTypeCollection.h"
#include "game/game_engine.h"
#include "H2MOD/Modules/Shell/H2MODShell.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"
#include "XLive/xbox/xbox.h"

extern void Check_Overlapped(PXOVERLAPPED pOverlapped);
extern XSESSION_LOCAL_DETAILS sessionDetails;

// 5289: XUserGetContext
DWORD WINAPI XUserGetContext(DWORD dwUserIndex, XUSER_CONTEXT* pContext, PXOVERLAPPED  pOverlapped)
{
	return S_OK;
}

void update_player_count()
{
	s_network_session* session = nullptr;
	if (NetworkSession::GetActiveNetworkSession(&session))
	{

	}
	else
	{
	}
}

std::string getEnglishMapName()
{
	wchar_t* p_englishMapName = Memory::GetAddress<wchar_t*>(0x97737C);
	std::wstring englishMapName(p_englishMapName);
	return  std::string(englishMapName.begin(), englishMapName.end());
}

std::string getVariantName()
{
	std::wstring variant = NetworkSession::GetGameVariantName();
	variant = variant.substr(0, variant.find_last_not_of(L"\xE008\t\n ") + 1);
	return std::string(variant.begin(), variant.end());
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

	if (Memory::IsDedicatedServer() || !H2Config_discord_enable || _Shell::GetInstanceId() > 1)
		return ERROR_SUCCESS;

	if (dwContextId == 0x00000003)
		diff_level = dwContextValue;

	if (dwContextId == X_CONTEXT_PRESENCE)
	{
		LOG_TRACE_XLIVE("- X_CONTEXT_PRESENCE = {}", dwContextValue);

		switch (static_cast<ContextPresence>(dwContextValue)) {
		case ContextPresence::singleplayer:
		{
			break;
		}
		case ContextPresence::public_game:
		case ContextPresence::invite_only_game:
		case ContextPresence::network_in_game:
		case ContextPresence::live_in_game:
		{
			update_player_count();
			break;
		}
		case ContextPresence::mainmenu:
		{
			break;
		}
		case ContextPresence::lobby:
		{
			update_player_count();
			break;
		}
		case ContextPresence::server_browser:
		{
			break;
		}
		case ContextPresence::results:
		{
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
