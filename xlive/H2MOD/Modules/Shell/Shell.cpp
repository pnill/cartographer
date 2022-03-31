#include "stdafx.h"

#include "Shell.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

bool __cdecl _Shell::IsGameMinimized()
{
	typedef bool(__cdecl* is_game_is_minimized_t)();
	auto p_game_is_minimized = Memory::GetAddress<is_game_is_minimized_t>(0x28729);

	if (Memory::IsDedicatedServer())
		return false;

	return p_game_is_minimized();
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

	default:
		addDebugText("Unknown file failure! error 0x%x", fpErrNo);
		_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "Unknown file failure!", "Error 0x%x", fpErrNo);
		break;
	}
}