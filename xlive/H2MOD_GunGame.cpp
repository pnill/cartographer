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

std::unordered_map<int, int> GunGame::level_weapon;
std::unordered_map<std::wstring, int> GunGame::gungamePlayers;

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

GunGame::GunGame()
{
	this->deinitializer = new GunGameDeinitializer();
	this->initializer = new GunGameInitializer();
	this->preSpawnPlayer = new GunGamePreSpawnHandler();
	this->spawnPlayer = new GunGameSpawnHandler();
	this->playerDeath = new GunGameDeathHandler();
	this->playerKill = new GunGameKillHandler();
}

void GunGame::initWeaponLevels() {
	if (weapon_one != 0) {
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one: %i", weapon_one);
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one enum:  %08X", Weapon_ID[weapon_one]);
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_two enum:  %08X", Weapon_ID[weapon_two]);
		GunGame::level_weapon[0] = Weapon_ID[weapon_one];
		GunGame::level_weapon[1] = Weapon_ID[weapon_two];
		GunGame::level_weapon[2] = Weapon_ID[weapon_three];
		GunGame::level_weapon[3] = Weapon_ID[weapon_four];
		GunGame::level_weapon[4] = Weapon_ID[weapon_five];
		GunGame::level_weapon[5] = Weapon_ID[weapon_six];
		GunGame::level_weapon[6] = Weapon_ID[weapon_seven];
		GunGame::level_weapon[7] = Weapon_ID[weapon_eight];
		GunGame::level_weapon[8] = Weapon_ID[weapon_nine];
		GunGame::level_weapon[9] = Weapon_ID[weapon_ten];
		GunGame::level_weapon[10] = Weapon_ID[weapon_eleven];
		GunGame::level_weapon[11] = Weapon_ID[weapon_tweleve];
		GunGame::level_weapon[12] = Weapon_ID[weapon_thirteen];
		GunGame::level_weapon[13] = Weapon_ID[weapon_fourteen];
		GunGame::level_weapon[14] = Weapon_ID[weapon_fiffteen];
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

void GunGame::spawnPlayerServer(int playerIndex) {
	wchar_t* pName = h2mod->get_player_name_from_index(playerIndex);
	wchar_t* localName = h2mod->get_local_player_name();
	TRACE_GAME("[H2Mod-GunGame]: SpawnPlayer(%i) : %ws ", playerIndex, pName);

	int unit_datum_index = h2mod->get_unit_datum_from_player_index(playerIndex);
	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object) {
		int level = GunGame::gungamePlayers[players->getPlayerName(playerIndex)];

		TRACE_GAME("[H2Mod-GunGame]: SpawnPlayer(%i) %ws - Level: %i", playerIndex, pName, level);

		int CurrentWeapon = GunGame::level_weapon[level];

		if (level == 15) {
			TRACE_GAME("[H2Mod-GunGame]: %ws on frag grenade level!", pName);
			if (wcscmp(pName, localName) == 0) {
				call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(playerIndex));
				h2mod->set_local_grenades(GrenadeType::Frag, 99, playerIndex);
			} else {
				sendGrenadePacket(GrenadeType::Frag, 99, playerIndex, 1);
			}
		}

		if (level == 16) {
			TRACE_GAME("[H2Mod-GunGame]: %ws on plasma grenade level!", pName);

			if (wcscmp(pName, localName) == 0) {
				call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(playerIndex));
				h2mod->set_local_grenades(GrenadeType::Plasma, 99, playerIndex);
			} else {
				sendGrenadePacket(GrenadeType::Plasma, 99, playerIndex, 1);
			}
		}

		if (level < 15)	{
			GivePlayerWeapon(playerIndex, CurrentWeapon, 1);
		}
	}
}

void GunGame::playerDiedServer(int unit_datum_index)
{
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object) {
		call_unit_reset_equipment(unit_datum_index);

		int pIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);
		sendGrenadePacket(GrenadeType::Frag, 0, pIndex, 1);
		sendGrenadePacket(GrenadeType::Plasma, 0, pIndex, 1);
	}
}

void GunGame::levelUpServer(int PlayerIndex)
{
	wchar_t* PlayerName = h2mod->get_player_name_from_index(PlayerIndex);
	wchar_t* localName = h2mod->get_local_player_name();
	TRACE_GAME("[H2Mod-GunGame]: LevelUp(%i) : %ws", PlayerIndex, PlayerName);

	int level = GunGame::gungamePlayers[players->getPlayerName(PlayerIndex)];
	level++;
	GunGame::gungamePlayers[players->getPlayerName(PlayerIndex)] = level;

	TRACE_GAME("[H2Mod-GunGame]: LevelUp(%i) - new level: %i ", PlayerIndex, level);

	if (level == 15) {
		TRACE_GAME("[H2Mod-GunGame]: %ws Level 15 - Frag Grenades!", PlayerName);

		if (wcscmp(PlayerName, localName) == 0) {
			call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(PlayerIndex));
			h2mod->set_local_grenades(GrenadeType::Frag, 99, PlayerIndex);
		} else {
			sendGrenadePacket(GrenadeType::Frag, 99, PlayerIndex, 1);
		}
	}

	if (level == 16) {
		TRACE_GAME("[H2Mod-GunGame]: %ws Level 16 - Plasma Grenades!", PlayerName);
		if (wcscmp(PlayerName, localName) == 0) {
			call_unit_reset_equipment(h2mod->get_unit_datum_from_player_index(PlayerIndex));
			h2mod->set_local_grenades(GrenadeType::Plasma, 99, PlayerIndex);
		} else {
			sendGrenadePacket(GrenadeType::Plasma, 99, PlayerIndex, 1);
		}
		//TODO: end game
	}

	if (level < 15)	{
		TRACE_GAME("[H2Mod-GunGame]: %ws on level %i giving them weapon...", PlayerName, level);

		int LevelWeapon = GunGame::level_weapon[level];
		sendGrenadePacket(GrenadeType::Frag, 0, PlayerIndex, 1);
		sendGrenadePacket(GrenadeType::Plasma, 0, PlayerIndex, 1);
		GivePlayerWeapon(PlayerIndex, LevelWeapon, 1);
	}
}

void GunGame::sendGrenadePacket(BYTE type, BYTE count, int pIndex, bool bReset)
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
		if (h2mod->Server) {
			if (pIndex != 0) {
				//when sending a packet we use the players data structure to get peer index
				//the player structure does not have a slot for dedis, this player index was calculated based on that assumption
				//so we always subtract by 1 (since 0 here is not the slot for dedi anymore and can be a player)
				pIndex -= 1;
				TRACE_GAME("[h2mod-infection] altering player index from %d to %d", pIndex - 1, pIndex);
			}
		}
		TRACE_GAME("[H2Mod-Infection] Sending grenade packet, playerIndex=%d, peerIndex=%d", pIndex, players->getPeerIndex(pIndex));

		H2ModPacket teampak;
		teampak.set_type(H2ModPacket_Type_set_unit_grenades);

		h2mod_set_grenade *set_grenade = teampak.mutable_set_grenade();
		set_grenade->set_count(count);

		//protobuf has some weird bug where passing 0 has type in the current packet definition for set_grenade
		//completely breaks on the serialization side, https://groups.google.com/forum/#!topic/protobuf/JsougkaRcw4
		//no idea why, so we always add 1 to value till I can work with protobuf developers and figure out why
		set_grenade->set_type(type + 1);
		//send over player index
		set_grenade->set_pindex(pIndex);

		network->send_h2mod_packet_player(players->getPeerIndex(pIndex), teampak);
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
	GunGame::gungamePlayers[players->getPlayerName(0)] = 0;
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
	this->unitDatumIndex = unitDatumIndex;
}

int GunGameHandler::getPlayerIndex()
{
	return this->playerIndex;
}

int GunGameHandler::getUnitDatumIndex()
{
	return this->unitDatumIndex;
}
