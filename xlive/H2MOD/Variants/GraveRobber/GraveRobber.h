#pragma once

#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "H2MOD/Variants/VariantSystem.h"

#define SND_HEADHUNTER_CH L"sounds/ch/headhunter.wav"
#define SND_SKULL_SCORED_CH L"sounds/ch/skull_scored.wav"
#define SND_HEADHUNTER_EN L"sounds/en/headhunter.wav"
#define SND_SKULL_SCORED_EN L"sounds/en/skull_scored.wav"
#define SND_HEADHUNTER_ES L"sounds/es/headhunter.wav"
#define SND_SKULL_SCORED_ES L"sounds/es/skull_scored.wav"
#define SND_HEADHUNTER_FR L"sounds/fr/headhunter.wav"
#define SND_SKULL_SCORED_FR L"sounds/fr/skull_scored.wav"
#define SND_HEADHUNTER_GE L"sounds/ge/headhunter.wav"
#define SND_SKULL_SCORED_GE L"sounds/ge/skull_scored.wav"
#define SND_HEADHUNTER_IT L"sounds/it/headhunter.wav"
#define SND_SKULL_SCORED_IT L"sounds/it/skull_scored.wav"
#define SND_HEADHUNTER_JP L"sounds/jp/headhunter.wav"
#define SND_SKULL_SCORED_JP L"sounds/jp/skull_scored.wav"
#define SND_HEADHUNTER_KO L"sounds/ko/headhunter.wav"
#define SND_SKULL_SCORED_KO L"sounds/ko/skull_scored.wav"

enum e_graverobber_sounds
{
	_snd_head_hunter,
	_snd_skull_scored,
	_graverobber_end
};

class GraveRobber : public ICustomGameVariant
{
public:
	void Initialize() override;
	void Dispose() override;
	CustomVariantId GetVariantId();

	// on map load can be used as Initialize
	virtual void OnMapLoad(ExecTime execTime, s_game_options* options) override;
	virtual void OnPlayerSpawn(ExecTime execTime, datum playerIdx) override;
	virtual void OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4) override {/*Unused*/ };
	virtual void OnPlayerDeath(ExecTime execTime, datum playerIdx) override;
	virtual bool OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx) override;
	virtual bool c_game_statborg__adjust_player_stat(ExecTime execTime, c_game_statborg* statborg, datum player_datum, e_statborg_entry statistic, short count, int game_results_statistic, bool adjust_team_stat) override;

	GraveRobber() = default;
	virtual ~GraveRobber() = default;
	static void InitializeClient();
	static void SpawnSkull(datum playerIdx);
	static void PickupSkull(datum playerIdx, datum skullDatum);
	static void TriggerSound(e_graverobber_sounds sound, int sleep);
	static void SpawnPlayerClientSetup();
};

bool graverobber_player_picking_up_skull();
