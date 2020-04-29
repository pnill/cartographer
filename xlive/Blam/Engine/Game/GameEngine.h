#pragma once

#include "Blam/Common/Common.h"
#include "Blam/Cache//DataTypes/Macros.h"

struct game_engine_settings
{
	scnr_type map_type;
	char field_4;
	char field_5;
	BYTE gap_6[2];
	__int16 tickrate;
	BYTE gap_C[4];
	char field_10[8];
	DWORD field_18;
	BYTE gap_1C[100];
	DWORD field_80;
	DWORD field_84;
	BYTE gap_88[528];
	BYTE field_298;
	WORD field_29A;
	BYTE gap_29C[3820];
};
CHECK_STRUCT_SIZE(game_engine_settings, 0x1188);