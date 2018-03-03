#pragma once

//class used to allow external classes iterate the synchronized players structure without exposing the entire structure
class PlayerIterator {
public:
	virtual void onIteratePlayer(Player* player) = 0;
};

//class used to allow external classes to lookup a synchronized player and indicate when they found a player without exposing the entire players data structure
class PlayerLookup {
public:
	//TODO: add ways to lookup players via index, team, name, etc
	virtual void lookup(Player* player) = 0;
	BOOL foundPlayer = false;
};

class Players {
public:
	int getPlayerCount();
	void reload();
	void logAllPlayersToConsole();
	void iteratePlayers(PlayerIterator* playerHandler);
	void lookupPlayer(PlayerLookup* playerLookup);
	int getPlayerOffset();
private:
	std::mutex playerLock;
	std::vector<Player*> players;
};