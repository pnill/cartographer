#pragma once

#include "stdafx.h"
#include "H2MOD.h"
#include "Util\Hooks\Hook.h"
#include "Util\ReadIniArguments.h"

#include "Blam\BlamLibrary.h"
#include "H2MOD\Variants\VariantPlayer.h"
#include "H2MOD\Variants\VariantSystem.h"
#include "H2MOD\Variants\DeviceShop\DeviceShop.h"
#include "H2MOD\Variants\H2Final\H2Final.h"
#include "H2MOD\Variants\XboxTick\XboxTick.h"
#include "H2MOD\Modules\DataArray\DataArray.h"
#include "H2MOD\Variants\Infection\Infection.h"
#include "H2MOD\Variants\FireFight\FireFight.h"
#include "H2MOD\Variants\GunGame\GunGame.h"
#include "H2MOD\Variants\HeadHunter\HeadHunter.h"
#include "H2MOD\Modules\Achievements\Achievements.h"
#include "H2MOD\Modules\AdvLobbySettings\AdvLobbySettings.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

#define run_async(expression) \
	std::thread{ [=] { expression; } }.detach();

using namespace Blam::EngineDefinitions::Actors;
using namespace Blam::EngineDefinitions::Players;
using namespace Blam::EngineDefinitions::Objects;

extern s_datum_array* game_state_actors;
extern s_datum_array* game_state_players;
extern s_datum_array* game_state_objects_header;

extern DeviceShop* device_shop;
extern VariantPlayer* variant_player;

extern AdvLobbySettings* advLobbySettings;
extern bool displayXyz;
extern volatile bool isLobby;

extern std::map<DWORD, bool> achievementList;

extern bool microphoneEnabled;
extern std::unordered_map<XUID, BOOL> xuidIsTalkingMap;

//some utility functions below
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
int stripWhitespace(wchar_t *inputStr);

extern int H2GetInstanceId();