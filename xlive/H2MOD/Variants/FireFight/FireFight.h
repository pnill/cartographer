#pragma once
#include "Globals.h"

class FireFightHandler : public GameClientServerHandler {
public:
	void SetXUID(XUID xuid);
	void SetPlayerIndex(DatumIndex player_datum);
	void SetUnitDatum(DatumIndex unit_datum);
	void SetKilledDatum(DatumIndex unit_datum);
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
	static void KilledAI(DatumIndex ai_datum,XUID killer);
	static void initClient();
	static void initHost();
};
