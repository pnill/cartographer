#pragma once
#include "game_statborg.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"

#define k_maximum_game_engine_event_responses_per_type 128

enum e_valid_multiplayer_games : short
{
	valid_multiplayer_game_capture_the_flag = FLAG(0),
	valid_multiplayer_game_slayer = FLAG(1),
	valid_multiplayer_game_oddball = FLAG(2),
	valid_multiplayer_game_king_of_the_hill = FLAG(3),
	valid_multiplayer_game_juggernaut = FLAG(4),
	valid_multiplayer_game_territories = FLAG(5),
	valid_multiplayer_game_assault = FLAG(6),
};

enum e_relevant_multiplayer_games : int
{
	relevant_multiplayer_game_capture_the_flag = FLAG(0),
	relevant_multiplayer_game_slayer = FLAG(1),
	relevant_multiplayer_game_oddball = FLAG(2),
	relevant_multiplayer_game_king_of_the_hill = FLAG(3),
	relevant_multiplayer_game_juggernaut = FLAG(4),
	relevant_multiplayer_game_territories = FLAG(5),
	relevant_multiplayer_game_assault = FLAG(6)
};

enum e_multiplayer_event_response_definition_flags : int16
{
	_multiplayer_event_response_definition_flag_quantity_message = FLAG(0)
};

enum e_multiplayer_event : int16
{
	_multiplayer_event_game_start = 0,
	_multiplayer_event_hill_controlled = 1,
	_multiplayer_event_hill_contested = 2,
	_multiplayer_event_hill_tick = 3,
	_multiplayer_event_hill_move = 4,
	_multiplayer_event_hill_controlled_team = 5,
	_multiplayer_event_hill_contested_team = 6
};

enum e_multiplayer_event_audience : int16
{
	_multiplayer_event_audience_cause_player = 0,
	_multiplayer_event_audience_cause_team = 1,
	_multiplayer_event_audience_effect_player = 2,
	_multiplayer_event_audience_effect_team = 3,
	_multiplayer_event_audience_all = 4
};

enum e_multiplayer_event_audience_type : int16
{
	_multiplayer_event_audience_type_none = 0,
	_multiplayer_event_audience_type_cause_player = 1,
	_multiplayer_event_audience_type_cause_team = 2,
	_multiplayer_event_audience_type_effect_player = 3,
	_multiplayer_event_audience_type_effect_team = 4
};

enum e_multiplayer_event_sound_flags : int16
{
	_multiplayer_event_sound_flag_announcer_sound = FLAG(0),
};

struct s_game_engine_global_player_info
{
	bool valid;
	byte pad_1[3];
	real_point3d point;
	short field_10;
	short field_12;
	byte field_14[4];
};
CHECK_STRUCT_SIZE(s_game_engine_global_player_info, 24);

struct s_game_engine_globals
{
	DWORD flags;
	short team_flags;
	WORD field_6;
	WORD field_8;
	uint16 team_bitmask;
	WORD field_C;
	short field_E;
	WORD field_10;
	WORD field_12[8];
	DWORD field_24;
	DWORD field_28;
	DWORD gap_3A[16];
	short field_6A;
	short field_6C;
	DWORD field_70;
	DWORD gap_74[28];
	real32 unk_local_player_hud_field[k_number_of_users];
	byte field_F4;
	byte pad_F5[4];
	byte gapF9[523];
	c_game_statborg game_statborg;
	s_game_engine_global_player_info player_info[k_maximum_players];
	DWORD ticks;
	BYTE gap71C[1336];
	DWORD game_engine_index;
	BYTE gapC58[132];
};
CHECK_STRUCT_SIZE(s_game_engine_globals, 0xCDC);

// max count: 1
struct s_sound_response_extra_sounds
{
	tag_reference japanese_sound;	// snd!
	tag_reference german_sound;		// snd!
	tag_reference french_sound;		// snd!
	tag_reference spanish_sound;	// snd!
	tag_reference italian_sound;	// snd!
	tag_reference korean_sound;		// snd!
	tag_reference chinese_sound;	// snd!
	tag_reference portuguese_sound;	// snd!
};
TAG_BLOCK_SIZE_ASSERT(s_sound_response_extra_sounds, 64);


// max count: 10
struct s_multiplayer_event_sound_response_definition
{
	e_multiplayer_event_sound_flags sound_flags;
	int16 pad;
	tag_reference english_sound;
	s_sound_response_extra_sounds extra_sounds;
	real32 probability;
};
TAG_BLOCK_SIZE_ASSERT(s_multiplayer_event_sound_response_definition, 80);

// max count: k_maximum_game_engine_event_responses_per_type
struct s_multiplayer_event_response_definition
{
    e_multiplayer_event_response_definition_flags flags;
    int16 pad;
    e_multiplayer_event event;
    e_multiplayer_event_audience audience;

    int16 pad1;
    int16 pad2;
    string_id display_string;
	e_multiplayer_event_audience_type required_field;
	e_multiplayer_event_audience_type excluded_audience;
    string_id primary_string;
    int32 primary_string_duration_seconds;

	string_id pluralDisplayString;
	int32 pad3[7];
    real32 sound_delay_announcer_only;

	e_multiplayer_event_sound_flags sound_flags;
	int16 pad4;
	tag_reference sound;    // snd!
	s_sound_response_extra_sounds extra_sounds;

	int32 pad5;
	int32 pad6[4];
    tag_block<s_multiplayer_event_sound_response_definition> sound_permutations;
};
TAG_BLOCK_SIZE_ASSERT(s_multiplayer_event_response_definition, 0xA8);

s_game_engine_globals* game_engine_globals_get(void);

bool __cdecl game_engine_get_change_colors(s_player_profile* player_profile, e_game_team team_index, real_rgb_color* change_colors);
