#pragma once

#include "..\VariantSystem.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

enum e_infection_sounds
{
	_snd_infection,
	_snd_infected,
	_snd_new_zombie
};

class Infection : public ICustomGameVariant
{
public:
	virtual void Initialize() override;
	virtual void Dispose() override;
	virtual CustomVariantId GetVariantId() override;

	// on map load can be used as Initialize
	virtual void OnMapLoad(ExecTime execTime, s_game_options* gameOptions) override;
	virtual void OnPlayerSpawn(ExecTime execTime, datum playerIdx) override;
	virtual void OnPlayerDeath(ExecTime execTime, datum playerIdx) override;
	
	// unused bellow
	virtual void OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4) override
	{
	}

	virtual bool OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx) override
	{
		return false;
	}

	virtual bool OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6) override
	{
		return false;
	}

	Infection();
	~Infection();

	static void InitClient();
	static void InitHost();
	static void preSpawnServerSetup();
	static void resetWeaponInteractionAndEmblems();
	static void sendTeamChange();
	static void disableSlayerSounds();
	static void resetZombiePlayerStatus();
	static void setZombiePlayerStatus(unsigned long long identifier);
	static void setPlayerAsHuman(int playerIndex);
	static void setPlayerAsZombie(int playerIndex);
	static void triggerSound(e_infection_sounds sound, int sleep);
	static int calculateZombiePlayerIndex();
private:
	static std::vector<unsigned long long> zombieIdentifiers;
};

