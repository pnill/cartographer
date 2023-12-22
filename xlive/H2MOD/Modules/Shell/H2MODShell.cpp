#include "stdafx.h"

#include "H2MODShell.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

int instanceNumber = 0;

int _Shell::GetInstanceId() {
	return instanceNumber;
}

static void InitializeInstanceId() {
	addDebugText("Determining Process Instance Number.");
	HANDLE mutex;
	DWORD lastErr;
	do {
		instanceNumber++;
		wchar_t mutexName[64];
		swprintf(mutexName, ARRAYSIZE(mutexName), (Memory::IsDedicatedServer() ? L"Halo2Server%d" : L"Halo2Player%d"), instanceNumber);
		mutex = CreateMutexW(0, TRUE, mutexName);
		lastErr = GetLastError();
		if (lastErr == ERROR_ALREADY_EXISTS) {
			CloseHandle(mutex);
		}
	} while (lastErr == ERROR_ALREADY_EXISTS);
	addDebugText("You are Instance #%d.", instanceNumber);
}

void _Shell::Initialize()
{
	// initialize game instance Id
	InitializeInstanceId();
}

void _Shell::OpenMessageBox(HWND hWnd, UINT uType, const char* caption,  const char* format, ...)
{
	va_list valist;
	va_start(valist, format);

	/* get the formatted buffer size */
	int stringLength = _vscprintf(format, valist) + 1; // +1 adds null characeter, "_vscwprintf" doesn't add it

	if (stringLength == -1)
	{
		LOG_TRACE_GAME("{} - error trying to get string length size", __FUNCTION__);
		return;
	}

	char* textBufferA = (char*)malloc(stringLength * sizeof(char));
	vsprintf(textBufferA, format, valist);

	MessageBoxA(hWnd, textBufferA, caption, uType);

	free(textBufferA);
	va_end(valist);
}

void _Shell::FileErrorDialog(int fpErrNo) 
{
	switch (fpErrNo)
	{
	case EIO:
	case EPERM:
	case EACCES:
		_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "Permission Error!", "Cannot write a file. Please restart Halo 2 in Administrator mode!");
		break;
	case ESRCH:
		_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "Permission Error!", "Probably a missing folder issue if file writing related. Please restart Halo 2 in Administrator mode!");
		break;
	default:
		_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "Unknown file failure!", "Error 0x%x", fpErrNo);
		break;
	}
}