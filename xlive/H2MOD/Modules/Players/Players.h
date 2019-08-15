#pragma once

#include "../Networking/NetworkSession/NetworkSession.h"

class Players {
public:
	bool IsActive(int playerIndex);
	int getPlayerCount();
	Membership* getMembershipData();
	PlayerInformation* getPlayerInformation(int playerIndex);

	int getPeerIndexFromPlayerXuid(long long xuid);
	int getPeerIndexFromPlayerIndex(int playerIndex);

	wchar_t* getPlayerName(int playerIndex);
	long long getPlayerXuidFromPlayerIndex(int playerIndex); 
	int getPlayerTeamFromXuid(long long xuid);
	int getPlayerTeamFromPlayerIndex(int playerIndex);

	void logAllPlayersToConsole();
	void logAllPeersToConsole();
};