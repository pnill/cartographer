#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "stdafx.h"
#include <unordered_map>
#include <stdlib.h>
#include <mutex>
#include "h2mod.h"
#include <windows.h>
#include "xliveless.h"
#include "Util\Hooks\Hook.h"
#include "Util\ReadIniArguments.h"

#include "Blam\BlamLibrary.h"
#include "H2MOD\TeamSpeak\TSClient.h"
#include "H2MOD\TeamSpeak\TSServer.h"
#include "H2MOD\Modules\Players\Players.h"
#include "H2MOD\Variants\VariantPlayer.h"
#include "H2MOD\Variants\VariantSystem.h"
#include "H2MOD\Variants\DeviceShop\DeviceShop.h"
#include "H2MOD\Variants\H2Final\H2Final.h"
#include "H2MOD\Variants\Infection\Infection.h"
#include "H2MOD\Variants\FireFight\FireFight.h"
#include "H2MOD\Variants\GunGame\GunGame.h"
#include "H2MOD\Variants\HeadHunter\HeadHunter.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"
#include "H2MOD\Modules\MapManager\MapManager.h"
#include "H2MOD\Modules\Networking\Networking.h"
#include "H2MOD\Modules\GameManager\GameManager.h"
#include "H2MOD\Modules\AdvLobbySettings\AdvLobbySettings.h"
#include "H2MOD\Modules\Achievements\Achievements.h"


using namespace Blam::EngineDefinitions::Players;
using namespace Blam::EngineDefinitions::Objects;
using namespace Blam::EngineDefinitions::Actors;
using namespace Blam::EngineDefinitions::Tag;

extern GameStatePlayerTable *game_state_players;
extern GameStateObjectHeaderTable* game_state_objects_header;
extern GameStateActorTable* game_state_actors;
extern global_tag_instance* tag_instances;

extern DeviceShop* device_shop;
extern VariantPlayer* variant_player;

extern XNADDR join_game_xn;
extern MapManager* mapManager;
extern AdvLobbySettings* advLobbySettings;
extern bool displayXyz;
extern volatile bool isLobby;
extern bool overrideUnicodeMessage;
extern ConsoleCommands* commands; 
extern GameManager* gameManager;
extern Players* players;
extern CustomNetwork *network;
extern char* replacedNetworkNormalTextWidget;
extern char* replacedNetworkNormalTextWidget2;

extern std::unordered_map<XUID, BOOL> xuidIsTalkingMap;
extern std::unordered_map<XUID, anyID> xuidToTSid;
extern std::map<DWORD, bool> achievementList;

extern bool microphoneEnabled;
extern IN_ADDR clientMachineAddress;
extern TSClient* tsClient;
extern TSServer* tsServer;

//some utility functions below
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
int stripWhitespace(wchar_t *inputStr);

void patchBYTEs(BYTE* orig, BYTE* values, int size);

extern int H2GetInstanceId();

/* XLive Globals*/
extern UINT g_online;
extern XUID xFakeXuid[4];
