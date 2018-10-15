#pragma once
#include "Globals.h"

class FireFightHandler : public GameClientServerHandler {
public:
	void SetXUID(XUID xuid);
	void SetPlayerIndex(DatumIndex player_datum);
	void SetUnitDatum(DatumIndex unit_datum);
	void SetKilledDatum(DatumIndex unit_datum);
	bool BuyWeapon(DatumIndex device_datum);
	XUID GetXUID();
	DatumIndex GetKilledDatum();
private:
	XUID xuid;
	DatumIndex killed_datum;
};


class FireFightInitializer : public FireFightHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class FireFightDeinitializer : public FireFightHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class FireFightSpawnHandler : public FireFightHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class FireFightPreSpawnHandler : public FireFightHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class FireFightDeathHandler : public FireFightHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class FireFightKillHandler : public FireFightHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};

class FireFight : public GameType<FireFightHandler>
{
public:
	FireFight();
	static void KilledAI(DatumIndex ai_datum);
	static bool BuyWeapon(int cost, DatumIndex Weapon, XUID xuid);
	static void initClient();
	static void initHost();
private:
	static std::unordered_map<XUID, int> player_points;
};
