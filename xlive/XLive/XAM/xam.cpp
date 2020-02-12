#include "stdafx.h"

#include "xam.h"
#include "H2MOD/Modules/Console/ConsoleCommands.h"

//extern ConsoleCommands* commands;

struct NOTIFY_LISTEN
{
	HANDLE id;
	ULONGLONG area;
	DWORD print;
	CRITICAL_SECTION lock;
};

static int g_dwListener = 0;
static NOTIFY_LISTEN g_listener[50];
extern int update_signin_ui_state_index;

// #5270: XNotifyCreateListener
HANDLE WINAPI XNotifyCreateListener(ULONGLONG qwAreas)
{
	int listenerIndex = g_dwListener++;
	g_listener[listenerIndex].print = 0;
	g_listener[listenerIndex].area = qwAreas;
	g_listener[listenerIndex].id = CreateMutex(NULL, NULL, NULL);

	InitializeCriticalSection(&g_listener[listenerIndex].lock);

	LOG_TRACE_XLIVE("XNotifyCreateListener({:x}), ", qwAreas);
	LOG_TRACE_XLIVE(" - handle: {:p}", (void*)g_listener[listenerIndex].id);
	SetEvent(g_listener[listenerIndex].id);

	return g_listener[listenerIndex].id;
}

// #651: XNotifyGetNext
BOOL WINAPI XNotifyGetNext(HANDLE hNotification, DWORD dwMsgFilter, PDWORD pdwId, PULONG_PTR pParam)
{
	static DWORD print_limit = 30;

	static DWORD sys_storage = 0x7FFFFFFF;
	static DWORD sys_profile = 0x7FFFFFFF;
	static DWORD sys_controller = 0x7FFFFFFF;
	static DWORD sys_controller_force = 0x7FFFFFFF;

	static DWORD live_connection = 0x7FFFFFFF;
	static DWORD live_content = 0x7FFFFFFF;
	static DWORD live_membership = 0x7FFFFFFF;

	static bool last_ui_state = false;

	int listenerIndex = 0;
	for (; listenerIndex < g_dwListener; listenerIndex++)
	{
		if (g_listener[listenerIndex].id == hNotification)
			break;
	}

	if (listenerIndex >= g_dwListener)
	{
		LOG_TRACE_XLIVE("XNotifyGetNext  (hNotification = {0:p}, dwMsgFilter = {1:x}, pdwId = {2:p}, pParam = {3:p}) - unknown notifier",
			(void*)hNotification, dwMsgFilter, (void*)pdwId, (void*)pParam);

		return FALSE;
	}

	ResetEvent(hNotification);

	if ((g_listener[listenerIndex].area & ((XNID_AREA(dwMsgFilter) << 1) | 1)) == 0)
	{
		LOG_TRACE_XLIVE("XNotifyGetNext  (hNotification = {0:p}, dwMsgFilter = {1:x}, pdwId = {2:x}, pParam = {3:p})",
			(void*)hNotification, dwMsgFilter, (void*)pdwId, (void*)pParam);

		LOG_TRACE_XLIVE("- bad area: {0:x} ~ {1:x}", g_listener[listenerIndex].area, (XNID_AREA(dwMsgFilter) << 1) | 1);

		return FALSE;
	}

	if (g_listener[listenerIndex].print < print_limit)
	{
		LOG_TRACE_XLIVE("XNotifyGetNext  (hNotification = {0:p}, dwMsgFilter = {1:x}, pdwId = {2:p}, pParam = {3:p})",
			(void*)hNotification, dwMsgFilter, (void*)pdwId, (void*)pParam);


		g_listener[listenerIndex].print++;
	}

	EnterCriticalSection(&g_listener[listenerIndex].lock);

	// set to next available message
	if (g_listener[listenerIndex].area & XNOTIFY_SYSTEM &&
		dwMsgFilter == 0)
	{
		if (commands->consoleOpen() != last_ui_state)
		{
			last_ui_state = commands->consoleOpen();
			dwMsgFilter = XN_SYS_UI;
		}

		else if (signInStatusChanged())
			dwMsgFilter = XN_SYS_SIGNINCHANGED;

		/* Hacky fix */
		else if (update_signin_ui_state_index++ <= 2)
			dwMsgFilter = XN_SYS_UI;

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


	if ((g_listener[listenerIndex].area & XNOTIFY_LIVE) &&
		dwMsgFilter == 0)
	{
		if (live_connection == 0)
			dwMsgFilter = XN_LIVE_CONNECTIONCHANGED;

		else if (live_content == 0)
			dwMsgFilter = XN_LIVE_CONTENT_INSTALLED;

		else if (live_membership == 0)
			dwMsgFilter = XN_LIVE_MEMBERSHIP_PURCHASED;

	}

/*	if ((g_listener[curlist].area & XNOTIFY_CUSTOM) &&
		dwMsgFilter == 0)
	{
		if (0) // change this to if player kicked...
		{
			dwMsgFilter = XN_CUSTOM_ACTIONPRESSED;
		}
	}*/


	if (dwMsgFilter == 0)
	{
		LeaveCriticalSection(&g_listener[listenerIndex].lock);

		return FALSE;
	}

	if (pdwId)
		*pdwId = dwMsgFilter;

	BOOL exit_code = FALSE;

	switch (dwMsgFilter)
	{

	/*case XN_CUSTOM_ACTIONPRESSED:
		*pdwId = XN_CUSTOM_ACTIONPRESSED;
		*pParam = 0x00000000; // replace the high-bits with action_id
		exit_code = TRUE;
		break;*/

	case XN_SYS_UI:
		
		if (pParam)
		{
			if (update_signin_ui_state_index == 1)
			{
				*pParam = true;
				update_signin_ui_state_index++;
			}
			else if (update_signin_ui_state_index == 2)
			{
				*pParam = false;
				update_signin_ui_state_index++;
			}
			else
			{
				*pParam = commands->consoleOpen();
			}

			LOG_TRACE_XLIVE("- {:p} = XN_SYS_UI (1)", (void*)hNotification);

			exit_code = TRUE;
		}
		
		break;

	case XN_SYS_SIGNINCHANGED:
		if (pParam)
		{
			*pParam = 0;

			for (int i = 0; i < 4; i++)
			{
				if (signInChanged[i])
				{
					*pParam |= 1 << i;
					signInChanged[i] = false;
				}
			}
				
			if (*pParam) 
			{
				if (userSignedIn(0))
				{
					sys_storage = 0;
					sys_profile = 0;
					sys_controller = 0;
					sys_controller_force = 0;

					live_connection = 0;
					live_content = 0;
					live_membership = 0;
				}
				exit_code = TRUE;
			}
		}

		LOG_TRACE_XLIVE("- {0:p} = XN_SYS_SIGNINCHANGED (1)", (void*)hNotification);

		break;


	case XN_SYS_STORAGEDEVICESCHANGED:
		if (sys_storage == 0)
		{
			sys_storage++;
			LOG_TRACE_XLIVE("- {:p} = XN_SYS_STORAGEDEVICESCHANGED (-)", (void*)hNotification);

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
				for (int i = 0; i < 4; i++)
				{
					if (userSignedIn(i))
						*pParam |= 1 << i;
				}

				if (*pParam)
				{
					sys_profile++;
					exit_code = TRUE;
				}
			}

			LOG_TRACE_XLIVE("- {:p} = XN_SYS_PROFILESETTINGCHANGED (1)", (void*)hNotification);

		}
		break;


	case XN_SYS_INPUTDEVICESCHANGED:
		if (sys_controller == 0)
		{
			sys_controller++;


			LOG_TRACE_XLIVE("- {:p} = XN_SYS_INPUTDEVICESCHANGED (-)", (void*)hNotification);

			exit_code = TRUE;
		}
		break;


	case XN_SYS_INPUTDEVICECONFIGCHANGED:
		if (sys_controller_force == 0)
		{
			if (pParam)
			{
				*pParam = 0;

				for (int i = 0; i < 4; i++)
				{
					if (userSignedIn(i)) {
						
						*pParam |= 1 << i;
					}
				}
				if (*pParam)
				{
					sys_controller_force++;
					exit_code = TRUE;
				}
			}

			LOG_TRACE_XLIVE("- {:p} = XN_SYS_INPUTDEVICECONFIGCHANGED (1)", (void*)hNotification);

		}
		break;


	case XN_LIVE_CONNECTIONCHANGED:
		if (live_connection == 0)
		{
			live_connection++;

			if (pParam)
			{
				exit_code = TRUE;
				*pParam = XONLINE_S_LOGON_CONNECTION_ESTABLISHED;
			}

			LOG_TRACE_XLIVE("- {0:p} = XN_LIVE_CONNECTIONCHANGED (0) - {1:x}", (void*)hNotification, XN_LIVE_CONNECTIONCHANGED);
		}
		break;


	case XN_LIVE_CONTENT_INSTALLED:
		if (live_content == 0)
		{
			live_content++;

			LOG_TRACE_XLIVE("- {:p} = XN_LIVE_CONTENT_INSTALLED (-)", (void*)hNotification);

			exit_code = TRUE;
		}
		break;


	case XN_LIVE_MEMBERSHIP_PURCHASED:
		if (live_membership == 0)
		{
			live_membership++;


			LOG_TRACE_XLIVE("- {:p} = XN_LIVE_MEMBERSHIP_PURCHASED (-)", (void*)hNotification);

			exit_code = TRUE;
		}
		break;


	default:
		break;
	} // switch



	  // check for more messages
	if (exit_code)
		SetEvent(hNotification);


	LeaveCriticalSection(&g_listener[listenerIndex].lock);
	return exit_code;
}

// #652: XNotifyPositionUI
DWORD WINAPI XNotifyPositionUI(DWORD dwPosition)
{
	LOG_TRACE_XLIVE("XNotifyPositionUI ({})", dwPosition);
	return 0;
}

// #653
int WINAPI XNotifyDelayUI(int a1)
{
	//LOG_TRACE_XLIVE("XNotifyDelayUI");
	return 0;
}
