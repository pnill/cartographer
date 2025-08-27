#pragma once
#include "saved_games/player_profile.h"
#include "memory/bitstream.h"
#include "saved_games/game_variant.h"


/* enums */

/* structures */

// practically the xuid, but aligned to 4 (hence the split in 2 32 bit variables)
struct s_player_identifier
{
	uint32 id_low;
	uint32 id_high;
};
ASSERT_STRUCT_SIZE(s_player_identifier, 8);

struct s_clan_identifiers
{
	uint32 ID_1;
	uint32 ID_2;
	uint32 ID_3;
};
ASSERT_STRUCT_SIZE(s_clan_identifiers, 12);

struct s_player_properties
{
	wchar_t player_name[XUSER_NAME_SIZE];
	int32 spawn_protection_time;
	int8 gap_24[28];

	s_player_profile_traits profile_traits;
	wchar_t clan_name[16];
	s_clan_identifiers clan_identifiers;

	int8 team_index;
	int8 player_handicap_level;	// e_user_interface_controller_handicap
	int8 player_displayed_skill;
	int8 player_overall_skill;
	int8 player_is_griefer;
	int8 bungie_user_role;
	int8 achievement_flags;
	int8 unk2;
};
ASSERT_STRUCT_SIZE(s_player_properties, 132);

/* public code */
void network_game_definitions_apply_patches();

void __cdecl network_game_definitions_encode_game_variant(c_bitstream* packet, s_game_variant* variant);
bool __cdecl network_game_definitions_decode_game_variant(c_bitstream* packet, s_game_variant* variant);