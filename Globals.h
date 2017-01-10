#ifndef GLOBAL_ENUMS_H
#define GLOBAL_ENUMS_H

//TODO: there are 16 colors
enum COLOR { red, yellow, green, purple, oragne, brown, pink };

//3 different chat modes allowed for each client
enum CLIENT_CHAT_MODE {
	TEAM,
	PROXIMITY,
	OFF
};

#endif

#ifndef GLOBALS_H
#define GLOBALS_H
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "stdafx.h"
#include <unordered_map>
#include "h2mod.h"

#include "H2ChatBoxCommands.h"
#include "H2MOD_MapManager.h"

//TODO: move into H2Server
extern XNADDR join_game_xn;

extern CHAR customMapDownloadLink[128];
extern CHAR customMapZipDownloadLink[128];

extern MapManager* mapManager;

extern bool isLobby;
extern UINT map_downloading_enable;
extern CHAR map_download_external_host[128];
extern CHAR map_download_external_uri[128];
extern UINT chatbox_commands;
extern bool overrideUnicodeMessage;
extern ChatBoxCommands* commands;

//some utility functions below

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
int stripWhitespace(wchar_t *inputStr);
#endif