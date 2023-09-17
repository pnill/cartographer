#pragma once
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"

struct s_new_hud_globals_player_info
{
	real32 unk_0;
	real32 unk_4;
	int32 unk_8;
	uint8 unk_C[76];
	void* text_chat_widget;
	uint8 unk_5C[36];
};
CHECK_STRUCT_SIZE(s_new_hud_globals_player_info, 128);

struct s_new_hud_engine_globals
{
	s_new_hud_globals_player_info player_data[k_number_of_users];
	int32 field_200;
	int32 field_204;
	datum betraying_player_datum_index;
	int32 gap_20C;
	s_player_profile_traits default_profile_traits;
	int16 unk_220;
	bool show_hud;
	bool flag_20D;			// initialized to 1 but unused?
	bool flag_20E;			// initialized to 1 but unused?
	bool connected_to_live;
	int8 gap_222[2];
	real32 hud_opacity;
	real32 unk_22C;
	real32 unk_230;
};
CHECK_STRUCT_SIZE(s_new_hud_engine_globals, 564);


void should_draw_hud_override_set(bool flag);
s_new_hud_engine_globals* get_new_hud_engine_globals(void);

void set_crosshair_scale(float scale);
void new_hud_apply_patches();
void new_hud_patches_on_map_load(bool game_mode_ui_shell);
