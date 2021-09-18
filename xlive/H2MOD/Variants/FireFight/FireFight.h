#pragma once

#include "..\VariantSystem.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

class FireFightHandler : public GameClientServerHandler {
public:
	void SetXUID(XUID xuid);
	void SetPlayerIndex(datum player_datum);
	void SetUnitDatum(datum unit_datum);
	void SetKilledDatum(datum unit_datum);
	XUID GetXUID();
	datum GetKilledDatum();
private:
	XUID xuid;
	datum killed_datum = DATUM_NONE;
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
	static void KilledAI(datum ai_datum,XUID killer);
	static void initClient();
	static void initHost();
};
