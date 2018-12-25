#pragma once
#include "Globals.h"

class HeadHunterHandler : public GameClientServerHandler {
public:
	void SetXUID(XUID xuid);
	void SetPlayerIndex(DatumIndex player_datum);
	void SetUnitDatum(DatumIndex unit_datum);
	void SetDeadPlayer(DatumIndex dead_datum);
	bool SetInteractedObject(DatumIndex object_datum, int playerIndex);
	DatumIndex GetDeadPlayer();
	DatumIndex GetInteractedObject();
	XUID GetXUID();
private:
	XUID xuid;
	DatumIndex object_interaction;
	DatumIndex DeadPlayer;
	std::vector<DatumIndex> skulls_spawned;
	int playerIndex;
};


class HeadHunterInitializer : public HeadHunterHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class HeadHunterDeinitializer : public HeadHunterHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class HeadHunterSpawnHandler : public HeadHunterHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class HeadHunterPreSpawnHandler : public HeadHunterHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class HeadHunterDeathHandler : public HeadHunterHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class HeadHunterKillHandler : public HeadHunterHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};
class HeadHunterItmInteraction : public HeadHunterHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};

class HeadHunter : public GameType<HeadHunterHandler>
{
public:
	HeadHunter();
	static void SpawnSkull(DatumIndex PlayerDatum);
	static void PickupSkull(XUID player, DatumIndex SkullDatum);
	static void initClient();
	static void initHost();
	static void triggerSound(const wchar_t* name, int);
};
