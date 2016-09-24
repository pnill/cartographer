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

#include "TSUsers.h"
#include "TSClient.h"
#include "TSServer.h"
#include "H2ChatBoxCommands.h"
#include "H2Ban.h"
#include "H2Players.h"

//mix of voice and game globals
//TODO: move into H2Server
extern TSUsers* tsUsers;
extern TSClient* client;
extern TSServer* server;

//extern H2Client* h2Client;

extern IN_ADDR clientMachineAddress;
//xnetcreatekey sets this to true
//xsessiondelete/end set this to false
extern bool isServer;
extern bool customMapSocketOpen;
extern bool microphoneEnabled;
extern bool isLobby;
extern CLIENT_CHAT_MODE clientChatMode;
extern ChatBoxCommands* commands;

//some utility functions below

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
int stripWhitespace(wchar_t *inputStr);

extern H2Players* players;
#endif