#pragma once
#include "game/players.h"
#include "Networking/Session/NetworkSession.h"

/* structures */

struct s_hud_scripted_globals
{
	bool field_0;
	bool show_help_text;
	bool show_hud_messages;
};
ASSERT_STRUCT_SIZE(s_hud_scripted_globals, 3);

struct s_new_hud_globals_player_info
{
	real32 unk_0;
	real32 unk_4;
	int32 unk_8;
	uint8 unk_C[76];
	void* text_chat_widget;
	uint8 unk_5C[36];
};
ASSERT_STRUCT_SIZE(s_new_hud_globals_player_info, 128);

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
ASSERT_STRUCT_SIZE(s_new_hud_engine_globals, 564);

struct s_new_hud_temporary_user_state
{
	datum new_hud_tag_datum;
	int unk_4;
	int unk_8;
	int unk_c;
	int unk_10;
	int unk_14;
	int unk_18;
	int8 unk_1C;
	int8 gap_1D[3];
	float primary_auto_aim_level;
	float secondary_auto_aim_level;
	float unit_current_vitality;
	float unit_current_shield_vitality;
	int8 gap_30[4];
	int16 active_grenade_count;
	int8 gap_36[2];
	int16 fragmentation_grenade_count;
	int16 plasma_grenade_count;
	int16 current_zoom_level;
	bool current_weapon_has_zoom;
	int8 gap_3F[17];
	bool dead_unit_index_exists;
	int8 gap_51[567];
};
ASSERT_STRUCT_SIZE(s_new_hud_temporary_user_state, 0x288);

/* public code */

void new_hud_apply_patches(void);

void new_hud_patches_on_map_load(bool game_mode_ui_shell);

void should_draw_hud_override_set(bool flag);
s_new_hud_engine_globals* get_new_hud_engine_globals(void);
s_hud_scripted_globals* get_hud_scripted_globals(void);
s_new_hud_temporary_user_state* get_new_hud_temporary_user_state(int32 local_user_index);

// We scale crosshairs by adjusting the bitmap data size in the bitmap tag
// It doesn't seem to actually downscale the bitmap since the data loaded still remains the same
void set_crosshair_scale(real32 scale);

// Checks if we shouldn't draw the hud
bool new_hud_dont_draw(void);

