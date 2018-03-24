#pragma once

class Players {
public:
	int getPlayerCount();
	int getPlayerOffset();
	int getPlayerStart(int playerIndex);

	int getPeerIndex(int playerIndex);
	wchar_t* getPlayerName(int playerIndex);
	long long getPlayerIdentifier(int playerIndex);
	int getPlayerTeam(int playerIndex);

	void logAllPlayersToConsole();
};