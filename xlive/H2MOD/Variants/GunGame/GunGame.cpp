#include "stdafx.h"
#include "Globals.h"
#include "..\..\Modules\Networking\Networking.h"

using namespace NetworkSession;

static int weapon_one = 0;
static int weapon_two = 0;
static int weapon_three = 0;
static int weapon_four = 0;
static int weapon_five = 0;
static int weapon_six = 0;
static int weapon_seven = 0;
static int weapon_eight = 0;
static int weapon_nine = 0;
static int weapon_ten = 0;
static int weapon_eleven = 0;
static int weapon_tweleve = 0;
static int weapon_thirteen = 0;
static int weapon_fourteen = 0;
static int weapon_fiffteen = 0;
static int weapon_sixteen = 0;


//TODO(PermaNull): Add additional levels with dual weilding

std::unordered_map<int, datum> GunGame::levelWeapon;
std::unordered_map<XUID, int> GunGame::gungamePlayers;

datum weaponDatums[36] = {
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

GunGame::GunGame()
{
	this->deinitializer = new GunGameDeinitializer();
	this->initializer = new GunGameInitializer();
	this->preSpawnPlayer = new GunGamePreSpawnHandler();
	this->spawnPlayer = new GunGameSpawnHandler();
	this->playerDeath = new GunGameDeathHandler();
	this->playerKill = new GunGameKillHandler();
}

void GunGame::readWeaponLevels()
{
	FILE* gfp;
	gfp = fopen("gungame.ini", "r");

	if (gfp)
	{
		LOG_TRACE_GAME("[GunGame Enabled] - Opened GunGame.ini!");
		while (!feof(gfp))
		{
			char gstr[256];

			fgets(gstr, 256, gfp);

			gCHECK_ARG("weapon_one =", weapon_one);
			gCHECK_ARG("weapon_two =", weapon_two);
			gCHECK_ARG("weapon_three =", weapon_three);
			gCHECK_ARG("weapon_four =", weapon_four);
			gCHECK_ARG("weapon_five =", weapon_five);
			gCHECK_ARG("weapon_six =", weapon_six);
			gCHECK_ARG("weapon_seven =", weapon_seven);
			gCHECK_ARG("weapon_eight =", weapon_eight);
			gCHECK_ARG("weapon_nine =", weapon_nine);
			gCHECK_ARG("weapon_ten =", weapon_ten);
			gCHECK_ARG("weapon_eleven =", weapon_eleven);
			gCHECK_ARG("weapon_tweleve =", weapon_tweleve);
			gCHECK_ARG("weapon_thirteen =", weapon_thirteen);
			gCHECK_ARG("weapon_fourteen =", weapon_fourteen);
			gCHECK_ARG("weapon_fifteen =", weapon_fiffteen);
			gCHECK_ARG("weapon_sixteen =", weapon_sixteen);

		}

#ifdef __DEBUG
		LOG_TRACE_GAME("[GunGame] : {}", b_GunGame);

		LOG_TRACE_GAME("[GunGame] - weapon_one: {}", weapon_one);
		LOG_TRACE_GAME("[GunGame] - weapon_two: {}", weapon_two);
		LOG_TRACE_GAME("[GunGame] - weapon_three: {}", weapon_three);
#endif

		fclose(gfp);
	}
}

void GunGame::initWeaponLevels() {
	if (weapon_one != 0) {
		LOG_TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one: {}", weapon_one);
		LOG_TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one enum:  {:x}", weaponDatums[weapon_one].ToInt());
		LOG_TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_two enum:  {:x}", weaponDatums[weapon_two].ToInt());
		GunGame::levelWeapon[0] = weaponDatums[weapon_one];
		GunGame::levelWeapon[1] = weaponDatums[weapon_two];
		GunGame::levelWeapon[2] = weaponDatums[weapon_three];
		GunGame::levelWeapon[3] = weaponDatums[weapon_four];
		GunGame::levelWeapon[4] = weaponDatums[weapon_five];
		GunGame::levelWeapon[5] = weaponDatums[weapon_six];
		GunGame::levelWeapon[6] = weaponDatums[weapon_seven];
		GunGame::levelWeapon[7] = weaponDatums[weapon_eight];
		GunGame::levelWeapon[8] = weaponDatums[weapon_nine];
		GunGame::levelWeapon[9] = weaponDatums[weapon_ten];
		GunGame::levelWeapon[10] = weaponDatums[weapon_eleven];
		GunGame::levelWeapon[11] = weaponDatums[weapon_tweleve];
		GunGame::levelWeapon[12] = weaponDatums[weapon_thirteen];
		GunGame::levelWeapon[13] = weaponDatums[weapon_fourteen];
		GunGame::levelWeapon[14] = weaponDatums[weapon_fiffteen];
	} else {
		GunGame::levelWeapon[0] = Weapon::energy_blade_useless;
		GunGame::levelWeapon[1] = Weapon::needler;
		GunGame::levelWeapon[2] = Weapon::plasma_pistol;
		GunGame::levelWeapon[3] = Weapon::magnum;
		GunGame::levelWeapon[4] = Weapon::smg;
		GunGame::levelWeapon[5] = Weapon::plasma_rifle;
		GunGame::levelWeapon[6] = Weapon::brute_plasma_rifle;
		GunGame::levelWeapon[7] = Weapon::juggernaut_powerup;
		GunGame::levelWeapon[8] = Weapon::shotgun;
		GunGame::levelWeapon[9] = Weapon::brute_shot;
		GunGame::levelWeapon[10] = Weapon::covenant_carbine;
		GunGame::levelWeapon[11] = Weapon::battle_rifle;
		GunGame::levelWeapon[12] = Weapon::beam_rifle;
		GunGame::levelWeapon[13] = Weapon::sniper_rifle;
		GunGame::levelWeapon[14] = Weapon::rocket_launcher;
	}
}

void GunGame::resetPlayerLevels() {
	gungamePlayers.clear();
}

void GunGame::spawnPlayerServer(int playerIndex) {
	LOG_TRACE_GAME(L"[H2Mod-GunGame]: SpawnPlayer() player index: {}, player name: {1}", playerIndex, h2mod->get_player_name_from_player_index(playerIndex));

	datum unit_datum_index = h2mod->get_unit_datum_from_player_index(playerIndex);
	int unit_object = call_object_try_and_get_with_type(unit_datum_index, 3);

	if (unit_object) {
		int level = GunGame::gungamePlayers[getPlayerXuid(playerIndex)];

		LOG_TRACE_GAME(L"[H2Mod-GunGame]: SpawnPlayer() - player index: {}, player name: {1} - Level: {2}", playerIndex, h2mod->get_player_name_from_player_index(playerIndex), level);

		datum CurrentWeapon = GunGame::levelWeapon[level];

		if (level < 15) {
			call_give_player_weapon(playerIndex, CurrentWeapon, 1);
		}
		else if (level == 15) {
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} on frag grenade level!", h2mod->get_player_name_from_player_index(playerIndex));
			h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Frag, 99, true);
		}
		else if (level == 16) {
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} on plasma grenade level!", h2mod->get_player_name_from_player_index(playerIndex));
			h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Plasma, 99, true);
		}
	}
}

void GunGame::playerDiedServer(int unit_datum_index)
{
	int unit_object = call_object_try_and_get_with_type(unit_datum_index, 3);
	if (unit_object) {
		int playerIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);
		h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Frag, 0, true);
		h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Plasma, 0, true);
	}
}

void GunGame::levelUpServer(int playerIndex)
{
	LOG_TRACE_GAME(L"[H2Mod-GunGame]: LevelUp() player index: {0}, player name: {1}", playerIndex, h2mod->get_player_name_from_player_index(playerIndex));

	int level = GunGame::gungamePlayers[getPlayerXuid(playerIndex)];
	level++;

	if (level > 16)
		level = GunGame::gungamePlayers[getPlayerXuid(playerIndex)] = 0; // reset level, so we dont keep the player without weapons, in case the game doesnt end

	GunGame::gungamePlayers[getPlayerXuid(playerIndex)] = level;

	LOG_TRACE_GAME("[H2Mod-GunGame]: LevelUp() player index: {0} - new level: {1} ", playerIndex, level);

	if (level < 15) {
		LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} on level {} giving them weapon...", h2mod->get_player_name_from_player_index(playerIndex), level);

		datum LevelWeapon = GunGame::levelWeapon[level];
		h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Frag, 0, true);
		h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Plasma, 0, true);
		call_give_player_weapon(playerIndex, LevelWeapon, 1);
	}

	else if (level == 15) {
		LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} Level 15 - Frag Grenades!", h2mod->get_player_name_from_player_index(playerIndex));
		h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Frag, 99,  true);
	}

	else if (level == 16) {
		LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} Level 16 - Plasma Grenades!", h2mod->get_player_name_from_player_index(playerIndex));
		h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Frag, 0,  true);
		h2mod->set_player_unit_grenades_count(playerIndex, GrenadeType::Plasma, 99,  true);
		//TODO: end game
	}
}

void GunGameDeinitializer::onClient() {
	//gun game does nothing for client on deinit
}

void GunGameDeinitializer::onDedi() {
	//gun game does nothing for dedi on deinit
}

void GunGameDeinitializer::onPeerHost() {
	//gun game does nothing for peer host on deinit
}

void GunGameInitializer::onClient() {
	GunGame::initWeaponLevels();
}

void GunGameInitializer::onDedi() {
	GunGame::initWeaponLevels();
	GunGame::resetPlayerLevels();
}

void GunGameInitializer::onPeerHost() {
	GunGame::initWeaponLevels();
	GunGame::resetPlayerLevels();
	//TODO: is this really necessary (from old code)?
	//init peer host gun game level
	GunGame::gungamePlayers[getPlayerXuid(h2mod->get_player_datum_index_from_controller_index(0).ToAbsoluteIndex())] = 0;
}

void GunGamePreSpawnHandler::onClient() {
	//gungame does nothing during pre spawn
}

void GunGamePreSpawnHandler::onDedi() {
	//gungame does nothing during pre spawn
}

void GunGamePreSpawnHandler::onPeerHost() {
	//gungame does nothing during pre spawn
}

void GunGameSpawnHandler::onClient() {
	//gungame does nothing for the client on spawn
}

void GunGameSpawnHandler::onDedi() {
	GunGame::spawnPlayerServer(this->getPlayerIndex());
}

void GunGameSpawnHandler::onPeerHost() {
	GunGame::spawnPlayerServer(this->getPlayerIndex());
}

void GunGameDeathHandler::onClient() {
	//gun game does nothing for clients on death
}

void GunGameDeathHandler::onDedi() {
	GunGame::playerDiedServer(this->getUnitDatumIndex());
}

void GunGameDeathHandler::onPeerHost() {
	GunGame::playerDiedServer(this->getUnitDatumIndex());
}

void GunGameKillHandler::onClient() {
	//gun game does nothing for clients on score/kills
}

void GunGameKillHandler::onDedi() {
	GunGame::levelUpServer(this->getPlayerIndex());
}

void GunGameKillHandler::onPeerHost() {
	GunGame::levelUpServer(this->getPlayerIndex());
}

GunGameHandler::GunGameHandler() {}

void GunGameHandler::setPlayerIndex(int playerIndex)
{
	this->playerIndex = playerIndex;
}

void GunGameHandler::setUnitDatumIndex(int unitDatumIndex)
{
	this->unitDatumIndex = (h2mod->get_player_index_from_unit_datum(unitDatumIndex) != -1 ? unitDatumIndex : -1);
}

int GunGameHandler::getPlayerIndex()
{
	return this->playerIndex;
}

int GunGameHandler::getUnitDatumIndex()
{
	return this->unitDatumIndex;
}
