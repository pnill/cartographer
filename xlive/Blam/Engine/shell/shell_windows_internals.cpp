
#include "stdafx.h"

#include "shell_windows_internals.h"

NTSTATUS NtQueryTimerResolutionHelper(PULONG MinimumResolution, PULONG MaximumResolution, PULONG CurrentResolution)
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

NTSTATUS NtSetTimerResolutionHelper(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution)
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

NTSTATUS NtWaitForSingleObjectHelper(HANDLE ObjectHandle, BOOLEAN Alertable, PLARGE_INTEGER TimeOut)
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