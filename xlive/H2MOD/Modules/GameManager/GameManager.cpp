#include "Globals.h"
#include <H2MOD\protobuf\h2mod.pb.h>
#include "H2MOD\Modules\MapChecksum\MapChecksumSync.h"

using namespace std::chrono_literals;

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
					recvpak.ParseFromArray(command.c_str(), command.size());
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
							if (recvpak.has_map_info()) {
								std::string mapFilename = recvpak.map_info().mapfilename();
								if (!mapFilename.empty()) {
									TRACE_GAME_N("[h2mod-network] Received map file name from packet: %s", mapFilename.c_str());
									if (!mapManager->hasCustomMap(mapFilename)) {
										//TODO: set map filesize
										mapManager->setMapFileNameToDownload(mapFilename);
									}
									else {
										TRACE_GAME_N("[h2mod-network] already has map %s", mapFilename.c_str());
									}
								}
							}
							break;
						case H2ModPacket_Type_set_lobby_settings:
							TRACE_GAME("[h2mod-network] Received lobby settings packet");
							if (recvpak.has_lobby_settings()) {
								advLobbySettings->parseLobbySettings(recvpak.lobby_settings());
							}
							break;
						default:
							TRACE_GAME("[h2mod-network] Unknown packet type %d", recvpak.type());
							break;
						}
					}
				}
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