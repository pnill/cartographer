#pragma once
#define ENABLEKANTTEST 0
#include "Networking/Transport/NetworkObserver.h"

namespace KantTesting
{
	struct __declspec(align(8)) s_session_informations_live
	{
		DWORD dwServerExists;
		DWORD time;
		char gap8[60];
		BYTE bAlways_1_1;
		BYTE bQoSDataAcquired;
		WORD unk_word;
		s_qos_probe_data qos;
		WORD wAlways_1_2;
		char gap6[6];
		int8 session_info[0xA1C];
		char gapp[24];
	};

	ASSERT_STRUCT_SIZE(s_session_informations_live, 0xAA8);

	void Initialize();
}
