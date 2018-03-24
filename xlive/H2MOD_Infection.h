#pragma once
#include "Globals.h"

class ZombieHandler : public GameClientServerHandler {
public:
	void setPlayerIndex(int playerIndex);
	void setUnitDatumIndex(int unitDatumIndex);
	int getPlayerIndex();
	int getUnitDatumIndex();
private:
	int playerIndex;
	int unitDatumIndex;
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
	static void sendTeamChange(int peerIndex);
	static void disableSlayerSounds();
	static void resetZombiePlayerStatus();
	static void setZombiePlayerStatus(int index);
	static void spawnPlayerClientSetup(int index);
	static void spawnServerPlayerSetup(int index);
	static void setPlayerAsHuman(int index);
	static void setPlayerAsZombie(int index);
	static void infectPlayer(int unitDatumIndex, int playerIndex);
	static void infectPlayers(int unitDatumIndex, int playerIndex);
	static void triggerSound(const wchar_t* name, int);
	static int calculateZombieIndexBasedOnPlayerData();
private:
	static std::vector<std::wstring> zombieNames;
};

