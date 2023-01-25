#pragma once

// _Shell because Shell namespace causes compile errors for some reason

typedef
NTSTATUS
NTAPI
NtQueryTimerResolution(
	OUT PULONG              MinimumResolution,
	OUT PULONG              MaximumResolution,
	OUT PULONG              CurrentResolution);

typedef
NTSTATUS
NTAPI
NtSetTimerResolution(
	IN ULONG                DesiredResolution,
	IN BOOLEAN              SetResolution,
	OUT PULONG              CurrentResolution);

typedef
NTSTATUS
NTAPI
NtWaitForSingleObject(
	IN HANDLE               ObjectHandle,
	IN BOOLEAN              Alertable,
	IN PLARGE_INTEGER       TimeOut OPTIONAL);

namespace _Shell
{
	bool IsGameMinimized();
	void csmemset(char* dst, int a2, unsigned int a3);

	void Initialize();
	void FileErrorDialog(int fpErrNo);
	void OpenMessageBox(HWND hWnd, UINT uType, const char* caption, const char* format, ...);

	// time functions
	LARGE_INTEGER QPCGetStartupCounter();
	long long QPCToTime(long long denominator, LARGE_INTEGER counter, LARGE_INTEGER freq);

	long long QPCToTimeNowSec();
	long long QPCToTimeNowMsec();
	double	  QPCToSecondsPrecise(LARGE_INTEGER counter, LARGE_INTEGER freq);

	NTSTATUS NtQueryTimerResolutionHelper(PULONG MinimumResolution, PULONG MaximumResolution, PULONG CurrentResolution);
	NTSTATUS NtSetTimerResolutionHelper(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);
	NTSTATUS NtWaitForSingleObjectHelper(HANDLE ObjectHandle, BOOLEAN Alertable, PLARGE_INTEGER TimeOut);
	
	int GetInstanceId();
};
