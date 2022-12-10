#pragma once

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "H2MOD/Variants/VariantSystem.h"

extern class DeviceShop* deviceShop;

class FireFight : public ICustomGameVariant
{
public:
	virtual void Initialize() override;
	virtual void Dispose() override;
	virtual CustomVariantId GetVariantId();
	
	// on map load can be used as Initialize
	virtual void OnMapLoad(ExecTime execTime, s_game_options* gameOptions) override;
	virtual void OnPlayerSpawn(ExecTime execTime, datum playerIdx) override;
	virtual void OnPlayerDeath(ExecTime execTime, datum playerIdx) override;
	virtual void OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4) override;

	// unused bellow
	virtual bool OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx) override
	{
		return false;
	}

	virtual bool OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6) override;

	FireFight();
	~FireFight();

	static void KilledAI(datum killedAi, datum killerPlayerIdx);
};
