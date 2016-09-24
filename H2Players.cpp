#include "Globals.h"

H2Player H2Players::getPlayer(std::string playerName) {
	return nameToPlayersMap[playerName];
}

H2Player H2Players::getPlayer(int playerIndex) {
	return indexToPlayersMap[playerIndex];
}

H2Player H2Players::getPlayer(XUID playerXuid) {
	return indexToPlayersMap[xuidToPlayerIndexMap[playerXuid]];
}

void H2Players::getPlayersForTeam(H2Player players[], int team) {
	//TODO:
}

void H2Players::update(std::string playerName, int playerIndex, int playerTeam, XUID playerXuid) {
	H2Player player = H2Player();
	player.index = playerIndex;
	player.name = playerName;
	player.team = playerTeam;
	player.xuid = playerXuid;

	//this creates a copy of player in each 3 maps, since they are not on the heap, I don't need to do anything with them
	//I hope...
	//TODO: add logging to detect when players change positions in the maps

	//xuid is not always provided in updates
	xuidToPlayerIndexMap[playerXuid] = playerIndex;
	indexToPlayersMap[playerIndex] = player;
	nameToPlayersMap[playerName] = player;

	//TODO: detect when player team changes and fire off team change listeners
}

void H2Players::clear() {
	xuidToPlayerIndexMap.clear();
	indexToPlayersMap.clear();
	nameToPlayersMap.clear();
}

bool H2Players::playersEqual(H2Player player1, H2Player player2) {
	//TODO:
	return false;
}
