#include "stdafx.h"

#include "Shell.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

int instanceNumber = 0;
static LARGE_INTEGER startupCounter;

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

LARGE_INTEGER _Shell::QPCGetStartupCounter()
{
	return startupCounter;
}

void _Shell::Initialize()
{
	// get QPC time counter at startup
	QueryPerformanceCounter(&startupCounter);
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

NTSTATUS _Shell::NtQueryTimerResolutionHelper(PULONG MinimumResolution, PULONG MaximumResolution, PULONG CurrentResolution)
{
	static NtQueryTimerResolution* pNtQueryTimerResolution = NULL;
	if (NULL == pNtQueryTimerResolution)
	{
		pNtQueryTimerResolution = (NtQueryTimerResolution*)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryTimerResolution");
	}

	if (pNtQueryTimerResolution)
		return pNtQueryTimerResolution(MinimumResolution, MaximumResolution, CurrentResolution);

	return (NTSTATUS)0;
}

NTSTATUS _Shell::NtSetTimerResolutionHelper(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution)
{
	static NtSetTimerResolution* pNtSetTimerResolution = NULL;
	if (NULL == pNtSetTimerResolution)
	{
		pNtSetTimerResolution = (NtSetTimerResolution*)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSetTimerResolution");
	}

	if (pNtSetTimerResolution)
		return pNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);

	return (NTSTATUS)0;
}

NTSTATUS _Shell::NtWaitForSingleObjectHelper(HANDLE ObjectHandle, BOOLEAN Alertable, PLARGE_INTEGER TimeOut)
{
	static NtWaitForSingleObject* pNtWaitForSingleObject = NULL;
	if (NULL == pNtWaitForSingleObject)
	{
		pNtWaitForSingleObject = (NtWaitForSingleObject*)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWaitForSingleObject");
	}

	if (pNtWaitForSingleObject)
		return pNtWaitForSingleObject(ObjectHandle, Alertable, TimeOut);

	return (NTSTATUS)0;
}