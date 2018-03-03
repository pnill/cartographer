#include "Globals.h"
#include <mutex>
#include <h2mod.pb.h>

std::string emptyStr("");
std::wstring emptyStr2(L"");

int Players::getPlayerOffset() {
	int playerDataOffset = (*(int*)(h2mod->GetBase() + (h2mod->Server ? 0x520B94 : 0x51C474)));
	int playerDataOffsetTmp = *(DWORD *)(playerDataOffset + 29600);
	if (playerDataOffsetTmp == 5 || playerDataOffsetTmp == 6 || playerDataOffsetTmp == 7 || playerDataOffsetTmp == 8) {
		//valid memory offset
		return playerDataOffset + 112;
	}

	playerDataOffset = (*(int*)(h2mod->GetBase() + (h2mod->Server ? 0x520B94 : 0x51C474))) + 31624;
	return playerDataOffset + 112;// there can't be any other place for the data
}

int Players::getPlayerCount() {
	int playerDataOffset = this->getPlayerOffset();
	int playerOrPeerCount = *(DWORD *)(playerDataOffset + 0x14);
	//the player structure we use contains the dedicated server for whatever reason, so we always subtract player count by 1 when we are the dedi
	return playerOrPeerCount;
}

void Players::reload() {
	playerLock.lock();

	if (this->players.empty()) {
		//the dedi has a extra peer slot for the dedi itself for whatever reason
		for (int i = 0; i < (h2mod->Server ? 17 : 16); i++) {

			Player* player = new Player();
			this->players.push_back(player);
		}
	}

	std::unordered_map<unsigned long long, int> identifierToGunGameLevel;
	std::unordered_map<unsigned long long, BOOL> identifierToIsZombie;
	std::vector<Player*>::iterator it;
	int i = 0;
	for (it = this->players.begin(); it != this->players.end(); it++, i++) {
		Player* player = *it;
		identifierToGunGameLevel[player->getPlayerIdentifier()] = player->getGunGameLevel();
		identifierToIsZombie[player->getPlayerIdentifier()] = player->getIsZombie();
	}

	int playerDataOffset = this->getPlayerOffset();
	int peerStart = playerDataOffset + 160;
	int playerStart = playerDataOffset + 4608;
	int playerCounter = 0;
	if (h2mod->Server) {
		peerStart += 268;
		playerCounter++;
	}
	do {
		//from everything I can tell in h2_set_membership_packet_data(), peer and player index don't actually ever
		//change, the people who join occupy spots and the other info is stored in those spots, but there is no shifting
		//of these indexes for in game players
		int peerIndex = playerCounter + (h2mod->Server ? 1 : 0);
		int playerIndex = playerCounter;

		wchar_t* peerName = (wchar_t*)(peerStart - 96);
		wchar_t* peerSessionName = (wchar_t*)(peerStart - 64);
		struct in_addr addr;
		addr.S_un.S_un_b.s_b1 = ((char*)(peerStart - 136))[0];
		addr.S_un.S_un_b.s_b2 = ((char*)(peerStart - 136))[1];
		addr.S_un.S_un_b.s_b3 = ((char*)(peerStart - 136))[2];
		addr.S_un.S_un_b.s_b4 = ((char*)(peerStart - 136))[3];
		char* peerAddress = inet_ntoa(addr);
		//TRACE_GAME("[h2mod-network] membership info, peerIndex=%d, peerName=%s, sessionName=%s", peerIndex, peerName, peerSessionName);
		//TRACE_GAME_N("[h2mod-network] membership info, peerAddress=%s", peerAddress);

		int newPlayerStart1 = playerStart + 4;
		int newPlayerStart2 = playerStart + 136;

		DWORD ident1 = *(DWORD *)(playerStart - 20);
		DWORD ident2 = *(DWORD *)(playerStart - 16);
		//the ident appears to made up of 8 bytes
		long long identifier = (long long)ident1 << 32 | ident2;

		wchar_t* playerName = (wchar_t*)(newPlayerStart1);
		playerName = (wchar_t*)(newPlayerStart2);

		int playerTeam = *(BYTE *)(newPlayerStart1 + 124);
		playerTeam = *(BYTE *)(newPlayerStart2 + 124);
		if (playerTeam != -1) {
			//TRACE_GAME_N("[h2mod-network] membership info, playerTeam=%d", playerTeam);
		}

		peerStart += 268;
		playerStart += 296;
		Player* player = this->players.at(playerCounter);
		player->setIp(peerAddress);
		player->setPeerIndex(peerIndex);
		player->setPlayerIndex(playerIndex);
		player->setPlayerIdentifier(identifier);
		player->setPlayerTeam(playerTeam);
		player->setPlayerName(playerName);
		player->setGunGameLevel(identifierToGunGameLevel[identifier]);
		player->setIsZombie(identifierToIsZombie[identifier]);
		playerCounter++;
	} while (playerCounter < this->getPlayerCount());

	//reset any player data that isn't active
	for (int i = playerCounter; i < 16; i++) {
		Player* player = this->players.at(playerCounter);
		player->setIp(emptyStr);
		player->setPeerIndex(-1);
		player->setPlayerIndex(-1);
		player->setPlayerIdentifier(-1);
		player->setPlayerTeam(-1);
		player->setPlayerName(emptyStr2);
		player->setGunGameLevel(-1);
		player->setIsZombie(false);
	}
	playerLock.unlock();

	//clear temp maps to carry over data
	identifierToGunGameLevel.clear();
	identifierToIsZombie.clear();
}

void Players::logAllPlayersToConsole() {
	if (this->getPlayerCount() > 0) {

		playerLock.lock();
		std::vector<Player*>::iterator it;
		int playerCounter = 0;
		do {
			Player* player = this->players.at(playerCounter);

			std::wstring outStr = L"Name=";
			outStr += player->getPlayerName();

			outStr += L", PeerIndex=";
			std::wostringstream ws;
			ws << player->getPeerIndex();
			outStr += ws.str();

			outStr += L", Team=";
			std::wostringstream ws2;
			ws2 << player->getPlayerTeam();
			outStr += ws2.str();

			outStr += L", ip=";
			std::string ipNonUnicode = player->getIp();
			std::wstring ip(ipNonUnicode.begin(), ipNonUnicode.end());
			outStr += ip;

			outStr += L", playerIndex=";
			std::wostringstream ws3;
			ws3 << player->getPlayerIndex();
			outStr += ws3.str();

			outStr += L", gunGameLevel=";
			std::wostringstream ws4;
			ws4 << player->getGunGameLevel();
			outStr += ws4.str();

			//outStr += L", identifier=";
			//std::wostringstream ws4;
			//ws4 << player.getPlayerIdentifier();
			//outStr += ws4.str();

			commands->output(outStr);
			playerCounter++;
		} while (playerCounter < this->getPlayerCount());
		playerLock.unlock();
	}
}


void Players::iteratePlayers(PlayerIterator* playerIterator) {
	if (this->getPlayerCount() > 0) {
		playerLock.lock();
		std::vector<Player*>::iterator it;
		int playerCounter = 0;
		do {
			Player* player = this->players.at(playerCounter);
			playerIterator->onIteratePlayer(player);
			playerCounter++;
		} while (playerCounter < this->getPlayerCount());
		playerLock.unlock();
	}
}

void Players::lookupPlayer(PlayerLookup* playerLookup)
{
	if (this->getPlayerCount() > 0) {
		playerLock.lock();
		std::vector<Player*>::iterator it;
		int playerCounter = 0;
		do {
			Player* player = this->players.at(playerCounter);
			playerLookup->lookup(player);
			if (playerLookup->foundPlayer) {
				break;
			}
			playerCounter++;
		} while (playerCounter < this->getPlayerCount());
		playerLock.unlock();
	}
}
