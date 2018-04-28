#include "Globals.h"
#include <h2mod.pb.h>

using namespace std; 

time_t start2 = time(0);

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
								//protobuf has some weird bug where passing 0 has type in the current set_grenade packet definition
								//completely breaks on the serialization side, https://groups.google.com/forum/#!topic/protobuf/JsougkaRcw4
								//no idea why, so we always subtract 1 here (since we added 1 before save)
								int type = recvpak.set_grenade().type() - 1;
								int count = recvpak.set_grenade().count();
								int pIndex = recvpak.set_grenade().pindex();
								TRACE_GAME("[h2mod-network] grenades packet info type=%d, count=%d, pIndex=%d", type, count, pIndex);
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
									else {
										TRACE_GAME_N("[h2mod-network] already has map %s", mapFilename.c_str());
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
				else {
					TRACE_GAME_N("[h2mod-network] already has map %s", mapFilenameToDownload.c_str());
				}
				mapManager->setMapFileNameToDownload(EMPTY_STR2);
			}
		} else {
			double secondsPassed = difftime(time(0), start2);
			if (secondsPassed > 5) {
				//send map info every 5 seconds if somehow membership packets don't send the map filename
				mapManager->sendMapInfoPacket();
				start2 = time(0);
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