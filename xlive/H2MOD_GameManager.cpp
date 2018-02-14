#include "Globals.h"
#include <h2mod.pb.h>

using namespace std;

void startGameThread() {
	while (1) {
		if (!gameManager->isHost()) {
			if (!mapManager->hasCustomMap(mapManager->clientMapFilename)) {
				bool success = mapManager->downloadFromRepo(mapManager->clientMapFilename);
				//TODO: if downloading from repo fails, try to download directly from host
				//if its supported for this host
				//TODO: direct downloads happen once per map name, we should only download repo maps once per joined game so we don't hammer
				//the repo server
			}
			if (!network->queuedNetworkCommands.empty()) {
				std::deque<std::string>::iterator it = network->queuedNetworkCommands.begin();
				std::string command = *it;
				network->queuedNetworkCommands.pop_front();
				if (!command.empty()) {
					H2ModPacket recvpak;
					recvpak.ParseFromArray(command.c_str(), strlen(command.c_str()));
					if (recvpak.has_type()) {
						switch (recvpak.type()) {
						case H2ModPacket_Type_set_player_team:
							TRACE_GAME("[h2mod-network] Received change player team packet");
							if (recvpak.has_h2_set_player_team()) {
								BYTE TeamIndex = recvpak.h2_set_player_team().team();
								TRACE_GAME("[h2mod-network] Got a set team request from server! TeamIndex: %i", TeamIndex);
								h2mod->set_local_team_index(TeamIndex);
							}
							break;
						case H2ModPacket_Type_set_unit_grenades:
							TRACE_GAME("[h2mod-network] Received change player grenades packet");
							if (recvpak.has_set_grenade())
							{
								BYTE type = recvpak.set_grenade().type();
								BYTE count = recvpak.set_grenade().count();
								BYTE pIndex = recvpak.set_grenade().pindex();

								h2mod->set_local_grenades(type, count, pIndex);
							}
							break;
						default:
							//TODO: log error
							break;
						}
					}
				}
			}
		}
		else {
			//reload player data if host
			players->reload();
		}
		std::this_thread::sleep_for(1s);
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