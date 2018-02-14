#pragma once

class Player {
public:
	Player(std::string playerIp, std::wstring playerName, int peerIndex, int playerTeam, int playerIndex, long long identifier);
	void setIsZombie(bool isZombie);
	void setGunGameLevel(int level);
	std::wstring getPlayerName();
	int getPlayerTeam();
	int getPeerIndex();
	std::string getIp();
	int getPlayerIndex();
	long long getPlayerIdentifier();
	int getGunGameLevel();
	BOOL getIsZombie();

private:
	std::string playerIp;
	std::wstring playerName;
	int peerIndex;
	int playerIndex;
	int playerTeam;
	bool isZombie = false; //no one is a zombie by default
	int gunGameLevel = 0; //all gun game levels start at 0
	long long identifier;

	//TODO: xuid (is identifier the xuid?)
	//TODO: ping value
	//TODO: last ping time
	//TODO: is zombie
};

/*
 * This class exists to allow other classes/functions to iterate players in a synchronized fashion
 */
class PlayerIterableHandler {
public:
	virtual void onPlayerIterated(Player* player) = 0;
	BOOL stopIteration = false;
};