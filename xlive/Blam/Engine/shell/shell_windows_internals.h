#pragma once

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

NTSTATUS NtQueryTimerResolutionHelper(PULONG MinimumResolution, PULONG MaximumResolution, PULONG CurrentResolution);
NTSTATUS NtSetTimerResolutionHelper(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);
NTSTATUS NtWaitForSingleObjectHelper(HANDLE ObjectHandle, BOOLEAN Alertable, PLARGE_INTEGER TimeOut);