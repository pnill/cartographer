#pragma once

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Variants/VariantSystem.h"

#define SND_INFECTION_CH L"sounds/ch/infection.wav"
#define SND_INFECTED_CH L"sounds/ch/infected.wav"
#define SND_NEW_ZOMBIE_CH L"sounds/ch/new_zombie.wav"

#define SND_INFECTION_EN L"sounds/en/infection.wav"
#define SND_INFECTED_EN L"sounds/en/infected.wav"
#define SND_NEW_ZOMBIE_EN L"sounds/en/new_zombie.wav"

#define SND_INFECTION_ES L"sounds/es/infection.wav"
#define SND_INFECTED_ES L"sounds/es/infected.wav"
#define SND_NEW_ZOMBIE_ES L"sounds/es/new_zombie.wav"

#define SND_INFECTION_FR L"sounds/fr/infection.wav"
#define SND_INFECTED_FR L"sounds/fr/infected.wav"
#define SND_NEW_ZOMBIE_FR L"sounds/fr/new_zombie.wav"

#define SND_INFECTION_GE L"sounds/ge/infection.wav"
#define SND_INFECTED_GE L"sounds/ge/infected.wav"
#define SND_NEW_ZOMBIE_GE L"sounds/ge/new_zombie.wav"

#define SND_INFECTION_IT L"sounds/it/infection.wav"
#define SND_INFECTED_IT L"sounds/it/infected.wav"
#define SND_NEW_ZOMBIE_IT L"sounds/it/new_zombie.wav"

#define SND_INFECTION_JP L"sounds/jp/infection.wav"
#define SND_INFECTED_JP L"sounds/jp/infected.wav"
#define SND_NEW_ZOMBIE_JP L"sounds/jp/new_zombie.wav"

#define SND_INFECTION_KO L"sounds/ko/infection.wav"
#define SND_INFECTED_KO L"sounds/ko/infected.wav"
#define SND_NEW_ZOMBIE_KO L"sounds/ko/new_zombie.wav"

enum e_infection_sounds
{
	_snd_infection,
	_snd_infected,
	_snd_new_zombie,
	_infection_end
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

	// unused below
	virtual void OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4) override {}
	virtual bool OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx) override { return false; }
	virtual bool OnPlayerScore(ExecTime execTime, void* thisptr, datum playerIdx, int a3, int a4, int a5, char a6) override { return false; }

	Infection() = default;
	virtual ~Infection() = default;

	static void InitClient();
	static void InitHost();
	static void preSpawnServerSetup();
	static void resetWeaponInteractionAndEmblems();
	static void sendTeamChange();
	static void resetZombiePlayerStatus();
	static void setZombiePlayerStatus(unsigned long long identifier);
	static void setPlayerAsHuman(int playerIndex);
	static void setPlayerAsZombie(int playerIndex);
	static void triggerSound(e_infection_sounds sound, int sleep);
	static void onGameTick();
	static int calculateZombiePlayerIndex();
private:
	static std::vector<unsigned long long> zombieIdentifiers;
};
