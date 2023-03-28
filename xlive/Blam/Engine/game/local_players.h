#pragma once
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Engine/game/players.h"

#define k_number_of_controllers 4

struct s_player_identifier
{
	DWORD unk1;
	DWORD unk2;
};
CHECK_STRUCT_SIZE(s_player_identifier, 8);

#pragma pack(push, 1)
struct s_local_player_properties
{
	bool profile_toggle;
	s_player_identifier player_identifier;
	byte pad_9[3];
	s_player::s_player_properties* player_properties_pointer;
	s_player::s_player_properties player_properties;
	DWORD player_voice;
	DWORD player_text_chat;
	DWORD unk_9C[2];
	DWORD unk_A4[2];
	DWORD unk_AC[2];
	byte session[2];
	byte pad_B6[2];

	static s_local_player_properties* get();
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(s_local_player_properties, 0xB8);

s_local_player_properties* get_local_player_properties(DWORD controller_index);

bool local_user_has_player(int user_index);
datum local_user_get_player_idx(int user_index);
