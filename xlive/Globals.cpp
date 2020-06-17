#include "stdafx.h"
#include "Globals.h"

//all these global variables live till the app closes
AdvLobbySettings* advLobbySettings = new AdvLobbySettings();

s_datum_array* game_state_actors = nullptr;
s_datum_array* game_state_players = nullptr;
s_datum_array* game_state_objects_header = nullptr;

bool displayXyz = false;

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

void HexStrToBytes(const std::string& hexStr, BYTE* byteBuf, size_t bufLen) {
	for (unsigned int i = 0; i < hexStr.length() && i < bufLen; i++) {
		byteBuf[i] = (BYTE)strtol(hexStr.substr(i * 2, 2).c_str(), NULL, 16);
	}
}

std::string ByteToHexStr(const BYTE* buffer, size_t size) {
	std::stringstream str;
	str.setf(std::ios_base::hex, std::ios::basefield);
	str.setf(std::ios_base::uppercase);
	str.fill('0');

	for (size_t i = 0; i < size; i++) {
		str << std::setw(2) << (unsigned short)(BYTE)buffer[i];
	}
	return str.str();
}
