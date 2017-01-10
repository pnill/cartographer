#include "Globals.h"
#include <Shlwapi.h>
#include <fstream>
#include <sstream>
#include <string>

ChatBoxCommands::ChatBoxCommands() {
}

/*
* Handles the given string command
* Returns a bool indicating whether the command is a valid command or not
*/
void ChatBoxCommands::handle_command(std::string command) {
	//split by a space
	std::vector<std::string> splitCommands = split(command, ' ');
	if (splitCommands.size() != 0) {
		std::string firstCommand = splitCommands[0];
		std::transform(firstCommand.begin(), firstCommand.end(), firstCommand.begin(), ::tolower);
		if (firstCommand == "$reloadmaps") {
			if (splitCommands.size() != 1) {
				h2mod->write_inner_chat_dynamic(L"Invalid command, usage - $reloadMaps");
				return;
			}

			mapManager->reloadMaps();
		}
		else if (firstCommand == "$resetSpawnCommandList") {
			//reset checked_for_ids, so you can reload new object_datums at runtime
			this->checked_for_ids = false;
		}
		else if (firstCommand == "$spawn") {
			if (splitCommands.size() != 6) {
				h2mod->write_inner_chat_dynamic(L"Invalid command, usage $spawn command_name count x y z");
				return;
			}

			if (isLobby && !h2mod->Server) {
				//TODO: need a nicer way to detect this for dedis
				h2mod->write_inner_chat_dynamic(L"Can only be used ingame");
				return;
			}

			//lookup a commands.txt file that contain string->object_datums
			if (!checked_for_ids) {
				//only check once per game
				checked_for_ids = true;
				std::ifstream infile("commands.txt");
				if (infile.good()) {
					std::string line;
					while (std::getline(infile, line)) {
						std::vector<std::string> command = split(line, ',');
						if (command.size() != 2) {
							TRACE_GAME("Found line not comma separated right, %s", line);
						}
						else {
							object_ids[command[0]] = strtoul(command[1].c_str(), NULL, 0);
						}
					}
				}
			}

			std::string secondArg = splitCommands[1];
			std::string thirdArg = splitCommands[2];
			int count = stoi(thirdArg);
			int x = stoi(splitCommands[3]);
			int y = stoi(splitCommands[4]);
			int z = stoi(splitCommands[5]);
			unsigned int object_datum;
			if (object_ids.find(secondArg) == object_ids.end()) {
				//read from chatbox line
				std::string secondArg = splitCommands[1];
				object_datum = strtoul(secondArg.c_str(), NULL, 0);
			}
			else {
				//read from object_id map
				object_datum = object_ids[secondArg];
			}
			printf("object_datum: %08X\n", object_datum);
			TRACE_GAME("object_datum = %08X", object_datum);

			for (int i = 0; i < count; i++) {
				try {
					char* nObject = new char[0xC4];
					DWORD dwBack;
					VirtualProtect(nObject, 0xC4, PAGE_EXECUTE_READWRITE, &dwBack);

					if (object_datum)
					{
						unsigned int player_datum = h2mod->get_unit_datum_from_player_index(0);
						call_object_placement_data_new(nObject, object_datum, player_datum, 0);
						*(float*)(nObject + 0x1C) = x + (5 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
						*(float*)(nObject + 0x20) = y + (5 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
						*(float*)(nObject + 0x24) = z + (5 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
						unsigned int object_gamestate_datum = call_object_new(nObject);
						call_add_object_to_sync(object_gamestate_datum);
					}
				}
				catch (...) {
					TRACE_GAME_N("Error running spawn command");
				}
			}
		}
	}
}