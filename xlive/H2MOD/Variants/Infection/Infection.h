#pragma once

#include "..\VariantSystem.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

class ZombieHandler : public GameClientServerHandler {
public:
	void setPlayerIndex(int playerIndex);
	void setUnitDatumIndex(datum unitDatumIndex);
	int getPlayerIndex();
	datum getUnitDatumIndex();
private:
	int playerIndex;
	datum unitDatumIndex;
};

class InfectionInitializer : public ZombieHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class InfectionDeinitializer : public ZombieHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class ZombieSpawnHandler : public ZombieHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class ZombiePreSpawnHandler : public ZombieHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class ZombieDeathHandler : public ZombieHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class KillZombieHandler : public ZombieHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};

enum e_infection_sounds
{
	infection,
	infected,
	new_zombie
};

class Infection : public GameType<ZombieHandler>
{
public:
	Infection();
	static void initClient();
	static void initHost();
	static void preSpawnServerSetup();
	static void resetWeaponInteractionAndEmblems();
	static void sendTeamChange();
	static void disableSlayerSounds();
	static void resetZombiePlayerStatus();
	static void setZombiePlayerStatus(XUID identifier);
	static void spawnPlayerClientSetup(int playerIndex);
	static void spawnServerPlayerSetup(int playerIndex);
	static void setPlayerAsHuman(int playerIndex);
	static void setPlayerAsZombie(int playerIndex);
	static void infectPlayer(int playerIndex, datum unitDatumIndex);
	static void infectPlayers(int playerIndex, datum unitDatumIndex);
	static void triggerSound(e_infection_sounds sound, int sleep);
	static int calculateZombiePlayerIndex();
private:
	static std::vector<XUID> zombieIdentifiers;
};

