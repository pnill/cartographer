#pragma once

#include "game/game_options.h"

#pragma pack(push,1)
struct s_game_state_header
{
	int alloc_checksum;
	void* base_address;
	char scenario_name[256];
	char game_build[32];
	int map_checksum;
	int field_12C;
	s_game_options options;
	int active_bsp_index;
	int field_12BC;
	int field_12C0;
	int field_12C4;
	int field_12C8;
	int field_12CC;
	int field_12D0;
	int field_12D4;
	int field_12D8;
	int field_12DC;
	int field_12E0;
	int field_12E4;
	int field_12E8;
	int field_12EC;
	int field_12F0;
	int field_12F4;
};
ASSERT_STRUCT_SIZE(s_game_state_header, 0x12F8);
#pragma pack(pop)

void __cdecl game_state_initialize(void);

void* __cdecl game_state_malloc(const char* name, const char* description, uint32 size);
