#pragma once
#include "Blam/Common/Common.h"
#include <wtypes.h>

#pragma pack(push, 1)
struct s_damage_owner
{
	DWORD unk1;
	DWORD unk2;
	WORD unk3;
};
CHECK_STRUCT_SIZE(s_damage_owner, 10);
#pragma pack(pop)
