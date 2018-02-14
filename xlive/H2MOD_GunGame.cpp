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

class ResetGunGameLevelHandler : public PlayerIterableHandler {
	void onPlayerIterated(Player* player) {
		player->setGunGameLevel(0);
	}
};

class GunGameSpawnHandler : public PlayerIterableHandler {
public:
	GunGameSpawnHandler(int PlayerIndex, GunGame* gg) {
		this->PlayerIndex = PlayerIndex;
		this->gg = gg;
	}

	void onPlayerIterated(Player* player) {
		if (player->getPlayerIndex() == PlayerIndex)
		{
			int level = player->getGunGameLevel();
			TRACE_GAME("[H2Mod-GunGame]: SpawnPlayer found : %s level: %i", player->getPlayerName().c_str(), level);

			int CurrentWeapon = gg->level_weapon[level];

			if (level == 15)
			{
				TRACE_GAME("[H2Mod-GunGame]: %s on frag grenade level!", player->getPlayerName().c_str());
				gg->set_unit_grenades(GrenadeType::Frag, 99, player->getPeerIndex(), 1);
			}

			if (level == 16)
			{
				TRACE_GAME("[H2Mod-GunGame]: %s on plasma grenade level!", player->getPlayerName().c_str());
				gg->set_unit_grenades(GrenadeType::Plasma, 99, player->getPeerIndex(), 1);
			}

			if (level < 15)
			{
				GivePlayerWeapon(PlayerIndex, CurrentWeapon, 1);
			}
			this->stopIteration = true;
		}
	}
private:
	int PlayerIndex;
	GunGame* gg;
};

class GetGunGameLevelHandler : public PlayerIterableHandler {
public:
	GetGunGameLevelHandler(int* level, int PlayerIndex) {
		this->level = level;
	}
	void onPlayerIterated(Player* player) {
		if (player->getPlayerIndex() == PlayerIndex)
		{
			*this->level = player->getGunGameLevel();
			this->stopIteration = true;
		}
	}
private:
	int* level;
	int PlayerIndex;
};

void GunGame::resetPlayerLevels() {
	ResetGunGameLevelHandler* gunGameHandler = new ResetGunGameLevelHandler();
	players->iteratePlayers(gunGameHandler);
	delete gunGameHandler;
}

class IncreaseGunGameLevelHandler : public PlayerIterableHandler {
public:
	IncreaseGunGameLevelHandler(int PlayerIndex, GunGame* gg) {
		this->PlayerIndex = PlayerIndex;
		this->gg = gg;
	}
	void onPlayerIterated(Player* player) {
		if (player->getPlayerIndex() == PlayerIndex)
		{
			TRACE_GAME("[H2Mod-GunGame]: LevelUp(%i) : %s", PlayerIndex, player->getPlayerName().c_str());
			int level = player->getGunGameLevel();
			TRACE_GAME("[H2Mod-GunGame]: LevelUp() - Found %s in GunGamePlayers map level: %i", player->getPlayerName().c_str(), level);
			level++;
			player->setGunGameLevel(level);

			TRACE_GAME("[H2Mod-GunGame]: LevelUp(%i) - new level: %i ", PlayerIndex, level);

			if (level == 15)
			{
				TRACE_GAME("[H2Mod-GunGame]: %s Level 15 - Frag Grenades!", player->getPlayerName().c_str());
				gg->set_unit_grenades(GrenadeType::Frag, 99, player->getPeerIndex(), 1);
			}

			if (level == 16)
			{
				TRACE_GAME("[H2Mod-GunGame]: %s Level 16 - Plasma Grenades!", player->getPlayerName().c_str());
				gg->set_unit_grenades(GrenadeType::Plasma, 99, player->getPeerIndex(), 1);
			}

			if (level < 15)
			{
				TRACE_GAME("[H2Mod-GunGame]: %s on level %i giving them weapon...", player->getPlayerName().c_str(), level);

				int LevelWeapon = gg->level_weapon[level];
				gg->set_unit_grenades(GrenadeType::Frag, 0, player->getPeerIndex(), 1);
				gg->set_unit_grenades(GrenadeType::Plasma, 0, player->getPeerIndex(), 1);
				GivePlayerWeapon(PlayerIndex, LevelWeapon, 1);
			}
			this->stopIteration = true;
		}
	}
private:
	GunGame* gg;
	int PlayerIndex;
};

class PlayerDiedHandler : public PlayerIterableHandler {
public:
	PlayerDiedHandler(int pIndex, GunGame* gg) {
		this->pIndex = pIndex;
		this->gg = gg;
	}

	void onPlayerIterated(Player* player) {
		if (this->pIndex = player->getPlayerIndex()) {
			gg->set_unit_grenades(GrenadeType::Frag, 0, player->getPeerIndex(), 1);
			gg->set_unit_grenades(GrenadeType::Plasma, 0, player->getPeerIndex(), 1);
			stopIteration = true;
		}
	}

private:
	GunGame* gg;
	int pIndex;
};

void GunGame::Initialize()
{
	TRACE_GAME("[H2Mod-GunGame] : Initialize()");
	
	if (weapon_one != 0)
	{
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one: %i", weapon_one);
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one enum:  %08X", Weapon_ID[weapon_one]);
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_two enum:  %08X", Weapon_ID[weapon_two]);
		this->level_weapon[0] = Weapon_ID[weapon_one];
		this->level_weapon[1] = Weapon_ID[weapon_two];
		this->level_weapon[2] = Weapon_ID[weapon_three];
		this->level_weapon[3] = Weapon_ID[weapon_four];
		this->level_weapon[4] = Weapon_ID[weapon_five];
		this->level_weapon[5] = Weapon_ID[weapon_six];
		this->level_weapon[6] = Weapon_ID[weapon_seven];
		this->level_weapon[7] = Weapon_ID[weapon_eight];
		this->level_weapon[8] = Weapon_ID[weapon_nine];
		this->level_weapon[9] = Weapon_ID[weapon_ten];
		this->level_weapon[10] = Weapon_ID[weapon_eleven];
		this->level_weapon[11] = Weapon_ID[weapon_tweleve];
		this->level_weapon[12] = Weapon_ID[weapon_thirteen];
		this->level_weapon[13] = Weapon_ID[weapon_fourteen];
		this->level_weapon[14] = Weapon_ID[weapon_fiffteen];
	
	
	}
	else
	{
		this->level_weapon[0] = Weapon::energy_blade_useless;
		this->level_weapon[1] = Weapon::needler;
		this->level_weapon[2] = Weapon::plasma_pistol;
		this->level_weapon[3] = Weapon::magnum;
		this->level_weapon[4] = Weapon::smg;
		this->level_weapon[5] = Weapon::plasma_rifle;
		this->level_weapon[6] = Weapon::brute_plasma_rifle;
		this->level_weapon[7] = Weapon::juggernaut_powerup;
		this->level_weapon[8] = Weapon::shotgun;
		this->level_weapon[9] = Weapon::brute_shot;
		this->level_weapon[10] = Weapon::covenant_carbine;
		this->level_weapon[11] = Weapon::battle_rifle;
		this->level_weapon[12] = Weapon::beam_rifle;
		this->level_weapon[13] = Weapon::sniper_rifle;
		this->level_weapon[14] = Weapon::rocket_launcher;
	}

	resetPlayerLevels();
}

void GunGame::PlayerDied(int unit_datum_index) // we need to start passing player index her for sanity.
{
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		call_unit_reset_equipment(unit_datum_index);

		int pIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);
		PlayerDiedHandler* gunGameHandler = new PlayerDiedHandler(pIndex, this);
		players->iteratePlayers(gunGameHandler);
		delete gunGameHandler;
	}
}

void GunGame::NextRound()
{
	//TODO:
}

void GunGame::SpawnPlayer(int PlayerIndex)
{
	int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);
	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object)
	{
		GunGameSpawnHandler* gunGameHandler = new GunGameSpawnHandler(PlayerIndex, this);
		players->iteratePlayers(gunGameHandler);
		delete gunGameHandler;
	}
}

int GunGame::GetCurrentWeapon(int PlayerIndex)
{
	TRACE("[H2Mod-GunGame]: GetCurrentWeapon(%i)", PlayerIndex);
	
	int level = 0;
	GetGunGameLevelHandler* gunGameHandler = new GetGunGameLevelHandler(&level, PlayerIndex);
	players->iteratePlayers(gunGameHandler);
	delete gunGameHandler;

	TRACE("[H2Mod-GunGame]: GetCurrentWeapon - player_level: %i", level);
	TRACE("[H2Mod-GunGame]: level_weapon-> %08X", this->level_weapon[level]);

	return this->level_weapon[level];
}

void GunGame::LevelUp(int PlayerIndex)
{
	IncreaseGunGameLevelHandler* gunGameHandler = new IncreaseGunGameLevelHandler(PlayerIndex, this);
	players->iteratePlayers(gunGameHandler);
	delete gunGameHandler;
}

void GunGame::set_unit_grenades(BYTE type, BYTE count, int pIndex, bool bReset)
{
	int unit_datum_index = h2mod->get_unit_datum_from_player_index(pIndex);
	wchar_t* pName = h2mod->get_player_name_from_index(pIndex);

	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		if (bReset)
			call_unit_reset_equipment(unit_datum_index);

		if (type == GrenadeType::Frag)
		{
			*(BYTE*)((BYTE*)unit_object + 0x252) = count;


		}
		if (type == GrenadeType::Plasma)
		{
			*(BYTE*)((BYTE*)unit_object + 0x253) = count;
		}

		H2ModPacket GrenadePak;
		GrenadePak.set_type(H2ModPacket_Type_set_unit_grenades);
		h2mod_set_grenade *gnade = GrenadePak.mutable_set_grenade();
		gnade->set_count(count);
		gnade->set_pindex(pIndex);
		gnade->set_type(type);

		char* SendBuf = new char[GrenadePak.ByteSize()];
		GrenadePak.SerializeToArray(SendBuf, GrenadePak.ByteSize());

		network->networkCommand = SendBuf;
		network->sendCustomPacket(pIndex);
		network->networkCommand = NULL;

		delete[] SendBuf;
	}
}