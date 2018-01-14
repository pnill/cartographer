#include "Globals.h"

void startGameThread() {
	while (1) {
		if (gameManager->isHost()) {
			//the server stores a map of unicode map names to actual filenames so when the server
			//sends over filenames we don't slow down the serializing the membership packets
			mapManager->reloadMapFilenames();
		}

		if (!gameManager->isHost()) {
			if (!mapManager->hasCustomMap(mapManager->clientMapFilename)) {
				bool success = mapManager->downloadFromRepo(mapManager->clientMapFilename);
				//TODO: if downloading from repo fails, try to download directly from host
				//if its supported for this host
				//TODO: direct downloads happen once per map name
			}
		}
		Sleep(1000);
	}
}

bool GameManager::isHost() {
	if (h2mod->Server) {
		return true;
	}
	int packetDataObj = (*(int*)(h2mod->GetBase() + 0x420FE8));
	DWORD isHostByteValue = *(DWORD*)(packetDataObj + 29600);
	return isHostByteValue == 5 || isHostByteValue == 6 || isHostByteValue == 7 || isHostByteValue == 8;
}

bool GameManager::start() {
	if (!this->started) {
		std::thread t1(startGameThread);
		t1.detach();
		this->started = true;
	}
	return this->started;
}