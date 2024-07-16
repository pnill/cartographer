#pragma once

struct s_transport_qos_attempt
{
	int16 field_0;
	int16 xnqos_count;
	XNQOS* xnqos;
	uint8 gap_8[24];
};
ASSERT_STRUCT_SIZE(s_transport_qos_attempt, 32);

struct s_qos_attempt_data
{
	uint32 cProbesXmit;
	uint32 cProbesRecv;
	uint32 wRttMinInMsecs;
	uint32 wRttMedInMsecs;
	uint32 dwUpBitsPerSec;
	uint32 dwDnBitsPerSec;
	uint32 cbData;
	void* pbData;
};
ASSERT_STRUCT_SIZE(s_qos_attempt_data, 32);

datum __cdecl transport_qos_target_new_hook(int a1, int a2, int a3, int a4);
