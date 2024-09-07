#include "stdafx.h"
#include "xbox.h"
#include "interface/screens/screen_cartographer_account_manager.h"

extern void Check_Overlapped(PXOVERLAPPED pOverlapped);

// #5260: XShowSigninUI
int WINAPI XShowSigninUI(DWORD cPanes, DWORD dwFlags)
{
	if (!UserSignedIn(0))
	{
		cartographer_account_manager_open_list();
	}

	return ERROR_SUCCESS;
}

// #5215: XShowGuideUI
int WINAPI XShowGuideUI(DWORD dwUserIndex)
{
	extern void* ui_load_cartographer_guide_menu(); // Custom menu Guide Popup from H2MOD
	ui_load_cartographer_guide_menu();

	return 0;
}


DWORD WINAPI XShowArcadeUI(DWORD dwUserIndex)
{
	return 0;
}

// 5299: XShowGuideKeyRemapUI
DWORD WINAPI XShowGuideKeyRemapUI(DWORD dwUserIndex)
{
	return S_OK;
}

// #5275: XShowFriendsUI
int WINAPI XShowFriendsUI(DWORD dwUserIndex)
{
	LOG_TRACE_XLIVE("XShowFriendsUI");
	return 0;
}

// #5271: XShowPlayersUI
int WINAPI XShowPlayersUI(DWORD dwUserIndex)
{
	LOG_TRACE_XLIVE("XShowPlayersUI");
	return 1;
}

// #5006: XShowMessagesUI
int WINAPI XShowMessagesUI(DWORD dwUserIndex)
{
	LOG_TRACE_XLIVE("XShowMessagesUI");
	return 1;   // ERROR_NOT_LOGGED_ON
}

// #5209: XShowMessageComposeUI
int WINAPI XShowMessageComposeUI(DWORD dwUserIndex, void * pXuidRecepients, DWORD cRecipients, void * wszText)
{
	LOG_TRACE_XLIVE("XShowMessageComposeUI");
	return 1;   // ERROR_NOT_LOGGED_ON
}

// #5208: XShowGameInviteUI
int WINAPI XShowGameInviteUI(DWORD dwUserIndex, void * pXuidRecipients, DWORD cRecipients, LPCWSTR pszText)
{
	LOG_TRACE_XLIVE("XShowGameInviteUI");
	return 1; // ERROR_NOT_LOGGED_ON
}

// #5210: XShowFriendRequestUI
int WINAPI XShowFriendRequestUI(DWORD dwUserIndex, XUID xuidUser)
{
	LOG_TRACE_XLIVE("XShowFriendRequestUI");
	return 1;
}

// #5212: XShowCustomPlayerListUI
DWORD WINAPI XShowCustomPlayerListUI(DWORD dwUserIndex, DWORD dwFlags, LPCWSTR pzsTitle, LPCWSTR pszDescription, CONST BYTE *pbImage, DWORD cbImage, CONST XPLAYERLIST_USER *rgPlayers, DWORD cPlayers, CONST XPLAYERLIST_BUTTON *pXButton, CONST XPLAYERLIST_BUTTON *pYButton, XPLAYERLIST_RESULT *pResult, XOVERLAPPED *pOverlapped)
{
	if (pOverlapped) {
		//asynchronous

		pOverlapped->InternalLow = ERROR_SUCCESS;

		Check_Overlapped(pOverlapped);

		return ERROR_IO_PENDING;
	}
	else {
		//synchronous
		//return result;
	}
	return ERROR_SUCCESS;
}

// #5216: XShowKeyboardUI
DWORD WINAPI XShowKeyboardUI(DWORD dwUserIndex, DWORD dwFlags, LPCWSTR wseDefaultText, LPCWSTR wszTitleText, LPCWSTR wszDescriptionText, LPWSTR wszResultText, DWORD cchResultText, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE(L"XShowKeyboardUI  (dwUserIndex = {0}, dwFlags = {1:x}, wseDefaultText = {2}, wszTitleText = {3}, wszDescriptionText = {4}, wszResultText = {5:p}, cchResultText = {6:x}, pOverlapped = {7:p})",
		dwUserIndex, dwFlags, wseDefaultText, wszTitleText, wszDescriptionText, (void*)wszResultText, cchResultText, (void*)pOverlapped);
	return ERROR_SUCCESS;
}

// #5252: XShowGamerCardUI
int WINAPI XShowGamerCardUI(DWORD dwUserIndex,XUID XuidPlayer)
{
	LOG_TRACE_XLIVE("XShowGamerCardUI");
	return 0;
}

// #5250
DWORD WINAPI XShowAchievementsUI(DWORD dwUserIndex)
{
	LOG_TRACE_XLIVE("XShowAchievementsUI");
	return 0;
}

// #5365: XShowMarketplaceUI
DWORD WINAPI XShowMarketplaceUI(DWORD dwUserIndex, DWORD dwEntryPoint, ULONGLONG dwOfferId, DWORD dwContentCategories)
{
	LOG_TRACE_XLIVE("XShowMarketplaceUI");
	return 1;
}

// 5366 XShowMarketplaceDownloadItemsUI
DWORD WINAPI XShowMarketplaceDownloadItemsUI(DWORD dwUserIndex, DWORD dwEntryPoint, CONST ULONGLONG* pOfferIDs, DWORD dwOfferIdCount, HRESULT *phrResult, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XShowMarketplaceDownloadItemsUI");

	// not done - error now
	return 0x57;
}

// 5375: XMarketplaceGetImageUrl
DWORD WINAPI XMarketplaceGetImageUrl(DWORD dwTitleId, ULONGLONG qwOfferID, DWORD cchStringBuffer, WCHAR *pwszStringBuffer)
{
	return 0;
}

// 5374: XMarketplaceGetDownloadStatus
DWORD WINAPI XMarketplaceGetDownloadStatus(DWORD dwUserIndex, ULONGLONG qwOfferID, LPDWORD pdwResult /* out*/)
{
	return 0;
}

// #5266: XShowMessageBoxUI
DWORD WINAPI XShowMessageBoxUI(DWORD dwUserIndex, LPCWSTR wszTitle, LPCWSTR wszText, DWORD cButtons, LPCWSTR *pwszButtons,
	DWORD dwFocusButton, DWORD dwFlags, MESSAGEBOX_RESULT *pResult, XOVERLAPPED *pOverlapped)
{
	LOG_TRACE_XLIVE(L"XShowMessageBoxUI  ({0} = {1})", wszTitle, wszText);
	return ERROR_SUCCESS;
}

// #5214: XShowPlayerReviewUI
int WINAPI XShowPlayerReviewUI(DWORD dwUserIndex,XUID XuidFeedbackTarget)
{
	LOG_TRACE_XLIVE("XShowPlayerReviewUI");
	return 0;
}
