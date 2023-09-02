#pragma once

#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Engine/game/players.h"

struct s_new_hud_globals_player_info
{
	float unk_0;
	float unk_4;
	int unk_8;
	byte unk_C[76];
	void* text_chat_widget;
	byte unk_5C[36];
};
CHECK_STRUCT_SIZE(s_new_hud_globals_player_info, 128);

struct s_new_hud_engine_globals
{
	s_new_hud_globals_player_info player_data[4];
	int field_200;
	int field_204;
	datum betraying_player_datum_index;
	int gap_20C;
	s_player_profile_traits default_profile_traits;
	__int16 unk_220;
	bool show_hud;
	bool flag_20D;			// initialized to 1 but unused?
	bool flag_20E;			// initialized to 1 but unused?
	bool connected_to_live;
	byte gap_222[2];
	float hud_opacity;
	float unk_22C;
	float unk_230;
};
CHECK_STRUCT_SIZE(s_new_hud_engine_globals, 564);
s_new_hud_engine_globals* get_new_hud_engine_globals();

void set_crosshair_scale(float scale);
void new_hud_apply_patches();
void new_hud_patches_on_map_load(bool game_mode_ui_shell);
