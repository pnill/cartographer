#include "Globals.h"
#include <Shlwapi.h>
#include <fstream>
#include <sstream>
#include <string>
#include <h2mod.pb.h>
#include "H2Startup.h"
#include "H2Tweaks.h"
#include "H2Config.h"

ConsoleCommands::ConsoleCommands() {
	command = "";
	caretPos = 0;
}

void ConsoleCommands::writePreviousCommand(std::string msg) {
	std::vector<std::string>::iterator it;
	it = this->prevCommands.begin();
	this->prevCommands.insert(it, msg);

	if (this->prevCommands.size() > 18) {
		this->prevCommands.pop_back();
	}
}

time_t start = time(0);
BOOL ConsoleCommands::handleInput(WPARAM wp) {
	if (H2hWnd != GetForegroundWindow()) {
		//halo2 is not in focus
		return false;
	}
	double seconds_since_start = difftime(time(0), start);
	if (wp == H2Config_hotkeyIdConsole) {
		if (seconds_since_start > 0.5) {
			this->console = !this->console;
			start = time(0);
		}
		return true;
	}
	switch (wp) {
	case '\b':   // backspace
	{
		if (this->console) {
			if (this->caretPos > 0)
			{
				this->command.erase(this->caretPos - 1, 1);
				this->caretPos -= 1;
			}
			return true;
		}
	}
	break;

	case '\r':    // return/enter
	{
		if (console) {
			this->writePreviousCommand(this->command);

			std::string fullCommand("$");
			fullCommand += this->command;
			this->handle_command(fullCommand);

			command = "";
			caretPos = 0;
			return true;
		}
	}
	break;

	default:
		if (this->console) {
			//0x30 = 0
			//0x5A = z
			//0x20 = spacebar

			if (((wp >= 0x30 && wp <= 0x5A) || wp == 0x20 || wp == VK_OEM_MINUS)) {
				if (wp == VK_OEM_MINUS) {
					wp = '-';
				}
				//0x10 = shift
				//0xA0 = left shift
				//0xA1 = right shift
				if (GetAsyncKeyState(0x10) & 0x8000 || GetAsyncKeyState(0xA0) & 0x8000 || GetAsyncKeyState(0xA1) & 0x8000) {
					if (wp == '-') {
						wp = '_';
					}
					else {
						wp = toupper(wp);
					}
				}
				else {
					wp = tolower(wp);
				}
				this->command.insert(this->caretPos, 1, (char)wp);
				this->caretPos += 1;
				return true;
			}
		}
		break;
	}
	return false;
}

void ConsoleCommands::checkForIds() {
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

void ConsoleCommands::spawn(unsigned int object_datum, int count, float x, float y, float z, float randomMultiplier) {

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

void ConsoleCommands::output(std::wstring result) {
	if (h2mod->Server) {
		h2mod->logToDedicatedServerConsole((wchar_t*)result.c_str());
	}
	else {
		std::string str(result.begin(), result.end());
		writePreviousCommand(str);
	}
}

void ConsoleCommands::display(std::string output)
{
	writePreviousCommand(output);
}

bool ConsoleCommands::isNum(const char *s) {
	int i = 0;
	while (s[i]) {
		//if there is a letter in a string then string is not a number
		if (isalpha(s[i])) {
			return false;
		}
		i++;
	}
	return true;
}

void downloadFromRepoThruConsole(std::string mapFilename) {
	mapManager->downloadFromRepo(mapFilename);
}

/*
* Handles the given string command
* Returns a bool indicating whether the command is a valid command or not
*/
void ConsoleCommands::handle_command(std::string command) {
	//split by a space
	std::vector<std::string> splitCommands = split(command, ' ');

	if (splitCommands.size() != 0) {
		std::string firstCommand = splitCommands[0];
		std::transform(firstCommand.begin(), firstCommand.end(), firstCommand.begin(), ::tolower);
		if (firstCommand == "$reloadmaps") {
			if (splitCommands.size() != 1) {
				output(L"Invalid command, usage - $reloadMaps");
				return;
			}

			mapManager->reloadMaps();
		}
		else if (firstCommand == "$help") {
			output(L"reloadMaps");
			output(L"kick");
			output(L"logPlayers");
			output(L"maxPlayers");
			output(L"resetSpawnCommandList");
			output(L"isHost");
			output(L"downloadMap");
			output(L"spawn");
			output(L"controller_sens");
			output(L"mouse_sens");
		}
		else if (firstCommand == "$downloadmap") {
			if (splitCommands.size() != 2) {
				output(L"Invalid download map command, usage - $downloadMap MAP_NAME");
				return;
			}
			std::string firstArg = splitCommands[1];
			mapManager->downloadFromRepo(firstArg);
		}
		else if (firstCommand == "$kick") {
			if (h2mod->Server) {
				output(L"Don't use this on dedis");
				return;
			}
			if (splitCommands.size() != 2) {
				output(L"Invalid kick command, usage - $kick PLAYER_INDEX");
				return;
			}
			std::string firstArg = splitCommands[1];
			char *cstr = new char[firstArg.length() + 1];
			strcpy(cstr, firstArg.c_str());

			if (!gameManager->isHost()) {
				output(L"Only the server can kick players");
			}
			else {
				if (isNum(cstr)) {
					int peerIndex = atoi(cstr);
					if (peerIndex == 0) {
						output(L"Don't kick yourself");
						return;
					}
					h2mod->kick_player(peerIndex);
				}
			}
			delete[] cstr;
		}
		else if (firstCommand == "$logplayers") {
			if (!gameManager->isHost()) {
				output(L"Only host can log out information about players");
				return;
			}
			players->logAllPlayersToConsole();
		}
		else if (firstCommand == "$sendteamchange") {
			if (!gameManager->isHost()) {
				output(L"Only host can issue a team change");
				return;
			}
			if (splitCommands.size() != 3) {
				output(L"Usage: $sendTeamChange peerIndex newTeam");
				return;
			}
			std::string firstArg = splitCommands[1];
			std::string secondArg = splitCommands[2];

			H2ModPacket teampak;
			teampak.set_type(H2ModPacket_Type_set_player_team);

			h2mod_set_team *set_team = teampak.mutable_h2_set_player_team();
			set_team->set_team(atoi(secondArg.c_str()));

			char* SendBuf = new char[teampak.ByteSize()];
			teampak.SerializeToArray(SendBuf, teampak.ByteSize());

			network->networkCommand = SendBuf;
			network->sendCustomPacket(atoi(firstArg.c_str()));

			network->networkCommand = NULL;
			delete[] SendBuf;
		}
		else if (firstCommand == "$maxplayers") {
			if (splitCommands.size() != 2) {
				output(L"Usage: $maxplayers value (betwen 1 and 16).");
				return;
			}
			if (!gameManager->isHost()) {
				output(L"Can be only used while hosting.");
				return;
			}

			std::string firstArg = splitCommands[1];
			char *cstr = new char[firstArg.length() + 1];
			strcpy(cstr, firstArg.c_str());

			DWORD lobby_globals = *(DWORD*)((char*)h2mod->GetBase() + 0x420FE8);
			BYTE playerNumber = *(BYTE*)(lobby_globals + 0x1254);

			if (isNum(cstr)) {
				delete[] cstr;

				int maxPlayersSet = stoi(firstArg);
				if (maxPlayersSet < 1 || maxPlayersSet > 16) {
					output(L"The value needs to be between 1 and 16.");
					return;
				}

				if (maxPlayersSet < playerNumber) {
					output(L"You can't set a value of max players smaller than the actual number of players on the server.");
					return;
				}
				else {
					*(BYTE*)(lobby_globals + 0x4C80) = maxPlayersSet;
					output(L"Maximum players set");
					return;
				}
			}
			delete[] cstr;
		}
		else if (firstCommand == "$resetspawncommandlist") {
			//reset checked_for_ids, so you can reload new object_datums at runtime
			this->checked_for_ids = false;
		}
		else if (firstCommand == "$spawnnear") {
			if (splitCommands.size() < 3 || splitCommands.size() > 4) {
				output(L"Invalid command, usage $spawn command_name count");
				return;
			}

			if (isLobby && !h2mod->Server) {
				//TODO: need a nicer way to detect this for dedis
				output(L"Can only be used ingame");
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
				randomMultiplier = stof(fourthArg);
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

			this->spawn(object_datum, count, x += 0.5f, y += 0.5f, z += 0.5f, randomMultiplier);
		}
		else if (firstCommand == "$ishost") {
			int packetDataObj = (*(int*)(h2mod->GetBase() + 0x420FE8));
			std::wstring isHostStr = L"isHost=";
			DWORD isHostByteValue = *(DWORD*)(packetDataObj + 29600);
			std::wostringstream ws;
			ws << isHostByteValue;
			const std::wstring s(ws.str());
			isHostStr += (gameManager->isHost() ? L"yes" : L"no");
			isHostStr += L",value=";
			isHostStr += s;
			output(isHostStr);
		}
		else if (firstCommand == "$downloadmap") {
			if (splitCommands.size() != 2 && !splitCommands[1].empty()) {
				output(L"Invalid command, usage downloadMap filename");
				return;
			}
			std::string secondArg = splitCommands[1];
			secondArg += ".map";
			std::thread t1(downloadFromRepoThruConsole, secondArg);
			t1.detach();
		}
		else if (firstCommand == "$spawn") {
			if (splitCommands.size() != 6) {
				output(L"Invalid command, usage $spawn command_name count x y z");
				return;
			}

			if (isLobby && !h2mod->Server) {
				//TODO: need a nicer way to detect this for dedis
				output(L"Can only be used ingame");
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
			float x = stof(splitCommands[3]);
			float y = stof(splitCommands[4]);
			float z = stof(splitCommands[5]);

			this->spawn(object_datum, count, x, y, z, 1.0f);
		}
		else if (firstCommand == "$controller_sens") {
			if (splitCommands.size() != 2) {
				output(L"Invalid usage, usage $controller_sens value");
				return;
			}
			std::string sensVal = splitCommands[1];
			char *cstr = new char[sensVal.length() + 1];
			strcpy(cstr, sensVal.c_str());

			if (isNum(cstr)) {
				setSens(CONTROLLER, stoi(sensVal)); 
				H2Config_sens_controller = stoi(sensVal);
			}
			else {
				output(L"Wrong input! Use a number.");
			}
			delete[] cstr;
		}
		else if (firstCommand == "$mouse_sens") {
			if (splitCommands.size() != 2) {
				output(L"Invalid usage, usage $mouse_sens value");
				return;
			}
			std::string sensVal = splitCommands[1];
			char *cstr = new char[sensVal.length() + 1];
			strcpy(cstr, sensVal.c_str());

			if (isNum(cstr)) {
				setSens(MOUSE, stoi(sensVal));
				H2Config_sens_mouse = stoi(sensVal);
			}
			else {
				output(L"Wrong input! Use a number.");
			}
			delete[] cstr;
		}
		else {
			output(L"Unknown command.");
		}
	}
}