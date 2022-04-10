#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "H2MOD\Variants\VariantSystem.h"

class GunGame : public ICustomGameVariant
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

	virtual bool OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6) override;

	GunGame();
	~GunGame() {  };

	static void ReadWeaponLevels();
	static void InitWeaponLevels();
	static void ResetPlayerLevels();

	static std::unordered_map<int, datum> levelWeapon;
	static std::unordered_map<XUID, int> gungamePlayers;

};
