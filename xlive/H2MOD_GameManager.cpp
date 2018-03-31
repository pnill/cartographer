#include "Globals.h"
#include <h2mod.pb.h>

using namespace std; 

std::string EMPTY_STR2("");

void startGameThread() {
	while (1) {
		if (!gameManager->isHost()) {
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
						case H2ModPacket_Type_map_info_request:
							TRACE_GAME("[h2mod-network] Received map info packet");
							if (recvpak.has_map_info()) {
								std::string mapFilename = recvpak.map_info().mapfilename();
								if (!mapFilename.empty()) {
									TRACE_GAME_N("[h2mod-network] map file name from packet %s", mapFilename.c_str());
									if (!mapManager->hasCustomMap(mapFilename)) {
										//TODO: set map filesize
										//TODO: if downloading from repo files, try p2p
										mapManager->downloadFromRepo(mapFilename);
									}
								}
							}
						default:
							//TODO: log error
							break;
						}
					}
				}
			}

			std::string mapFilenameToDownload = mapManager->getMapFilenameToDownload();
			if (!mapFilenameToDownload.empty()) {
				TRACE_GAME_N("[h2mod-network] map file name from membership packet %s", mapFilenameToDownload.c_str());
				if (!mapManager->hasCustomMap(mapFilenameToDownload)) {
					//TODO: set map filesize
					//TODO: if downloading from repo files, try p2p
					mapManager->downloadFromRepo(mapFilenameToDownload);
				}
				mapManager->setMapFileNameToDownload(EMPTY_STR2);
			}
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