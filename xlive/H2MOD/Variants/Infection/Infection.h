#pragma once
#include "Globals.h"

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
	static void setZombiePlayerStatus(int playerIndex);
	static void spawnPlayerClientSetup(int playerIndex);
	static void spawnServerPlayerSetup(int playerIndex);
	static void setPlayerAsHuman(int playerIndex);
	static void setPlayerAsZombie(int playerIndex);
	static void infectPlayer(datum unitDatumIndex, int playerIndex);
	static void infectPlayers(datum unitDatumIndex, int playerIndex);
	static void triggerSound(const wchar_t* name, int);
	static int calculateZombiePlayerIndex();
private:
	static std::vector<XUID> zombieIdentifiers;
};

