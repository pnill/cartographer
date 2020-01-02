#pragma once

enum MapType : int
{
	INVALID_MAP_TYPE = 0,
	SINGLE_PLAYER_MAP = 1,
	MULTIPLAYER_MAP = 2,
	MAIN_MENU = 3
};


struct game_engine_settings
{
	MapType map_type;
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
static_assert(sizeof(game_engine_settings) == 4488, "Invalid game_engine_settings size");
