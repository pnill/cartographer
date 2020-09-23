#pragma once

#include "H2MOD.h"
#include "Util\Hooks\Hook.h"
#include "Util\ReadIniArguments.h"

#include "H2MOD\Variants\VariantPlayer.h"
#include "H2MOD\Variants\VariantSystem.h"
#include "H2MOD\Variants\DeviceShop\DeviceShop.h"
#include "H2MOD\Variants\XboxTick\XboxTick.h"
#include "Blam/Engine/DataArray/DataArray.h"
#include "H2MOD\Variants\Infection\Infection.h"
#include "H2MOD\Variants\FireFight\FireFight.h"
#include "H2MOD\Variants\GunGame\GunGame.h"
#include "H2MOD\Variants\HeadHunter\HeadHunter.h"
#include "H2MOD\Modules\Achievements\Achievements.h"
#include "H2MOD\Modules\AdvLobbySettings\AdvLobbySettings.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

#define run_async(expression) \
	std::thread{ [=] { expression; } }.detach();

extern s_datum_array* game_state_actors;
extern s_datum_array* game_state_objects_header;

extern DeviceShop* device_shop;
extern VariantPlayer* variant_player;

extern AdvLobbySettings* advLobbySettings;
extern bool displayXyz;

extern std::map<DWORD, bool> achievementList;

extern int H2GetInstanceId();

