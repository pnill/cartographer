#pragma once

#include "Globals.h"

enum Weapon
{
	//Unknown = 0x2,//36
	//None = 0x1,//37
	chaingun_turret = 0xE53D2AD8,//0
	gauss_turret = 0xE5F02B8B,//1
	warthog_horn = 0xE6322BCD,//2
	banshee_gun = 0xE6AF2C4A,//3
	ghost_gun = 0xE75F2CFA,//4
	h_turret_ap = 0xE79B2D36,//5
	cannon_turret = 0xE8172DB2,//6
	cannon_turret_mp = 0xE8382DD3,//7
	big_needler = 0xE8742E0F,//8
	mortar_turret = 0xE8D32E6E,//9
	minigun_turret = 0xE9062EA1,//10
	mortar_turret_mp = 0xE90C2EA7,//11
	plasma_turret = 0xE9732F0E,//12
	plasma_cannon = 0xE9F62F91,//13
	magnum = 0xEAD83073,//14
	needler = 0xEB4230DD,//15
	plasma_pistol = 0xEB9E3139,//16
	battle_rifle = 0xEC3131CC,//17
	covenant_carbine = 0xEC673202,//18
	plasma_rifle = 0xEC9E3239,//19
	shotgun = 0xECD63271,//20
	smg = 0xED3F32DA,//21
	sniper_rifle = 0xED753310,//22
	flak_cannon = 0xEDA2333D,//23
	rocket_launcher = 0xEDD4336F,//24
	brute_shot = 0xEE0933A4,//25
	assault_bomb = 0xEE3433CF,//26
	ball = 0xEE5233ED, //27
	flag = 0xEE5F33FA,//28
	energy_blade = 0xEE7B3416, //29
	energy_blade_useless = 0xEE993434, //30
	beam_rifle = 0xEE9E3439, //31
	brute_plasma_rifle = 0xEED3346E,//32
	sentinel_aggressor_beam = 0xEEF1348C,//33
	smg_silenced = 0xEF1B34B6,//34
	juggernaut_powerup = 0xF33838D2 //35
};

class GunGameHandler : public GameClientServerHandler {
public:
	GunGameHandler();
	void setPlayerIndex(int playerIndex);
	void setUnitDatumIndex(int unitDatumIndex);
	int getPlayerIndex();
	int getUnitDatumIndex();
private:
	int playerIndex;
	int unitDatumIndex;
};

class GunGameInitializer : public GunGameHandler {
public:
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class GunGameDeinitializer : public GunGameHandler {
public:
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class GunGameSpawnHandler : public GunGameHandler {
public:
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class GunGamePreSpawnHandler : public GunGameHandler {
public:
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class GunGameDeathHandler : public GunGameHandler {
public:
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class GunGameKillHandler : public GunGameHandler {
public:
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};

class GunGame : public GameType<GunGameHandler>
{
public:
	GunGame();
	static void readWeaponLevels();
	static void initWeaponLevels();
	static void spawnPlayerServer(int playerIndex);
	static void playerDiedServer(int unit_datum_index); // We need to start using PlayerIndex here for sanity.
	static void levelUpServer(int PlayerIndex);
	static void resetPlayerLevels();

	static std::unordered_map<int, datum> levelWeapon;
	static std::unordered_map<XUID, int> gungamePlayers;

	~GunGame() {  };
};
