#pragma once
#include "new_hud_definitions.h"

#include "game/game_engine_territories.h"
#include "game/players.h"
#include "math/color_math.h"

/* enums */

enum e_model_group_targets_flags : __int32
{
	e_model_group_targets_flags_locked_by_human_tracking = 0x1,
	e_model_group_targets_flags_locked_by_plasma_tracking = 0x2,
	e_model_group_targets_flags_headshot = 0x4,
	e_model_group_targets_flags_vulnerable = 0x8,
	e_model_group_targets_flags_alwas_locked_by_plasma_tracking = 0x10,
};


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
	real32 field_0;
	real32 field_4;
	int32 field_8;
	uint8 gap_C[76];
	void* text_chat_widget;
	uint8 gap_5C[36];
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
	real32 primary_auto_aim_level;
	real32 secondary_auto_aim_level;
	real32 unit_current_vitality;
	real32 unit_current_shield_vitality;
	int8 gap_30[4];
	int16 active_grenade_count;
	int8 gap_36[2];
	int16 fragmentation_grenade_count;
	int16 plasma_grenade_count;
	int16 current_zoom_level;
	bool current_weapon_has_zoom;
	int8 gap_3F[17];
	bool dead_unit_index_exists;
	int8 gap_51[7];
	e_model_group_targets_flags crosshair_flags;
	int8 gap_5C[225];
	bool game_is_coop;
	int8 gap_13E[86];
	datum local_player_datum;
	real_rgb_color local_player_color;
	int8 gap_198[60];
	datum other_player_datum;
	real_rgb_color other_player_color;
	int8 gap_1E4[62];
	bool player_talking;
	int8 gap_22F;
	int8 gap_230;
	int8 gap_231;
	datum player_index;
	uint32 territories_count;
	pixel32 territory_pixel_color[k_maximum_territories_flags];
	real32 territory_control_progress[k_maximum_territories_flags];
	int8 gap_260[10];
};
ASSERT_STRUCT_SIZE(s_new_hud_temporary_user_state, 0x288);

/* public code */

void new_hud_apply_patches(void);

void should_draw_hud_override_set(bool flag);
s_new_hud_engine_globals* get_new_hud_engine_globals(void);
s_new_hud_globals_player_info* __cdecl new_hud_engine_globals_get_player_data(int32 local_player_index);
void __cdecl new_hud_engine_globals_set_drawing_player_index(datum player_datum);
s_hud_scripted_globals* get_hud_scripted_globals(void);
s_new_hud_temporary_user_state* get_new_hud_temporary_user_state(int32 local_user_index);

real_rgb_color* get_local_user_hud_color(uint32 local_user_index);

void __cdecl new_hud_widget_anchor_calculate_point(int32 anchor, real_point2d* out_point);

uint32 __cdecl new_hud_text_get_split_screen_font_type(e_hud_anchor anchor);

// Checks if we shouldn't draw the hud
bool new_hud_dont_draw(void);

