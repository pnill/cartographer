#pragma once

#include "../VariantSystem.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

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
	static void preSpawnPlayerSetup(int playerIndex);

	static std::unordered_map<int, datum> levelWeapon;
	static std::unordered_map<XUID, int> gungamePlayers;

	~GunGame() {  };
};
