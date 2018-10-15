#pragma once

class Players {
public:
	int getPlayerCount();
	int getPlayerOffset();
	int getPlayerStart(int playerIndex);

	int getPeerIndex(long long xuid);
	int getPeerIndex(int playerIndex);
	wchar_t* getPlayerName(int playerIndex);
	long long getPlayerXuid(int playerIndex); 
	int getPlayerTeam(long long xuid);
	int getPlayerTeam(int playerIndex);

	void logAllPlayersToConsole();
};