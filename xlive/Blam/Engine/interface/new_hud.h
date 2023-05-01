#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"

struct s_new_hud_globals_player_info
{
	byte player_info[128];
};

struct s_new_hud_globals
{
	s_new_hud_globals_player_info player_info[4];
	DWORD field_200;
	DWORD field_204;
	datum betraying_player_datum_index;
	byte gap_20C[28];
	float hud_opacity;
	byte gap_22C[8];
};

void toggle_hud(bool state);
void new_hud_apply_patches();