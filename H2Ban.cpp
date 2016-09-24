#include "Globals.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

void BanUtility::addBannedPlayer(BannedPlayer player) {
	getInstance().bannedPlayers.insert(player);
	getInstance().bannedAddresses.insert(player.ipaddress);
	getInstance().bannedGamerTags.insert(player.gamertag);
	getInstance().bannedXboxIds.insert(player.xboxId);
}

void BanUtility::buildBannedPlayerData() {
	std::ifstream fin("bannedPlayers.csv");
	if (!fin) {
		TRACE_GAME_N("Could not open ban file");
		return;
	}

	std::string line;
	const char delim = ';';
	while (getline(fin, line)) {
		std::string line_value;
		std::vector<std::string> line_values;
		std::stringstream ss(line);
		int i = 0;

		std::string gamertag;
		BannedPlayerAddress ipAddress;
		XUID xboxId;
		bool muteBan;
		while (std::getline(ss, line_value, ',')) {
			if (i == 0) {
				gamertag = line_value;
			}
			else if (i == 1) {
				ipAddress.S_un.S_addr = inet_addr(line_value.c_str());
			}
			else if (i == 2) {
				std::stringstream(line_value) >> xboxId;
			}
			i++;
		}
		BannedPlayer bannedPlayer = {
			gamertag, ipAddress, xboxId
		};
		addBannedPlayer(bannedPlayer);
	}
}

bool BanUtility::isBannedAddress(IN_ADDR possibleBannedAddress) {
	BannedPlayerAddress ipAddress;
	ipAddress.S_un.S_addr = possibleBannedAddress.S_un.S_addr;
	return bannedAddresses.find(ipAddress) != bannedAddresses.end();
}

bool BanUtility::isBannedGamerTag(char* possibleBannedGamertag) {
	return bannedGamerTags.find(std::string(possibleBannedGamertag)) != bannedGamerTags.end();
}

bool BanUtility::isBannedXuid(XUID possibleBannedXuid) {
	return bannedXboxIds.find(possibleBannedXuid) != bannedXboxIds.end();
}

bool BanUtility::isMuteBan(std::string player) {
	return mutedPlayers.find(player) != mutedPlayers.end();
}

bool BanUtility::isValidAddressForAllowedCountries(IN_ADDR possibleBannedAddress) {
	//TODO:
	return true;
}

void BanUtility::banPlayer(char* gamertag, IN_ADDR ipAddress, XUID xboxId) {
	BannedPlayerAddress futureBannedAddress;
	futureBannedAddress.S_un.S_addr = ipAddress.S_un.S_addr;
	BannedPlayer bannedPlayer = {
		gamertag, futureBannedAddress, xboxId
	};

	if (bannedPlayers.find(bannedPlayer) == bannedPlayers.end()) {
		//not a banned player, so add the player and write to the csv file
		addBannedPlayer(bannedPlayer);

		std::fstream file("bannedPlayers.csv", std::fstream::app | std::fstream::in | std::fstream::out);
		file << bannedPlayer.gamertag << ',' << inet_ntoa(bannedPlayer.ipaddress) << ',' << bannedPlayer.xboxId << ";";
	}
}

void BanUtility::fillBannedPlayerDisplayStrList(std::vector<std::string> displayList) {
	std::set<BannedPlayer>::iterator it;
	for (it = bannedPlayers.begin(); it != bannedPlayers.end(); ++it) {
		BannedPlayer f = *it;
		std::stringstream oss;
		oss << f.gamertag << ',' << inet_ntoa(f.ipaddress) << ',' << f.xboxId << ";";
		displayList.emplace_back(oss.str());
	}
}