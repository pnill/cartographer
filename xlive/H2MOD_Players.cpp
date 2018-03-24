#include "Globals.h"
#include <mutex>
#include <h2mod.pb.h>

int Players::getPlayerStart(int index) {
	int playerDataOffset = this->getPlayerOffset();
	int playerStart = playerDataOffset + 4608;
	playerStart += index * 296;
	return playerStart;
}

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

int Players::getPeerIndex(int playerIndex)
{
	return *(DWORD*)(this->getPlayerStart(playerIndex) - 12);
}

wchar_t * Players::getPlayerName(int playerIndex)
{
	int newPlayerStart2 = this->getPlayerStart(playerIndex) + 136;
	return (wchar_t*)(newPlayerStart2);
}

long long Players::getPlayerIdentifier(int playerIndex)
{
	DWORD ident1 = *(DWORD *)(this->getPlayerStart(playerIndex) - 20);
	DWORD ident2 = *(DWORD *)(this->getPlayerStart(playerIndex) - 16);
	//the ident appears to made up of 8 bytes
	return (long long)ident1 << 32 | ident2;
}

int Players::getPlayerTeam(int playerIndex)
{
	int newPlayerStart2 = this->getPlayerStart(playerIndex) + 136;
	int playerTeam = *(BYTE *)(newPlayerStart2 + 124);
	return playerTeam;
}

int Players::getPlayerCount() {
	int playerDataOffset = this->getPlayerOffset();
	int playerOrPeerCount = *(DWORD *)(playerDataOffset + 0x14);
	//the player structure we use contains the dedicated server for whatever reason, so we always subtract player count by 1 when we are the dedi
	return playerOrPeerCount;
}

void Players::logAllPlayersToConsole() {
	if (this->getPlayerCount() > 0) {

		int playerCounter = 0;
		do {

			std::wstring outStr = L"Name=";
			outStr += this->getPlayerName(playerCounter);

			outStr += L", PeerIndex=";
			std::wostringstream ws;
			ws << this->getPeerIndex(playerCounter);
			outStr += ws.str();

			outStr += L", Team=";
			std::wostringstream ws2;
			ws2 << this->getPlayerTeam(playerCounter);
			outStr += ws2.str();

			outStr += L", playerIndex=";
			std::wostringstream ws3;
			ws3 << playerCounter;
			outStr += ws3.str();

			//outStr += L", gunGameLevel=";
			//std::wostringstream ws4;
			//ws4 << player->getGunGameLevel();
			//outStr += ws4.str();

			//outStr += L", identifier=";
			//std::wostringstream ws4;
			//ws4 << player.getPlayerIdentifier();
			//outStr += ws4.str();

			commands->output(outStr);
			playerCounter++;
		} while (playerCounter < this->getPlayerCount());
	}
}