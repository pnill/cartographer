#include "Globals.h"
#include <h2mod.pb.h>

extern int weapon_one;
extern int weapon_two;
extern int weapon_three;
extern int weapon_four;
extern int weapon_five;
extern int weapon_six;
extern int weapon_seven;
extern int weapon_eight;
extern int weapon_nine;
extern int weapon_ten;
extern int weapon_eleven;
extern int weapon_tweleve;
extern int weapon_thirteen;
extern int weapon_fourteen;
extern int weapon_fiffteen;
extern int weapon_sixteen;

unsigned int Weapon_ID[36] = { 
	0xE53D2AD8, 0xE5F02B8B, 0xE6322BCD, 0xE6AF2C4A,
	0xE79B2D36,0xE8172DB2,0xE8382DD3,0xE8742E0F,
	0xE8D32E6E,0xE9062EA1,0xE90C2EA7,0xE90C2EA7,
	0xE9732F0E,0xE9F62F91,0xEAD83073,0xEB4230DD,
	0xEB9E3139,0xEC3131CC,0xEC673202,0xEC9E3239,
	0xECD63271,0xED3F32DA,0xED753310,0xEDA2333D,
	0xEDD4336F,0xEE0933A4,0xEE3433CF,0xEE5233ED,
	0xEE5F33FA,0xEE7B3416,0xEE993434,0xEE9E3439,
	0xEED3346E,0xEEF1348C,0xEF1B34B6,0xF33838D2 
};

void GunGame::Initialize()
{
	TRACE_GAME("[H2Mod-GunGame] : Initialize");
}

void GunGame::PlayerDied(int unit_datum_index) // we need to start passing player index her for sanity.
{
}

void GunGame::SpawnPlayer(int PlayerIndex)
{
}

void GunGame::LevelUp(int PlayerIndex)
{
}