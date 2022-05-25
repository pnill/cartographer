#include "stdafx.h"

#include "Shell.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

long long _Shell::QPCToTime(long long denominator, LARGE_INTEGER counter, LARGE_INTEGER freq)
{
	long long _Whole, _Part;

	_Whole = (counter.QuadPart / freq.QuadPart) * denominator;
	_Part = (counter.QuadPart % freq.QuadPart) * denominator / freq.QuadPart;

	return _Whole + _Part;
}

long long _Shell::QPCToTimeNowSec()
{
	LARGE_INTEGER freq;
	LARGE_INTEGER currentCounter;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&currentCounter);
	return QPCToTime(1, currentCounter, freq);
}

long long _Shell::QPCToTimeNowMsec()
{
	LARGE_INTEGER freq;
	LARGE_INTEGER currentCounter;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&currentCounter);
	return QPCToTime(std::milli::den, currentCounter, freq);
}

double _Shell::QPCToSecondsPrecise(LARGE_INTEGER counter, LARGE_INTEGER freq)
{
	return static_cast<double>((double)QPCToTime(std::micro::den, counter, freq) / (double)std::micro::den);
}

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
		break;
	default:
		_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "Unknown file failure!", "Error 0x%x", fpErrNo);
		break;
	}
}