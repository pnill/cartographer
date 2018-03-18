#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "stdafx.h"
#include <unordered_map>
#include "h2mod.h"

#include "H2ConsoleCommands.h"
#include "H2MOD_MapManager.h"
#include "H2MOD_Network.h"
#include "H2MOD_GameManager.h"

extern XNADDR join_game_xn;
extern MapManager* mapManager;

extern volatile bool isLobby;
//xnetcreatekey sets this to true
//xsessiondelete/end set this to false
extern bool isServer;

extern bool overrideUnicodeMessage;
extern ConsoleCommands* commands; 
extern GameManager* gameManager;

//some utility functions below
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
int stripWhitespace(wchar_t *inputStr);
