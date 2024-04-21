#pragma once

struct s_qos_attempt_data
{
	DWORD cProbesXmit;
	DWORD cProbesRecv;
	DWORD wRttMinInMsecs;
	DWORD wRttMedInMsecs;
	DWORD dwUpBitsPerSec;
	DWORD dwDnBitsPerSec;
	DWORD cbData;
	DWORD pbData;
};
ASSERT_STRUCT_SIZE(s_qos_attempt_data, 32);

datum __cdecl transport_qos_target_new_hook(int a1, int a2, int a3, int a4);
