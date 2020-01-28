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

std::unordered_map<int, DatumIndex> GunGame::level_weapon;
std::unordered_map<std::wstring, int> GunGame::gungamePlayers;

DatumIndex weaponDatums[36] = {
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
		GunGame::level_weapon[0] = weaponDatums[weapon_one];
		GunGame::level_weapon[1] = weaponDatums[weapon_two];
		GunGame::level_weapon[2] = weaponDatums[weapon_three];
		GunGame::level_weapon[3] = weaponDatums[weapon_four];
		GunGame::level_weapon[4] = weaponDatums[weapon_five];
		GunGame::level_weapon[5] = weaponDatums[weapon_six];
		GunGame::level_weapon[6] = weaponDatums[weapon_seven];
		GunGame::level_weapon[7] = weaponDatums[weapon_eight];
		GunGame::level_weapon[8] = weaponDatums[weapon_nine];
		GunGame::level_weapon[9] = weaponDatums[weapon_ten];
		GunGame::level_weapon[10] = weaponDatums[weapon_eleven];
		GunGame::level_weapon[11] = weaponDatums[weapon_tweleve];
		GunGame::level_weapon[12] = weaponDatums[weapon_thirteen];
		GunGame::level_weapon[13] = weaponDatums[weapon_fourteen];
		GunGame::level_weapon[14] = weaponDatums[weapon_fiffteen];
	} else {
		GunGame::level_weapon[0] = Weapon::energy_blade_useless;
		GunGame::level_weapon[1] = Weapon::needler;
		GunGame::level_weapon[2] = Weapon::plasma_pistol;
		GunGame::level_weapon[3] = Weapon::magnum;
		GunGame::level_weapon[4] = Weapon::smg;
		GunGame::level_weapon[5] = Weapon::plasma_rifle;
		GunGame::level_weapon[6] = Weapon::brute_plasma_rifle;
		GunGame::level_weapon[7] = Weapon::juggernaut_powerup;
		GunGame::level_weapon[8] = Weapon::shotgun;
		GunGame::level_weapon[9] = Weapon::brute_shot;
		GunGame::level_weapon[10] = Weapon::covenant_carbine;
		GunGame::level_weapon[11] = Weapon::battle_rifle;
		GunGame::level_weapon[12] = Weapon::beam_rifle;
		GunGame::level_weapon[13] = Weapon::sniper_rifle;
		GunGame::level_weapon[14] = Weapon::rocket_launcher;
	}
}

void GunGame::resetPlayerLevels() {
	gungamePlayers.clear();
}

void GunGame::setGameScore() {
	if (h2mod->Server)
		*(int*)(0x300015F4) = 17;
	else
		*(int*)(0x30001A48) = 17;
}

void GunGame::spawnPlayerServer(int playerIndex) {
	wchar_t* playerName = h2mod->get_player_name_from_player_index(playerIndex);
	wchar_t* localName = h2mod->get_local_player_name();
	LOG_TRACE_GAME(L"[H2Mod-GunGame]: SpawnPlayer({0}) : {1}", playerIndex, playerName);

	DatumIndex unit_datum_index = h2mod->get_unit_datum_from_player_index(playerIndex);
	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object) {
		int level = GunGame::gungamePlayers[getPlayerName(playerIndex)];

		LOG_TRACE_GAME(L"[H2Mod-GunGame]: SpawnPlayer({0}) {1} - Level: {2}", playerIndex, playerName, level);

		DatumIndex CurrentWeapon = GunGame::level_weapon[level];

		if (level == 15) {
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} on frag grenade level!", playerName);
			if (wcscmp(playerName, localName) == 0) {
				call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(playerIndex));
				h2mod->set_local_player_unit_grenades(GrenadeType::Frag, 99, playerIndex);
			} else {
				sendGrenadePacket(GrenadeType::Frag, 99, playerIndex, 1);
			}
		}

		if (level == 16) {
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} on plasma grenade level!", playerName);

			if (wcscmp(playerName, localName) == 0) {
				call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(playerIndex));
				h2mod->set_local_player_unit_grenades(GrenadeType::Plasma, 99, playerIndex);
			} else {
				sendGrenadePacket(GrenadeType::Plasma, 99, playerIndex, 1);
			}
		}

		if (level < 15)	{
			call_give_player_weapon(playerIndex, CurrentWeapon, 1);
		}
	}
}

void GunGame::playerDiedServer(int unit_datum_index)
{
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object) {
		call_unit_reset_equipment(unit_datum_index);

		int playerIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);
		sendGrenadePacket(GrenadeType::Frag, 0, playerIndex, 1);
		sendGrenadePacket(GrenadeType::Plasma, 0, playerIndex, 1);
	}
}

void GunGame::levelUpServer(int playerIndex)
{
	wchar_t* playerName = h2mod->get_player_name_from_player_index(playerIndex);
	wchar_t* localName = h2mod->get_local_player_name();
	LOG_TRACE_GAME(L"[H2Mod-GunGame]: LevelUp({0}) : {1}", playerIndex, playerName);

	int level = GunGame::gungamePlayers[getPlayerName(playerIndex)];
	level++;
	GunGame::gungamePlayers[getPlayerName(playerIndex)] = level;

	LOG_TRACE_GAME("[H2Mod-GunGame]: LevelUp({0}) - new level: {1} ", playerIndex, level);

	if (level == 15) {
		LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} Level 15 - Frag Grenades!", playerName);

		if (wcscmp(playerName, localName) == 0) {
			call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(playerIndex));
			h2mod->set_local_player_unit_grenades(GrenadeType::Frag, 99, playerIndex);
		} else {
			sendGrenadePacket(GrenadeType::Frag, 99, playerIndex, 1);
		}
	}

	if (level == 16) {
		LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} Level 16 - Plasma Grenades!", playerName);
		if (wcscmp(playerName, localName) == 0) {
			call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(playerIndex));
			h2mod->set_local_player_unit_grenades(GrenadeType::Plasma, 99, playerIndex);
		} else {
			sendGrenadePacket(GrenadeType::Plasma, 99, playerIndex, 1);
		}
		//TODO: end game
	}

	if (level < 15)	{
		LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} on level {} giving them weapon...", playerName, level);

		DatumIndex LevelWeapon = GunGame::level_weapon[level];
		sendGrenadePacket(GrenadeType::Frag, 0, playerIndex, 1);
		sendGrenadePacket(GrenadeType::Plasma, 0, playerIndex, 1);
		call_give_player_weapon(playerIndex, LevelWeapon, 1);

	}
}

void GunGame::sendGrenadePacket(BYTE type, BYTE count, int playerIndex, bool bReset)
{
	DatumIndex unit_datum_index = h2mod->get_unit_datum_from_player_index(playerIndex);

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
		LOG_TRACE_GAME("[H2Mod-GunGame] Sending grenade packet, playerIndex={0}, peerIndex={1}", playerIndex, getPeerIndex(playerIndex));

		s_unit_grenades data;
		SecureZeroMemory(&data, sizeof(data));

		data.type = type;
		data.count = count;
		data.player_index = playerIndex;
		CustomPackets::sendUnitGrenadesPacket(getPeerIndex(playerIndex), &data);
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
	GunGame::setGameScore();
}

void GunGameInitializer::onDedi() {
	GunGame::initWeaponLevels();
	GunGame::resetPlayerLevels();
	GunGame::setGameScore();
}

void GunGameInitializer::onPeerHost() {
	GunGame::initWeaponLevels();
	GunGame::resetPlayerLevels();
	GunGame::setGameScore();
	//TODO: is this really necessary (from old code)?
	//init peer host gun game level
	GunGame::gungamePlayers[getPlayerName(0)] = 0;
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
