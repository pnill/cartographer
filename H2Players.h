#ifndef PLAYERS_H
#define PLAYERS_H

struct H2Player {
	std::string name;
	int index = -1;
	int team;
	XUID xuid;

	H2Player() {}

	//copy constructor
	H2Player(H2Player &obj) {
		name = obj.name;
		index = obj.index;
		team = obj.team;
		xuid = obj.xuid;
	}
};

class H2Players {
public:
	H2Player getPlayer(std::string playerName);
	H2Player getPlayer(int playerIndex);
	H2Player getPlayer(XUID playerXuid);
	void getPlayersForTeam(H2Player players[], int team);
	void update(std::string playerName, int playerIndex, int playerTeam, XUID playerXuid);
	void clear();

private:
	bool playersEqual(H2Player player1, H2Player player2);

	std::unordered_map<XUID, int> xuidToPlayerIndexMap;
	std::unordered_map<int, H2Player> indexToPlayersMap;
	std::unordered_map<std::string, H2Player> nameToPlayersMap;
	std::unordered_map<int, H2Player[]> teamToPlayersMap;
};

#endif