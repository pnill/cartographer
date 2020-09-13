#pragma once

#include "Blam/Common/Common.h"
#include "Blam/Engine/DataArray/DataArray.h"
#include "Blam/Engine/Objects/Objects.h"

#define ENGINE_PLAYER_MAX 16

#pragma pack(push, 1)
struct Player
{
	enum class Color : BYTE
	{
		White,
		Steel,
		Red,
		Orange,
		Gold,
		Olive,
		Green,
		Sage,
		Cyan,
		Teal,
		Colbat,
		Blue,
		Violet,
		Purple,
		Pink,
		Crimson,
		Brown,
		Tan
	};

	enum class Biped : BYTE
	{
		MasterChief = 0,
		Dervish = 1,
		Spartan = 2,
		Elite = 3
	};

	enum class EmblemForeground : BYTE
	{
		SeventhColumn = 0,
		Bullseye = 1,
		Vortex = 2,
		Halt = 3,
		Spartan = 4,
		DaBomb = 5,
		Trinity = 6,
		Delta = 7,
		Rampancy = 8,
		Sergeant = 9,
		Phenoix = 10,
		Champion = 11,
		JollyRoger = 12,
		Marathon = 13,
		Cube = 14,
		Radioactive = 15,
		Smiley = 16,
		Frowney = 17,
		Spearhead = 18,
		Sol = 19,
		Waypoint = 20,
		YingYang = 21,
		Helmet = 22,
		Triad = 23,
		GruntSymbol = 24,
		Cleave = 25,
		Thor = 26,
		SkullKing = 27,
		Triplicate = 28,
		Subnova = 29,
		FlamingNinja = 30,
		DoubleCresent = 31,
		Spades = 32,
		Clubs = 33,
		Diamonds = 34,
		Hearts = 35,
		Wasp = 36,
		MarkOfShame = 37,
		Snake = 38,
		Hawk = 39,
		Lips = 40,
		Capsule = 41,
		Cancel = 42,
		GasMask = 43,
		Grenade = 44,
		Tsanta = 45,
		Race = 46,
		Valkyire = 47,
		Drone = 48,
		Grunt = 49,
		GruntHead = 50,
		BruteHead = 51,
		Runes = 52,
		Trident = 53,
		Number0 = 54,
		Number1 = 55,
		Number2 = 56,
		Number3 = 57,
		Number4 = 58,
		Number5 = 59,
		Number6 = 60,
		Number7 = 61,
		Number8 = 62,
		Number9 = 63
	};

	enum class EmblemBackground : BYTE
	{
		Solid = 0,
		VerticalSplit = 1,
		HorizontalSplit1 = 2,
		HorizontalSplit2 = 3,
		VerticalGradient = 4,
		HorizontalGradient = 5,
		TripleColumn = 6,
		TripleRow = 7,
		Quadrants1 = 8,
		Quadrants2 = 9,
		DiagonalSlice = 10,
		Cleft = 11,
		X1 = 12,
		X2 = 13,
		Circle = 14,
		Diamond = 15,
		Cross = 16,
		Square = 17,
		DualHalfCircle = 18,
		Triangle = 19,
		DiagonalQuadrant = 20,
		ThreeQuaters = 21,
		Quarter = 22,
		FourRows1 = 23,
		FourRows2 = 24,
		SplitCircle = 25,
		OneThird = 26,
		TwoThirds = 27,
		UpperField = 28,
		TopandBottom = 29,
		CenterStripe = 30,
		LeftandRight = 31
	};

	enum class EmblemToggle : BYTE
	{
		Off = 0,
		On = 1
	};

	enum class Handicap : BYTE
	{
		None = 0,
		Minor = 1,
		Moderate = 2,
		Severe = 3
	};

	struct Properties
	{
		wchar_t player_name[16];
		char unk[32];

		struct PlayerProfile
		{
			Color primary_color;
			Color secondary_color;
			Color tertiary_color;
			Color quaternary_color;
			Biped player_character_type;
			EmblemForeground foreground_emblem;
			EmblemBackground background_emblem;
			char emblem_flags;
		} profile;

		BYTE gap48[8];
		wchar_t clan_name[16];
		struct {
			DWORD ID_1;
			DWORD ID_2;
			DWORD ID_3;
		} clan_identifiers;

		e_object_team player_team;
		Handicap player_handicap_level;
		Handicap player_displayed_skill;
		char player_overall_skill;
		char player_is_griefer;
		char bungie_user_role;
		char achievement_flags;
		char unk2;
	};
	CHECK_STRUCT_SIZE(Properties, 132);

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
	Properties properties; // 0x40
	Properties properties_2;
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

	/*
	- TO NOTE: 
	- This functions work only after game has started, if you need to do something in the pregame lobby, use the functions available in Network Session (H2MOD/Modules/Networking/NetworkSession)
	*/

	static s_datum_array* getArray();
	static bool indexValid(int playerIndex);
	static Player* getPlayer(int playerIndex);
	static e_object_team getTeam(int playerIndex);
	static void setTeam(int playerIndex, e_object_team team);
	static void setUnitBipedType(int playerIndex, Player::Biped bipedType);
	static void setBipedSpeed(int playerIndex, float speed);
	static wchar_t* getName(int playerIndex);
	static datum getPlayerUnitDatumIndex(int playerIndex);
	static XUID getIdentifier(int playerIndex);
};
CHECK_STRUCT_SIZE(Player, 0x204);
#pragma pack(pop)

class PlayerIterator : public DatumIterator<Player>
{
public:

	PlayerIterator();

	bool get_next_player();

	Player* get_current_player_data();

	int get_current_player_index();

	wchar_t* get_current_player_name();

private:
	Player* m_current_player = nullptr;
};