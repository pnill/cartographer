#include "Globals.h"

//all these global variables live till the app closes
MapManager* mapManager = new MapManager();
AdvLobbySettings* advLobbySettings = new AdvLobbySettings();

GameStatePlayerTable *game_state_players = NULL;
GameStateObjectHeaderTable* game_state_objects_header = NULL;
GameStateActorTable* game_state_actors = NULL;

volatile bool isLobby = true;
bool overrideUnicodeMessage = false;

char* replacedNetworkNormalTextWidget;
char* replacedNetworkNormalTextWidget2;

ConsoleCommands* commands = new ConsoleCommands();
GameManager* gameManager = new GameManager();
CustomNetwork *network = new CustomNetwork();

Players* players = new Players();

//TODO: actually check if they have a microphone or check some settings
bool microphoneEnabled = true;
std::unordered_map<XUID, BOOL> xuidIsTalkingMap;
std::unordered_map<XUID, anyID> xuidToTSid;
IN_ADDR clientMachineAddress = {};
TSClient* client = NULL;
TSServer* server = NULL;

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
	wchar_t *start;
	start = inputStr;
	while (*start && *start == ' ') start++;
	printf("It is %p and %p\n", inputStr, start);
	printf("Strlen + 1 is: %i\n", wcslen(start) + 1);
	memmove(inputStr, start, wcslen(start) + 1);
	return 0;
}

void patchBYTEs(BYTE* orig, BYTE* values, int size) {
	DWORD dwBack;
	VirtualProtect(orig, size, PAGE_EXECUTE_READWRITE, &dwBack);
	memcpy(orig, (BYTE*)values, size);
	VirtualProtect(orig, size, dwBack, NULL);
}