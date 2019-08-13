#pragma once

#include "Blam\Enums\Enums.h"
#include "Blam\Shared\SharedDefinitions.h"
#include "Blam\Cache\DataTypes\DataTypes.h"

using namespace Blam::Enums;
using namespace Blam::Cache::DataTypes;

struct Item
{
	short item_id;
	char pad[0x12];
}; static_assert(sizeof(Item) == 0x14, "Item size != 0x0A");

struct PlayerInventory {
	Item items[0x50];
	char pad_char[0x1380];
}; static_assert(sizeof(PlayerInventory) == 0x19C0, "PlayerInventory != 0x19C0");

#pragma pack(push, 1)
struct PlayerProperties
{
	wchar_t player_name[16];
	int spawn_protection_time;
	char unk[28];

	struct PlayerProfile
	{
		Player::Color primary_color;
		Player::Color secondary_color;
		Player::Color tertiary_color;
		Player::Color quaternary_color;
		Player::Biped player_caracter_type;
		Player::EmblemForeground foreground_emblem;
		Player::EmblemBackground background_emblem;
		char emblem_flags;
	} profile;

	BYTE gap48[8];
	wchar_t clan_identifier_name[16];
	struct {
		DWORD ID_1;
		DWORD ID_2;
		DWORD ID_3;
	} clan_identifiers;

	enum Team : BYTE
	{
		RED,
		BLUE
	};
	Player::Team player_team;
	Player::Handicap player_handicap_level;
	Player::Handicap player_displayed_skill;
	char player_overall_skill;
	char player_is_griefer;
	char bungie_user_role;
	char achievement_flags;
};
#pragma pack(pop)

namespace Blam 
{
	namespace EngineDefinitions
	{
		namespace Players
		{	
#pragma pack(push, 1) 
			struct GameStatePlayer //size:0x204
			{
				DWORD UnkPlayerDatum; //0x00
				XUID xuid; //0x04
				/* Trying to convert the peer_user_index to a datum via the entity one used in voice does not work... */
				DWORD peer_index; //0x0C
				DWORD peer_user_index; // 0x10
				/* These are based on the beta and can be wrong/off. */
				int machine_index; // 0x14  
				int machine_controller_index; //0x18
				int unk_1; // 0x1C
				int controller_index; // 0x20
				__int16 user_index; // 0x24;
				__int16 related_to_pos_on_bsp; // 0x26
				DatumIndex unit_index; // 0x28 //0x24 in h2x beta
				DatumIndex DeadGameStateDatum; //0x2C
				DatumIndex PossiblyDatum; // 0x30
				DWORD InputFlags; // 0x34
				DWORD InputFlags2; // 0x38
				DWORD unk_4; // 0x3C
				PlayerProperties properties; // 0x40
				BYTE pad1;
				PlayerProperties player_properties_2;
				char pad[13];
				int respawn_time; //0x154
				int unk_12; //0x158
				BYTE unk_pad3[36]; //0x15C
				float unit_speed; //0x180
				DatumIndex player_aimed_at; //0x184
				int unk_13; // 0x188
				int unk_14_related_to_player_aimed_at; // 0x18C
				__int16 unk_15; //0x190
				__int16 unk_16; //0x192
				DatumIndex betraying_player; //0x194
				BYTE unk_pad4[0x68]; //0x1FC
				int is_chatting; // 0x200
			};
#pragma pack(pop)
			static_assert(sizeof(GameStatePlayer) == 0x204, "Invalid GameStatePlayer size");	
		}
	}
}
