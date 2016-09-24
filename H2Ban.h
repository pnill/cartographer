#ifndef BAN_H
#define BAN_H

#include <set>

struct BannedPlayerAddress : IN_ADDR {

	bool operator<(const BannedPlayerAddress& rhs) const
	{
		return S_un.S_addr < rhs.S_un.S_addr;
	}
};

struct BannedPlayer {
	std::string gamertag;
	BannedPlayerAddress ipaddress;
	XUID xboxId;

	bool operator<(const BannedPlayer& rhs) const
	{
		return xboxId < rhs.xboxId;
	}

	bool operator==(const BannedPlayer& rhs) const
	{
		return xboxId == rhs.xboxId && ipaddress.S_un.S_addr == rhs.ipaddress.S_un.S_addr && gamertag == rhs.gamertag;
	}
};


class BanUtility {
public:
	static BanUtility& getInstance() {
		static BanUtility instance;
		// Guaranteed to be destroyed and Instantiated on first use.
		return instance;
	}

	void addBannedPlayer(BannedPlayer player);
	void buildBannedPlayerData();
	bool isBannedAddress(IN_ADDR possibleBannedAddress);
	bool isBannedGamerTag(char* gamertag);
	bool isBannedXuid(XUID id);
	bool isMuteBan(std::string player);
	bool isValidAddressForAllowedCountries(IN_ADDR possibleBannedAddress);
	void banPlayer(char* gamertag, IN_ADDR ipAddress, XUID xboxId);
	void fillBannedPlayerDisplayStrList(std::vector<std::string> displayList);
private:
	BanUtility() {};
	BanUtility(BanUtility const&);     // Don't Implement
	void operator=(BanUtility const&); // Don't impl3men|

	std::set<BannedPlayer> bannedPlayers;
	std::set<std::string> bannedGamerTags;
	std::set<BannedPlayerAddress> bannedAddresses;
	std::set<XUID> bannedXboxIds;
	std::set<std::string> mutedPlayers;
};
#endif