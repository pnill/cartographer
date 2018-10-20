#pragma once
#include "Blam\Enums\Enums.h"
#include "Blam\Shared\SharedDefinitions.h"
#include "Blam\Cache\DataTypes\DataTypes.h"

struct Item
{
	short item_id;
	char pad[0x12];
}; static_assert(sizeof(Item) == 0x14, "Item size != 0x0A");

struct PlayerInventory {
	Item items[0x50];
	char pad_char[0x1380];
}; static_assert(sizeof(PlayerInventory) == 0x19C0, "PlayerInventory != 0x19C0");
/*
.text:00611CF4                 mov     ecx, [ebp+UserGSIDX]
.text:00611CF7                 imul    ecx, 19C0h
.text:00611CFD                 mov     edx, [ebp+InventorySlot]
.text:00611D00                 imul    edx, 14h
.text:00611D03                 mov     ax, PlayersPointer[ecx+edx]
.text:00611D0B                 mov     [ebp+ItemID], ax
.text:00611D0F                 movsx   ecx, [ebp+ItemID]
.text:00611D13                 test    ecx, ecx
.text:00611D15                 jle     short loc_611D23
.text:00611D17                 movsx   edx, [ebp+ItemID]
.text:00611D1B                 cmp     edx, 6590h
.text:00611D21                 jl      short loc_611D28


// Location
int X = (int)*(DWORD *)(PlayerInfo + 6592 * UID + 8);
int Y = (int)*(DWORD *)(PlayerInfo + 6592 * UID + 12);
// Basic Info
int PID = (int)*(DWORD *)(PlayerInfo + 6592 * UID + 800);
memcpy(Name, (char *)(PlayerInfo + 6592 * UID + 804), 16);
Name[16] = 0x00;
memcpy(PinNumber, (char *)(PlayerInfo + 6592 * UID + 4316), 4);
PinNumber[4] = 0x00;
memcpy(UserName, (void *)(PlayerData + 15880 * UID + 1748), 24);
int Level = (int)*(DWORD *)(PlayerInfo + 6592 * UID + 976);
// Inventory
__int64 Gold = (__int64)*(DWORD64 *)(PlayerInfo + 6592 * UID + 3136);
// Stats
byte Nation = (byte)*(byte *)(PlayerInfo + 6592 * UID + 820);
byte Class = (byte)*(byte *)(PlayerInfo + 6592 * UID + 821);
short STR = (__int16)*(__int16 *)(PlayerInfo + 6592 * UID + 824);
short DEX = (__int16)*(__int16 *)(PlayerInfo + 6592 * UID + 826);
short INT = (__int16)*(__int16 *)(PlayerInfo + 6592 * UID + 828);
short CON = (__int16)*(__int16 *)(PlayerInfo + 6592 * UID + 830);
short SPI = (__int16)*(__int16 *)(PlayerInfo + 6592 * UID + 832);
// PVP
int Honor = (int)*(DWORD *)(PlayerInfo + 6592 * UID + 860);
int Kills = (int)*(DWORD *)(PlayerInfo + 6592 * UID + 864);
int Infamy = (int)*(DWORD *)(PlayerInfo + 6592 * UID + 868);


memcpy(PinNumber, (char *)(PlayerInfo + 6592 * UID + 4316), 4);


*/


/*
struct player_info
{
	

}; static_assert(sizeof(player_info) == 0x19C0, "Size of character_data is bad!");
*/

using namespace Blam::Enums;
using namespace Blam::Cache::DataTypes;

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
				XUID xuid;
				/* Trying to convert the peer_user_index to a datum via the entity one used in voice does not work... */
				int peer_index; //0x0C
				int peer_user_index; // 0x10
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
				wchar_t PlayerName[16]; //0x40
				wchar_t UnkPad[16]; // 0x60
				Player::PrimaryArmorColor primary_armor_color; //0x80
				Player::SecondaryArmorColor secondary_armor_color; //0x81
				Player::PrimaryEmblemColor primary_emblem_color; // 0x82
				Player::SecondaryEmblemColor secondary_emblem_color; // 0x83
				Player::Biped biped_index; // 0x84
				Player::EmblemForeground emblem_foreground; //0x85
				Player::EmblemBackground emblem_background; //0x86
				Player::EmblemToggle emblem_toggle;  //0x87
				BYTE unk_pad[52]; // 0x88
				Player::Team team_index; // 0xBC
				Player::Handicap handicap; //0xBD
				__int16 rank; // 0xBE
				BYTE bootable; // 0xC0
				BYTE unk_5; // 0xC1
				BYTE unk_6; // 0xC2
				BYTE unk_7; // 0xC3
				wchar_t PlayerName2[16]; // 0xC4
				wchar_t UnkPad2[16]; // 0xE4
				BYTE unk_8; // 0x104
				BYTE unk_9; // 0x105
				BYTE unk_10; // 0x106
				BYTE unk_11; // 0x107
				BYTE unk_pad2[76]; //0x108
				int respawn_time; //0x154
				int unk_12; //0x158
				BYTE unk_pad3[36]; //0x15C
				float player_speed; //0x180
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

			struct GameStatePlayerTable
			{
				//BYTE pad[0x44];
				char tag_string[0x20]; //0x20
				int maxium_count; // 0x24
				int datum_size; // 0x28
				BYTE alignment_bit; // 0x29
				bool is_valid; // 0x2A
				WORD flags;  // 0x2C
				char data_signature[4]; // 0x30
				void **allocator; // 0x34
				struct {
					int next_index; // 0x38
					int length; // 0x3C
				}active_indices; //0x3C
				int actual_count; //0x40 
				DatumIndex next_datum; //0x44
				GameStatePlayer *players; //0x48

			};
			
		}
	}
}
