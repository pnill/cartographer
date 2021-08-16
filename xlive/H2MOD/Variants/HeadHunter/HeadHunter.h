#pragma once

#include "..\VariantSystem.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

class HeadHunterHandler : public GameClientServerHandler {
public:
	void SetXUID(XUID xuid);
	void SetPlayerIndex(datum player_datum);
	void SetUnitDatum(datum unit_datum);
	void SetDeadPlayer(datum dead_datum);
	bool SetInteractedObject(datum object_datum);
	datum GetDeadPlayer();
	datum GetInteractedObject();
	XUID GetXUID();
private:
	XUID xuid;
	datum object_interaction;
	datum DeadPlayer;
	std::vector<datum> skulls_spawned;
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
enum e_headhunter_sounds
{
	head_hunter,
	skull_scored
};
class HeadHunter : public GameType<HeadHunterHandler>
{
public:
	HeadHunter();
	static void SpawnSkull(datum PlayerDatum);
	static void PickupSkull(XUID player, datum SkullDatum);
	static void initClient();
	static void initHost();
	static void triggerSound(e_headhunter_sounds sound, int sleep);
	static void spawnPlayerClientSetup();
};
