#pragma once

#include "Blam\Enums\Enums.h"
#include "Blam\Cache\DataTypes\DataTypes.h"
#include "H2MOD\Modules\DataArray\DataArray.h"

using namespace Blam::Enums;

struct Item
{
	short item_id;
	char pad[0x12];
}; 
static_assert(sizeof(Item) == 0x14, "Item size != 0x14");

struct PlayerInventory {
	Item items[0x50];
	char pad_char[0x1380];
}; 
static_assert(sizeof(PlayerInventory) == 0x19C0, "PlayerInventory != 0x19C0");

#pragma pack(push, 1)
struct PlayerProperties
{
	wchar_t player_name[16];
	char unk[32];

	struct PlayerProfile
	{
		Player::Color primary_color;
		Player::Color secondary_color;
		Player::Color tertiary_color;
		Player::Color quaternary_color;
		Player::Biped player_character_type;
		Player::EmblemForeground foreground_emblem;
		Player::EmblemBackground background_emblem;
		char emblem_flags;
	} profile;

	BYTE gap48[8];
	wchar_t clan_name[16];
	struct {
		DWORD ID_1;
		DWORD ID_2;
		DWORD ID_3;
	} clan_identifiers;

	Player::Team player_team;
	Player::Handicap player_handicap_level;
	Player::Handicap player_displayed_skill;
	char player_overall_skill;
	char player_is_griefer;
	char bungie_user_role;
	char achievement_flags;
	char unk2;
};
static_assert(sizeof(PlayerProperties) == 132, "PlayerProperties size != 132 bytes");
#pragma pack(pop)

extern s_datum_array* game_state_players;

namespace Blam 
{
	namespace EngineDefinitions
	{
		namespace Players
		{
#pragma pack(push, 1)
			struct GameStatePlayer //size:0x204
			{
				WORD DatumSalt; //0x00
				BYTE Flags; // 0x02
				BYTE unk; // 0x03
				XUID xuid; //0x04
				/* Trying to convert the peer_user_index to a datum via the entity one used in voice does not work... */
				DWORD peer_index; //0x0C
				BYTE abNet[6]; // 0x10 used to identify the peer index
				/* These are based on the beta and can be wrong/off. */
				short machine_index; // 0x14  
				int machine_controller_index; //0x18
				int unk_1; // 0x1C
				int controller_index; // 0x20
				__int16 user_index; // 0x24;
				__int16 related_to_pos_on_bsp; // 0x26
				datum BipedUnitDatum; // 0x28 //0x24 in h2x beta
				datum DeadGameStateDatum; //0x2C
				datum PossiblyDatum; // 0x30
				DWORD InputFlags; // 0x34
				DWORD InputFlags2; // 0x38
				DWORD unk_4; // 0x3C
				PlayerProperties properties; // 0x40
				PlayerProperties properties_2;
				char pad[12];
				int respawn_time; //0x154
				int unk_12; //0x158
				BYTE unk_pad3[36]; //0x15C
				float unit_speed; //0x180
				datum player_aimed_at; //0x184
				int unk_13; // 0x188
				int unk_14_related_to_player_aimed_at; // 0x18C
				__int16 unk_15; //0x190
				__int16 unk_16; //0x192
				datum betraying_player; //0x194
				int field_198; //0x198
				int field_19C; //0x19C 
				BYTE unk_pad4[0x60]; //0x1FC
				int is_chatting; // 0x200
			};
			static_assert(sizeof(GameStatePlayer) == 0x204, "Invalid GameStatePlayer size");	
#pragma pack(pop)

			// use while in game
			class PlayerIterator : public DatumIterator<GameStatePlayer>
			{
			public:

				PlayerIterator() :
					DatumIterator(game_state_players)
				{

				}

				bool get_next_player()
				{
					m_current_player = get_next_datum();
					if (m_current_player)
					{
						do
						{
							if (!(m_current_player->Flags & 2))
								break;

							m_current_player = get_next_datum();

						} while (m_current_player);
					}

					return m_current_player != nullptr;
				}

				GameStatePlayer* get_current_player_data()
				{
					return m_current_player;
				}

				int get_current_player_index()
				{
					return get_current_absolute_index();
				}

				wchar_t* get_current_player_name()
				{
					return m_current_player->properties.player_name;
				}

			private:
				GameStatePlayer* m_current_player = nullptr;
			};
		}
	}
}


