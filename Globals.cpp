#include "Globals.h"

//all these global variables live till the app closes

TSUsers* tsUsers = new TSUsers();
TSClient* client = NULL;
TSServer* server = NULL;

//TODO: only really works once the teamspeak client has opened the microphone up and validated it could, till then we just assume the microphone is enabled
//we need something a bit nicer for this
bool microphoneEnabled = true;
//always assume client is server unless they join a game which is called before xsessioncreate
//which creates or connects to teh team speak server
bool isServer = true;
bool customMapSocketOpen = false;
bool isLobby = true;

CLIENT_CHAT_MODE clientChatMode = TEAM;

ChatBoxCommands* commands = new ChatBoxCommands();

IN_ADDR clientMachineAddress = {};

H2Players* players = new H2Players();

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

int stripWhitespace(wchar_t *inputStr) {
	wchar_t *start, *end;
	start = inputStr;
	while (*start && *start == ' ') start++;
	printf("It is %i and %i\n", inputStr, start);
	printf("Strlen + 1 is: %i\n", wcslen(start) + 1);
	memmove(inputStr, start, wcslen(start) + 1);
	return 0;
}