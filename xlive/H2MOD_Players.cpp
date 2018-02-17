#include "Globals.h"
#include <mutex>
#include <h2mod.pb.h>

int Players::getPlayerOffset() {
	//TODO: add support for dedis
	int playerDataOffset = (*(int*)(h2mod->GetBase() + 0x51C474));
	int playerDataOffsetTmp = *(DWORD *)(playerDataOffset + 29600);
	if (playerDataOffsetTmp == 5 || playerDataOffsetTmp == 6 || playerDataOffsetTmp == 7 || playerDataOffsetTmp == 8) {
		//valid memory offset
	}

	playerDataOffset = (*(int*)(h2mod->GetBase() + 0x51C474)) + 31624; 
}

int Players::getPlayerCount() {
	//TODO: add support for dedis
	int playerDataOffset = this->getPlayerOffset();
	int playerOrPeerCount = *(DWORD *)(playerDataOffset + 0x14);
	return playerOrPeerCount;
}

void Players::reload() {
	playerLock.lock();

	std::unordered_map<unsigned long long, int> identifierToGunGameLevel;
	std::unordered_map<unsigned long long, BOOL> identifierToIsZombie;
	std::vector<Player*>::iterator it;
	int i = 0;
	for (it = this->players.begin(); it != this->players.end(); it++, i++) {
		Player* player = *it;
		identifierToGunGameLevel[player->getPlayerIdentifier()] = player->getGunGameLevel();
		identifierToIsZombie[player->getPlayerIdentifier()] = player->getIsZombie();
	}

	//wipe out the existing players off the heap
	i = 0;
	for (it = this->players.begin(); it != this->players.end(); it++, i++) {
		delete *it;
	}
	players.clear();

	int playerDataOffset = this->getPlayerOffset();
	int peerStart = playerDataOffset + 160;
	int playerStart = playerDataOffset + 4608;
	int playerCounter = 0;
	do {
		//from everything I can tell in h2_set_membership_packet_data(), peer and player index don't actually ever
		//change, the people who join occupy spots and the other info is stored in those spots, but there is no shifting
		//of these indexes for in game players
		int peerIndex = playerCounter;
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
		Player* player = new Player(peerAddress, peerName, peerIndex, playerTeam, playerIndex, identifier);
		player->setGunGameLevel(identifierToGunGameLevel[identifier]);
		player->setIsZombie(identifierToIsZombie[identifier]);
		this->players.push_back(player);
		playerCounter++;
	} while (playerCounter < this->getPlayerCount());

	playerLock.unlock();

	//clear temp maps to carry over data
	identifierToGunGameLevel.clear();
	identifierToIsZombie.clear();
}

void Players::logAllPlayersToConsole() {
	if (this->getPlayerCount() > 0) {

		playerLock.lock();
		std::vector<Player*>::iterator it;
		int i = 0;
		for (it = this->players.begin(); it != this->players.end(); it++, i++) {
			Player* player = *it;

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
		}
		playerLock.unlock();
	}
}

void Players::iteratePlayers(PlayerIterableHandler* playerHandler)
{
	if (this->getPlayerCount() > 0) {
		playerLock.lock();
		std::vector<Player*>::iterator it;
		int i = 0;
		for (it = this->players.begin(); it != this->players.end(); it++, i++) {
			Player* player = *it; 
			playerHandler->onPlayerIterated(player);
			if (playerHandler->stopIteration) {
				break;
			}
		}
		playerLock.unlock();
	}
}
