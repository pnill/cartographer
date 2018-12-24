#include "Globals.h"
#include <fstream>
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Config\Config.h"
#include "Blam/BlamLibrary.h"
#include "H2MOD\Modules\CustomMenu\Credits.h"


std::wstring ERROR_OPENING_CLIPBOARD(L"Error opening clipboard");

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

	//copy/paste functionality
	case 0x56:
		{
			//read 'V' before 'CTRL'
			if (console && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
				if (!OpenClipboard(NULL)) {
					output(ERROR_OPENING_CLIPBOARD);
					return false;
				}

				HANDLE clipboardHandle = GetClipboardData(CF_TEXT);
				//lock the clipboard
				std::string clipboardContent = (LPSTR)GlobalLock(clipboardHandle);
				//unlock  the clipboard
				GlobalUnlock(clipboardHandle);
				CloseClipboard();
				this->command += clipboardContent;
				this->caretPos = this->command.length();
			}
		}
		break;

	default:
		if (console) {

			if (wp == VK_UP)
			{
				command = "";
				caretPos = 0;
				if (prevCommands.size() > 0 && previous_command_index <= prevCommands.size() - 1)
				{
					command = prevCommands[previous_command_index];
					caretPos = command.length();
					previous_command_index++;
				}
				return true;
			}

			if (wp == VK_DOWN)
			{
				command = "";
				caretPos = 0;

				if (prevCommands.size() > 0 && previous_command_index > 0)
				{
					previous_command_index--;
					command = prevCommands[previous_command_index];
					caretPos = command.length();
				}
				return true;
			}

			if (wp == VK_END)
			{
				caretPos = command.length();
				return true;
			}

			if (wp == VK_HOME)
			{
				caretPos = 0;
				return true;
			}

			if (wp == VK_LEFT)
			{
				if (caretPos)
					caretPos--;

				return true;
			}

			if (wp == VK_RIGHT)
			{
				if (caretPos != command.length())
					caretPos++;

				return true;
			}


			if (((wp >= 0x30 && wp <= 0x5A) || wp == 0x20 || wp == VK_OEM_MINUS || wp == VK_OEM_COMMA || wp == VK_OEM_PERIOD || wp == VK_OEM_MINUS || wp == VK_OEM_PLUS ||
				wp == VK_OEM_1 || wp == VK_OEM_2 || wp == VK_OEM_3 || wp == VK_OEM_4 || wp == VK_OEM_5 || wp == VK_OEM_6 || wp == VK_OEM_7)) {



				switch (wp)
				{
				case VK_OEM_COMMA:
					wp = ',';
					break;

				case VK_OEM_PERIOD:
					wp = '.';
					break;

				case VK_OEM_MINUS:
					wp = '-';
					break;

				case VK_OEM_1:
					wp = ';';
					break;

				case VK_OEM_2:
					wp = '/';
					break;

				case VK_OEM_4:
					wp = '[';
					break;

				case VK_OEM_5:
					wp = '\\';
					break;

				case VK_OEM_6:
					wp = ']';
					break;

				case VK_OEM_7:
					wp = '\'';
					break;

				case VK_OEM_PLUS:
					wp = '=';
					break;
				}



				if (GetAsyncKeyState(0x10) & 0x8000 || GetAsyncKeyState(0xA0) & 0x8000 || GetAsyncKeyState(0xA1) & 0x8000) {

					switch (wp)
					{
					case '1':
						wp = '!';
						break;

					case '-':
						wp = '_';
						break;

					case '2':
						wp = '@';
						break;

					case '3':
						wp = '#';
						break;

					case '4':
						wp = '$';
						break;

					case '5':
						wp = '%';
						break;

					case '6':
						wp = '^';
						break;

					case '7':
						wp = '&';
						break;

					case '8':
						wp = '*';
						break;

					case '9':
						wp = '(';
						break;

					case '0':
						wp = ')';
						break;

					case '=':
						wp = '+';
						break;

					case '[':
						wp = '{';
						break;

					case ']':
						wp = '}';
						break;

					case '\'':
						wp = '"';
						break;

					case ';':
						wp = ':';
						break;

					case ',':
						wp = '<';
						break;

					case '.':
						wp = '>';
						break;

					case '/':
						wp = '?';
						break;

					case '\\':
						wp = '|';
						break;



					default:
						wp = toupper(wp);
						break;
					}

				}
				else {
					wp = tolower(wp);
				}
				command.insert(caretPos, 1, (char)wp);
				caretPos += 1;
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
					TRACE_GAME("Found line not comma separated right, %s", line.c_str());
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
		result = result + L"\n";
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

int __cdecl call_get_object_via_datum(DatumIndex object_datum_index, int object_type)
{
	//TRACE_GAME("call_get_object( object_datum_index: %08X, object_type: %08X )", object_datum_index, object_type);

	typedef int(__cdecl *get_object)(DatumIndex object_datum_index, int object_type);
	get_object pget_object = (get_object)((char*)h2mod->GetBase() + ((h2mod->Server) ? 0x11F3A6 : 0x1304E3));

	return pget_object(object_datum_index, object_type);
}

int __cdecl call_entity_datum_to_gamestate_datum(int entity_datum)
{
	typedef int(__cdecl *entity_datum_to_gamestate_datum)(int entity_datum);
	entity_datum_to_gamestate_datum pentity_datum_to_gamestate_datum = (entity_datum_to_gamestate_datum)((BYTE*)h2mod->GetBase() + 0x1F2211);

	return pentity_datum_to_gamestate_datum(entity_datum);
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
		else if (firstCommand == "$mapfilename")
		{
			std::string map_name_str = mapManager->getMapFilename();
			std::wstring mapname(map_name_str.begin(), map_name_str.end());
			std::wstring mapinternalname = mapManager->getMapName();
			output(mapname);
			output(mapinternalname);
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
			if (!gameManager->isHost()) {
				output(L"Only the server can kick players");
				return;
			}

			std::string firstArg = splitCommands[1];

			if (isNum(firstArg.c_str())) {
				int peerIndex = atoi(firstArg.c_str());
				if (peerIndex == 0) {
					output(L"Don't kick yourself");
					return;
				}
				h2mod->kick_player(peerIndex);
			}
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

			//network->send_h2mod_packet_player(atoi(firstArg.c_str()), teampak);
		}
		else if (firstCommand == "$menu_test") {
			//CreditsMenu_list *menu_test = new CreditsMenu_list(0xFF000006);
		
		}
		else if (firstCommand == "$test_player_ptr") {

			wchar_t buf[2048];

			int local_player_datum = h2mod->get_unit_datum_from_player_index(0);
			int local_player_ptr = call_get_object(local_player_datum, 3);
			
			using namespace Blam::EngineDefinitions::Players;
			using namespace Blam::EngineDefinitions::Objects;

			GameStatePlayerTable *test = (GameStatePlayerTable*)(*(DWORD*)(((BYTE*)h2mod->GetBase()+0x4A8260)));
			GameStateObjectHeaderTable* test2 = (GameStateObjectHeaderTable*)(*(DWORD*)(((BYTE*)h2mod->GetBase() + 0x4E461C)));
			GameStatePlayer *player1 = &test->players[0];
			GameStatePlayer *player2 = &test->players[1];
			
			GameStateObjectHeader *player1_object_header = &test2->object_header[player1->unit_index.ToAbsoluteIndex()];
			ObjectEntityDefinition *player1_object = player1_object_header->object;


			memset(buf, 0x00, sizeof(buf));
			
			output(buf);

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

			std::string secondArg = splitCommands[1];
			DWORD lobby_globals = *(DWORD*)((char*)h2mod->GetBase() + 0x420FE8);
			BYTE playerNumber = *(BYTE*)(lobby_globals + 0x1254);

			if (isNum(secondArg.c_str())) {

				int maxPlayersToSet = atoi(secondArg.c_str());
				if (maxPlayersToSet < 1 || maxPlayersToSet > 16) {
					output(L"The value needs to be between 1 and 16.");
					return;
				}
				if (maxPlayersToSet < playerNumber) {
					output(L"You can't set a value of max players smaller than the actual number of players on the server.");
					return;
				}
				else {
					*(BYTE*)(lobby_globals + 0x4C80) = static_cast<BYTE>(maxPlayersToSet);
					output(L"Maximum players set");
					return;
				}
			}
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
		else if (firstCommand == "$leavegame") {
			h2mod->exit_game();
		}
		else if (firstCommand == "$xyz") {
			if (!gameManager->isHost()) {
				output(L"Only host can see xyz for now...");
				return;
			}
			displayXyz = !displayXyz;
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
				H2Tweaks::setSens(CONTROLLER, stoi(sensVal));
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
				H2Tweaks::setSens(MOUSE, stoi(sensVal));
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