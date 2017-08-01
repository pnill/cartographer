#include "Globals.h"
#include <Shlwapi.h>
#include <fstream>
#include <sstream>
#include <string>

ChatBoxCommands::ChatBoxCommands() {
}

void ChatBoxCommands::checkForIds() {
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
}

void ChatBoxCommands::spawn(unsigned int object_datum, int count, float x, float y, float z, float randomMultiplier) {

	for (int i = 0; i < count; i++) {
		try {
			char* nObject = new char[0xC4];
			DWORD dwBack;
			VirtualProtect(nObject, 0xC4, PAGE_EXECUTE_READWRITE, &dwBack);

			if (object_datum) {
				unsigned int player_datum = h2mod->get_unit_datum_from_player_index(0);
				call_object_placement_data_new(nObject, object_datum, player_datum, 0);
				*(float*)(nObject + 0x1C) = h2mod->get_player_x(0, true) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				*(float*)(nObject + 0x20) = h2mod->get_player_y(0, true) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				*(float*)(nObject + 0x24) = (h2mod->get_player_z(0, true) + 5.0f) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				TRACE_GAME("object_datum = %08X, x=%d, y=%d, z=%d", object_datum, *(float*)(nObject + 0x1C), *(float*)(nObject + 0x20), *(float*)(nObject + 0x24));
				unsigned int object_gamestate_datum = call_object_new(nObject);
				call_add_object_to_sync(object_gamestate_datum);
			}
			delete[] nObject;
		}
		catch (...) {
			TRACE_GAME_N("Error running spawn command");
		}
	}
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
		else if (firstCommand == "$resetspawncommandlist") {
			//reset checked_for_ids, so you can reload new object_datums at runtime
			this->checked_for_ids = false;
		}
		else if (firstCommand == "$maxplayers") {
			if (splitCommands.size() != 2) {
				h2mod->write_inner_chat_dynamic(L"Usage: $maxplayers value (betwen 1 and 16).");
				return;
			}
			extern bool isHost;
			if (!isHost) {
				h2mod->write_inner_chat_dynamic(L"Can be only used while hosting.");
				return;
			} 

			std::string secondArg = splitCommands[1];
			int maxPlayersSet = stoi(splitCommands[1]);

			int baseAddr = (int)*(int*)((char*)h2mod->GetBase() + 0x420FE8);
			BYTE& playerNumber = *(BYTE*)(baseAddr + 0x1254);
			BYTE& maxPlayersNumber = *(BYTE*)(baseAddr + 0x4C80);
			
			if (maxPlayersSet < 1 || maxPlayersSet > 16) {
				h2mod->write_inner_chat_dynamic(L"The value needs to be between 1 and 16.");
				return;
			}

		    if (maxPlayersSet < playerNumber) {
				h2mod->write_inner_chat_dynamic(L"You can't set a value of max players smaller than the actual number of players on the server.");
				return;
			}
			else { 
				maxPlayersNumber = maxPlayersSet;
				h2mod->write_inner_chat_dynamic(L"Maximum players set");
			}
			
		}
		else if (firstCommand == "$setfov") {
			if (splitCommands.size() != 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid input.Usage - $setfov value");
				return;
			}
			else {
				std::string secondArg = splitCommands[1];
				unsigned int fov = stoi(splitCommands[1]);
				Field_of_View(fov, 1);
				h2mod->write_inner_chat_dynamic(L"Field of view set");
			}
		}
		else if (firstCommand == "$spawnnear") {
			if (splitCommands.size() < 3 || splitCommands.size() > 4) {
				h2mod->write_inner_chat_dynamic(L"Invalid command, usage $spawn command_name count");
				return;
			}

			if (isLobby && !h2mod->Server) {
				//TODO: need a nicer way to detect this for dedis
				h2mod->write_inner_chat_dynamic(L"Can only be used ingame");
				return;
			}

			//lookup a commands.txt file that contain string->object_datums
			checkForIds();

			std::string secondArg = splitCommands[1];
			std::string thirdArg = splitCommands[2];
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

			float randomMultiplier = 1.0f;
			if (splitCommands.size() == 4) {
				//optional multiplier provided
				std::string fourthArg = splitCommands[3];
				randomMultiplier = stoi(fourthArg);
			}

			int count = 1;
			try {
				count = stoi(thirdArg);
			}
			catch (...) {
				TRACE_GAME("Error converting string to int");
			}
			//since these are server only commands, the server would player index 1, which these are the position values for
			float x = *(float*)(h2mod->GetBase() + 0x4C072C);
			float y = *(float*)(h2mod->GetBase() + 0x4C0728);
			float z = *(float*)(h2mod->GetBase() + 0x4C0730);
			this->spawn(object_datum, count, x+=0.5f, y+=0.5f, z+=0.5f, randomMultiplier);
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
			checkForIds();

			std::string secondArg = splitCommands[1];
			std::string thirdArg = splitCommands[2];
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

			int count = stoi(thirdArg);
			int x = stoi(splitCommands[3]);
			int y = stoi(splitCommands[4]);
			int z = stoi(splitCommands[5]);

			this->spawn(object_datum, count, x, y, z, 1.0f);
		}
	}
}