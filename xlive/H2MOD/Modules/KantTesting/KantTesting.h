#pragma once
#define ENABLEKANTTEST 1
#include "Blam/Engine/Networking/Transport/NetworkObserver.h"

#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include <string>
typedef unsigned char BYTE;

std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);

#endif

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

	CHECK_STRUCT_SIZE(s_session_informations_live, 0xAA8);

	void Initialize();
}
