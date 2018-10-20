#include "Globals.h"
#include "xlivedefs.h"
#include "xliveless.h"
#include "xam.h"

DWORD sys_ui = -1;

HANDLE g_dwFakeListener = (HANDLE)-2;

struct NOTIFY_LISTEN
{
	HANDLE id;
	DWORD area;

	DWORD print;
};

static NOTIFY_LISTEN g_listener[50];
static int g_dwListener = 0;

int g_xlive = 0;

DWORD WINAPI sysui_timer(LPVOID lpParam)
{
	Sleep(20);



	SetEvent((HANDLE)lpParam);
	sys_ui = 2;


	TRACE("- %X = XN_SYS_UI  (signal)", (HANDLE)lpParam);


	return 0;
}

// #5270: XNotifyCreateListener
HANDLE WINAPI XNotifyCreateListener(ULONGLONG qwAreas)
{
	TRACE("XNotifyCreateListener  (0x%016x)", qwAreas);


	g_dwFakeListener = CreateMutex(NULL, NULL, NULL);



	g_listener[g_dwListener].id = g_dwFakeListener;
	g_listener[g_dwListener].area = qwAreas;
	g_listener[g_dwListener].print = 0;
	g_dwListener++;


	SetEvent(g_dwFakeListener);


	TRACE("- handle = %X", g_dwFakeListener);
	return g_dwFakeListener;
}


// #651: XNotifyGetNext
BOOL WINAPI XNotifyGetNext(HANDLE hNotification, DWORD dwMsgFilter, PDWORD pdwId, PULONG_PTR pParam)
{
	static int print_limit = 30;

	static DWORD sys_signin = 0x7FFFFFFF;
	static DWORD sys_storage = 0x7FFFFFFF;
	static DWORD sys_profile = 0x7FFFFFFF;
	static DWORD sys_controller = 0x7FFFFFFF;
	static DWORD sys_controller_force = 0x7FFFFFFF;

	static DWORD live_connection = 0x7FFFFFFF;
	static DWORD live_content = 0x7FFFFFFF;
	static DWORD live_membership = 0x7FFFFFFF;




	ResetEvent(hNotification);



	int curlist = 0;
	while (curlist < g_dwListener)
	{
		if (g_listener[curlist].id == hNotification) break;

		curlist++;
	}

	if (curlist == g_dwListener)
	{
		TRACE("XNotifyGetNext  (hNotification = %X, dwMsgFilter = %X, pdwId = %X, pParam = %X)",
			hNotification, dwMsgFilter, pdwId, pParam);

		TRACE("- unknown notifier");

		return 0;
	}



	if ((g_listener[curlist].area & ((XNID_AREA(dwMsgFilter) << 1) | 1)) == 0)
	{
		TRACE("XNotifyGetNext  (hNotification = %X, dwMsgFilter = %X, pdwId = %X, pParam = %X)",
			hNotification, dwMsgFilter, pdwId, pParam);

		TRACE("- bad area: %X ~ %X", g_listener[curlist].area, (XNID_AREA(dwMsgFilter) << 1) | 1);

		return 0;
	}



	// reset logger
	if (sys_ui == -1)
	{
		sys_ui = 0;
		g_listener[curlist].print = 0;
	}



	if (g_listener[curlist].print < print_limit)
	{
		TRACE("XNotifyGetNext  (hNotification = %X, dwMsgFilter = %X, pdwId = %X, pParam = %X)",
			hNotification, dwMsgFilter, pdwId, pParam);


		g_listener[curlist].print++;
	}



	EnterCriticalSection(&d_lock);


	BOOL exit_code = FALSE;

	if (pdwId) *pdwId = dwMsgFilter;




	// set to next available message
	if ((g_listener[curlist].area & XNOTIFY_SYSTEM) &&
		dwMsgFilter == 0)
	{
		if (0) {}

		else if (sys_ui == 0 || sys_ui == 2)
			dwMsgFilter = XN_SYS_UI;

		else if (sys_signin == 0)
			dwMsgFilter = XN_SYS_SIGNINCHANGED;

		// ex. GTA IV - recheck DLC containers
		else if (sys_storage == 0)
			dwMsgFilter = XN_SYS_STORAGEDEVICESCHANGED;

		else if (sys_profile == 0)
			dwMsgFilter = XN_SYS_PROFILESETTINGCHANGED;

		else if (sys_controller == 0)
			dwMsgFilter = XN_SYS_INPUTDEVICESCHANGED;

		else if (sys_controller_force == 0)
			dwMsgFilter = XN_SYS_INPUTDEVICECONFIGCHANGED;
	}


	if ((g_listener[curlist].area & XNOTIFY_LIVE) &&
		dwMsgFilter == 0)
	{
		if (live_connection == 0)
			dwMsgFilter = XN_LIVE_CONNECTIONCHANGED;

		else if (live_content == 0)
			dwMsgFilter = XN_LIVE_CONTENT_INSTALLED;

		else if (live_membership == 0)
			dwMsgFilter = XN_LIVE_MEMBERSHIP_PURCHASED;

	}

	if ((g_listener[curlist].area & XNOTIFY_CUSTOM) &&
		dwMsgFilter == 0)
	{
		if (0) // change this to if player kicked...
		{
			dwMsgFilter = XN_CUSTOM_ACTIONPRESSED;
		}
	}


	if (dwMsgFilter == 0)
	{
		LeaveCriticalSection(&d_lock);

		return FALSE;
	}



	switch (dwMsgFilter)
	{

	case XN_CUSTOM_ACTIONPRESSED:
		*pdwId = XN_CUSTOM_ACTIONPRESSED;
		*pParam = 0x00000000; // replace the high-bits with action_id
		exit_code = TRUE;
		break;

	case XN_SYS_UI:
		if (sys_ui == 0)
		{
			// show UI
			if (pParam) *pParam = 1;


			DWORD threadid;

			sys_ui++;
			CreateThread(NULL, 0, &sysui_timer, (LPVOID)hNotification, NULL, &threadid);



			TRACE("- %X = XN_SYS_UI (1)", hNotification);

			exit_code = TRUE;
		}

		else if (sys_ui == 2)
		{
			// hide UI
			if (pParam) *pParam = 0;

			sys_ui++;


			TRACE("- %X = XN_SYS_UI (0)", hNotification);

			exit_code = TRUE;




			sys_signin = 0;
			sys_storage = 0;
			sys_profile = 0;
			sys_controller = 0;
			sys_controller_force = 0;

			live_connection = 0;
			live_content = 0;
			live_membership = 0;
		}
		break;


	case XN_SYS_SIGNINCHANGED:
		if (sys_signin == 0)
		{
			*pdwId = XN_SYS_SIGNINCHANGED;
			if (pParam)
			{
				*pParam = 0;


				// player 1-4
				if (g_signin[0]) *pParam |= 1;
				if (g_signin[1]) *pParam |= 2;
				if (g_signin[2]) *pParam |= 4;
				if (g_signin[3]) *pParam |= 8;
			}


			sys_signin++;


			TRACE("- %X = XN_SYS_SIGNINCHANGED (1) - %08X", hNotification, XN_SYS_SIGNINCHANGED);

			exit_code = TRUE;
		}
		break;


	case XN_SYS_STORAGEDEVICESCHANGED:
		if (sys_storage == 0)
		{
			sys_storage++;


			TRACE("- %X = XN_SYS_STORAGEDEVICESCHANGED (-)", hNotification);

			exit_code = TRUE;
		}
		break;


	case XN_SYS_PROFILESETTINGCHANGED:
		if (sys_profile == 0)
		{
			if (pParam)
			{
				*pParam = 0;


				// player 1-4
				if (g_signin[0]) *pParam |= 1;
				if (g_signin[1]) *pParam |= 2;
				if (g_signin[2]) *pParam |= 4;
				if (g_signin[3]) *pParam |= 8;
			}


			sys_profile++;


			TRACE("- %X = XN_SYS_PROFILESETTINGCHANGED (1)", hNotification);

			exit_code = TRUE;
		}
		break;


	case XN_SYS_INPUTDEVICESCHANGED:
		if (sys_controller == 0)
		{
			sys_controller++;


			TRACE("- %X = XN_SYS_INPUTDEVICESCHANGED (-)", hNotification);

			exit_code = TRUE;
		}
		break;


	case XN_SYS_INPUTDEVICECONFIGCHANGED:
		if (sys_controller_force == 0)
		{
			if (pParam)
			{
				*pParam = 0;


				// player 1-4
				if (g_signin[0]) *pParam |= 1;
				if (g_signin[1]) *pParam |= 2;
				if (g_signin[2]) *pParam |= 4;
				if (g_signin[3]) *pParam |= 8;
			}


			sys_controller_force++;


			TRACE("- %X = XN_SYS_INPUTDEVICECONFIGCHANGED (1)", hNotification);

			exit_code = TRUE;
		}
		break;


	case XN_LIVE_CONNECTIONCHANGED:
		if (live_connection == 0)
		{
			live_connection++;

			*pdwId = XN_LIVE_CONNECTIONCHANGED;

			if (pParam) *pParam = XONLINE_S_LOGON_CONNECTION_ESTABLISHED;


			TRACE("- %X = XN_LIVE_CONNECTIONCHANGED (0) - %08X", hNotification, XN_LIVE_CONNECTIONCHANGED);

			exit_code = TRUE;
		}
		break;


	case XN_LIVE_CONTENT_INSTALLED:
		if (live_content == 0)
		{
			live_content++;


			TRACE("- %X = XN_LIVE_CONTENT_INSTALLED (-)", hNotification);

			exit_code = TRUE;
		}
		break;


	case XN_LIVE_MEMBERSHIP_PURCHASED:
		if (live_membership == 0)
		{
			live_membership++;


			TRACE("- %X = XN_LIVE_MEMBERSHIP_PURCHASED (-)", hNotification);

			exit_code = TRUE;
		}
		break;


	default:
		break;
	} // switch



	  // check for more messages
	if (exit_code)
		SetEvent(hNotification);


	LeaveCriticalSection(&d_lock);
	return exit_code;
}



// #652: XNotifyPositionUI
DWORD WINAPI XNotifyPositionUI(DWORD dwPosition)
{
	TRACE("XNotifyPositionUI (%d)", dwPosition);
	return 0;
}
// #653
int WINAPI XNotifyDelayUI(int a1)
{
	//TRACE("XNotifyDelayUI");
	return 0;
}
