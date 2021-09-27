#pragma once

// Credits for some of the code:

// https://github.com/HaloMods/OpenSauce

// PADDING MACROS
#define J( symbol1, symbol2 ) _DO_JOIN( symbol1, symbol2 )
#define _DO_JOIN( symbol1, symbol2 ) symbol1##symbol2
// Pad the desired length of BYTES and also hides it
#define PAD(BYTES) private: BYTE J(Unused_, __LINE__ )[BYTES];  public:       

/// Add an anonymous 8-bit (1 byte) field to a structure.
#define PAD8 unsigned char : 8;
/// Add an anonymous 16-bit (2 byte) field to a structure.
#define PAD16 unsigned short : 16;
/// Add an anonymous 24-bit field to a structure.
#define PAD24 unsigned char : 8; unsigned short : 16;
/// Add an anonymous 32-bit (4 byte) field to a structure.
#define PAD32 unsigned long : 32;
/// Add an anonymous 48-bit field to a structure.
#define PAD48 unsigned short : 16; unsigned long : 32;
/// Add an anonymous 64-bit (8 byte) field to a structure.
#define PAD64 unsigned __int64 : 64;
/// Add an anonymous 128-bit (16 byte) field to a structure.
#define PAD128 unsigned __int64 : 64; unsigned __int64 : 64;

/// Use this for setting up enum bitfields
#define FLAG(bit)( 1<<(bit) )
#define TEST_FLAG(flags, bit)( ((flags) & FLAG(bit)) != 0 )
#define SET_FLAG(flags, bit, value)( (value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)) )
#define SWAP_FLAG(flags, bit)			( (flags) ^=FLAG(bit) )
#define FLAG_RANGE(first_bit, last_bit)	( (FLAG( (last_bit)+1 - (first_bit) )-1) << (first_bit) )

/// Creates a mask out of a count number of flags
#define MASK(count) ( (unsigned)(1 << (count)) - (unsigned)1 )

#define CHECK_STRUCT_SIZE(STRUCT,_SIZE)\
static_assert (sizeof(STRUCT) == _SIZE, "Invalid size for struct (" #STRUCT")")

#define CHECK_STRUCT_OFFSET(STRUCT,FIELD,OFFSET)\
static_assert (offsetof(STRUCT, FIELD) == OFFSET, #STRUCT " layout is invalid")

#define TAG_BLOCK_SIZE_ASSERT(tagblock,size)\
static_assert (sizeof(tagblock) == (size),"Invalid Size for TagBlock <" #tagblock ">")

#define TAG_GROUP_SIZE_ASSERT(tagGroup,size)\
static_assert (sizeof(tagGroup) == (size),"Invalid Size for TagGroup <" #tagGroup">");

enum e_engine_type : int
{
	SinglePlayer = 1,
	Multiplayer,
	MainMenu,
	MultiplayerShared,
	SinglePlayerShared
};

enum game_life_cycle : int
{
	life_cycle_none,
	life_cycle_pre_game,
	life_cycle_start_game,
	life_cycle_in_game,
	life_cycle_post_game,
	life_cycle_joining,
	life_cycle_matchmaking
};