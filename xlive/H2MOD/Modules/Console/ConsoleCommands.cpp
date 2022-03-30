#include "stdafx.h"

#include "ConsoleCommands.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"

#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Input\Mouseinput.h"
#include "H2MOD\Modules\MapManager\MapManager.h"
#include "H2MOD\Modules\ServerConsole\ServerConsole.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "H2MOD\Variants\GunGame\GunGame.h"

#include "H2MOD\GUI\imgui_integration\ImGui_Handler.h"
#include "Util\ClipboardAPI.h"
// for XNet connection logging
#include "XLive\xnet\IpManagement\XnIp.h"

std::wstring ERROR_OPENING_CLIPBOARD(L"Error opening clipboard");

ConsoleCommands* commands = new ConsoleCommands();

ConsoleCommands::ConsoleCommands() {
	command = "";
	caretPos = 0;
}

void ConsoleCommands::writePreviousCommand(std::string& msg) {
	this->prevCommands.insert(this->prevCommands.begin(), msg);
	if (this->prevCommands.size() > 6)
		this->prevCommands.pop_back();
}

void ConsoleCommands::writePreviousOutput(std::string& msg) {
	this->prevOutput.insert(this->prevOutput.begin(), msg);
	if (this->prevOutput.size() > 18)
		this->prevOutput.pop_back();
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
			resetCaretBlink();
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
				resetCaretBlink();
			}
			return true;
		}
	}
	break;

	case '\r':    // return/enter
	{
		if (console && command.length() > 1) {
			writePreviousOutput(this->command);
			writePreviousCommand(this->command);
			std::string fullCommand("$");
			fullCommand += command;
			this->handle_command(fullCommand);

			command = "";
			caretPos = 0;
			resetCaretBlink();
			previous_command_index = 0;
			return true;
		}
	}
	break;

	// copy/paste functionality
	case 0x56:
	{
		//read 'V' before 'CTRL'
		if (console && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {

			std::string clipboardContent;
			if (!ClipboardAPI::read(clipboardContent)) {
				output(ERROR_OPENING_CLIPBOARD);
				return false;
			}
			this->command += clipboardContent;
			this->caretPos = this->command.length();
			break;
		}
	}

	default:

		if (console) {

			if (wp == VK_UP)
			{
				if (prevCommands.size() > 0 && previous_command_index < prevCommands.size())
				{
					command = prevCommands[previous_command_index];
					caretPos = command.length();
					resetCaretBlink();
					previous_command_index++;
				}
				return true;
			}

			if (wp == VK_DOWN)
			{
				if (prevCommands.size() > 0 && previous_command_index > 0)
				{
					previous_command_index--;
					command = prevCommands[previous_command_index];
					caretPos = command.length();
					resetCaretBlink();
				}
				return true;
			}

			if (wp == VK_END)
			{
				caretPos = command.length();
				resetCaretBlink();
				return true;
			}

			if (wp == VK_HOME)
			{
				caretPos = 0;
				resetCaretBlink();
				return true;
			}

			if (wp == VK_LEFT)
			{
				if (caretPos)
					caretPos--;

				resetCaretBlink();

				return true;
			}

			if (wp == VK_RIGHT)
			{
				if (caretPos != command.length())
					caretPos++;

				resetCaretBlink();

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

bool ConsoleCommands::shouldCaretBlink()
{
	if (timeGetTime() - lastTimeCaretBlink >= caretBlinkTimeMs)
	{
		lastTimeCaretBlink = timeGetTime();
		caretBlinked = !caretBlinked;
		return caretBlinked;
	}

	return caretBlinked;
}

void ConsoleCommands::resetCaretBlink()
{
	caretBlinked = false;
	lastTimeCaretBlink = timeGetTime();
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
					LOG_TRACE_GAME("Found line not comma separated right, {}", line);
				}
				else {
					object_ids[command[0]] = strtoul(command[1].c_str(), NULL, 0);
				}
			}
		}
	}
}

void ConsoleCommands::spawn(datum object_idx, int count, float x, float y, float z, float randomMultiplier, bool specificPosition, bool sameTeam) {

	for (int i = 0; i < count; i++) {
		try {
			s_object_placement_data nObject;

			if (!DATUM_IS_NONE(object_idx)) {
				datum player_datum = s_player::getPlayerUnitDatumIndex(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)));
				Engine::Objects::create_new_placement_data(&nObject, object_idx, player_datum, 0);
				real_point3d* player_position = h2mod->get_player_unit_coords(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)));

				if (player_position != nullptr) {
					nObject.position.x = player_position->x * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
					nObject.position.y = player_position->y * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
					nObject.position.z = (player_position->z + 5.0f) * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
				}
				if (specificPosition) {
					nObject.position.x = x;
					nObject.position.y = y;
					nObject.position.z = z;
				}

				if (!sameTeam)
					nObject.team_index = NONE;

				LOG_TRACE_GAME("object_datum = {0:#x}, x={1:f}, y={2:f}, z={3:f}", object_idx, nObject.position.x, nObject.position.y, nObject.position.z);
				datum object_gamestate_datum = Engine::Objects::object_new(&nObject);
				Engine::Objects::simulation_action_object_create(object_gamestate_datum);
			}
		}
		catch (...) {
			LOG_TRACE_GAME("Error running spawn command");
		}
	}
}

void ConsoleCommands::spawn_rotate(datum object_idx, float x, float y, float z, float i, float j, float k)
{
	s_object_placement_data nObject;
	if (!DATUM_IS_NONE(object_idx))
	{
		Engine::Objects::create_new_placement_data(&nObject, object_idx, -1, 0);
		typedef void(__cdecl t_set_orientation)(real_vector3d* forward, real_vector3d* up, real_point3d* orient);
		auto set_orientation = Memory::GetAddress<t_set_orientation*>(0x3347B);
		nObject.position.x = x;
		nObject.position.y = y;
		nObject.position.z = z;
		real_vector3d rotation{ i = i, j = j, k = k };
		set_orientation(&nObject.orientation, &nObject.up, &rotation);
		auto gamestate_datum = Engine::Objects::object_new(&nObject);
		Engine::Objects::simulation_action_object_create(gamestate_datum);
		output(L"Spawned object: ");
		output(IntToWString<unsigned int>(gamestate_datum, std::hex));
	}
}
void ConsoleCommands::delete_object(datum object_datum)
{
	if (!DATUM_IS_NONE(object_datum))
	{
		Engine::Objects::object_destroy(object_datum);
	}
}
void ConsoleCommands::output(std::wstring result) {
	if (Memory::isDedicatedServer()) {
		result = result + L"\n";
		ServerConsole::logToDedicatedServerConsole(result.c_str());
	}
	else {
		std::string str(result.begin(), result.end());
		writePreviousOutput(str);
	}
}

void ConsoleCommands::display(std::string output)
{
	writePreviousOutput(output);
}

bool ConsoleCommands::isNum(const char* s) {
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
			mapManager->ReloadAllMaps();
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
			output(L"warpfix");
			output(L"maingamelooppatches");
			output(L"injecttag (tag_name) (tag_type) (map_name)");
			return;
		}
		else if (firstCommand == "$mapfilename")
		{
			std::wstring map_file_name;
			mapManager->GetMapFilename(map_file_name);
			output(map_file_name);
			return;
		}
		else if (firstCommand == "$downloadmap") {
			if (splitCommands.size() != 2) {
				output(L"Invalid download map command, usage - $downloadMap MAP_NAME");
				return;
			}
			if (!NetworkSession::LocalPeerIsSessionHost())
			{
				output(L"Cannot download map using command while not being the session host!");
				return;
			}
			std::string firstArg = splitCommands[1];
			auto downloadQuery = mapManager->AddDownloadQuery(std::wstring(firstArg.begin(), firstArg.end()));
			downloadQuery->StartMapDownload(); // since we have the map name, start the download
			return;
		}
		else if (firstCommand == "$kick") {
			if (Memory::isDedicatedServer()) {
				output(L"Don't use this on dedis");
				return;
			}
			if (splitCommands.size() != 2) {
				output(L"Invalid kick command, usage - $kick PEER_INDEX");
				return;
			}
			if (!NetworkSession::LocalPeerIsSessionHost()) {
				output(L"Only the server can kick players");
				return;
			}

			std::string firstArg = splitCommands[1];

			if (isNum(firstArg.c_str())) {
				int peerIndex = atoi(firstArg.c_str());
				if (peerIndex == NetworkSession::GetCurrentNetworkSession()->session_host_peer_index) {
					output(L"Don't kick yourself");
					return;
				}
				if (peerIndex >= NetworkSession::GetPeerCount()) {
					output(L"Peer at the specified index doesn't exist");
					return;
				}
				NetworkSession::KickPeer(peerIndex);
			}
			return;
		}
		else if (firstCommand == "$logplayers") {
			if (!NetworkSession::LocalPeerIsSessionHost()) {
				output(L"Only host can log player information.");
				return;
			}
			NetworkSession::LogPlayersToConsole();
			return;
		}
		else if (firstCommand == "$logpeers") {
			if (!NetworkSession::LocalPeerIsSessionHost()) {
				output(L"Only host can log peer information.");
				return;
			}
			NetworkSession::LogPeersToConsole();
			return;
		}
		else if (firstCommand == "$maxplayers") {
			if (splitCommands.size() != 2) {
				output(L"Usage: $maxplayers value (betwen 1 and 16).");
				return;
			}
			if (!NetworkSession::LocalPeerIsSessionHost()) {
				output(L"Can be only used while hosting.");
				return;
			}

			std::string secondArg = splitCommands[1];

			if (isNum(secondArg.c_str())) {
				int maxPlayersToSet = atoi(secondArg.c_str());
				if (maxPlayersToSet < 1 || maxPlayersToSet > 16) {
					output(L"The value needs to be between 1 and 16.");
					return;
				}
				if (maxPlayersToSet < NetworkSession::GetPlayerCount()) {
					output(L"You can't set a value of max players smaller than the actual number of players on the server.");
					return;
				}
				else {
					NetworkSession::GetCurrentNetworkSession()->parameters[0].max_party_players = maxPlayersToSet;
					output(L"Maximum players set.");
					return;
				}
			}
			return;
		}
		else if (firstCommand == "$resetspawncommandlist") {
			//reset checked_for_ids, so you can reload new object_datums at runtime
			this->checked_for_ids = false;
			return;
		}
		else if (firstCommand == "$spawnnear") {
			if (splitCommands.size() < 4 || splitCommands.size() > 5) {
				output(L"Invalid command usage $spawnnear command_name count same_team optional:multiplier");
				return;
			}

			if (h2mod->GetEngineType() == e_engine_type::_main_menu) {
				//TODO: need a nicer way to detect this for dedis
				output(L"Can only be used ingame");
				return;
			}

			if (!NetworkSession::LocalPeerIsSessionHost() && h2mod->GetEngineType() != e_engine_type::_single_player)
			{
				output(L"Can only be used by the session host!");
				return;
			}

			//lookup a commands.txt file that contain string->object_datums
			checkForIds();

			std::string secondArg = splitCommands[1];
			std::string thirdArg = splitCommands[2];
			datum object_datum = DATUM_INDEX_NONE;
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
			if (splitCommands.size() == 5) {
				//optional multiplier provided
				std::string fourthArg = splitCommands[4];
				randomMultiplier = stof(fourthArg);
			}

			int count = 1;
			try {
				count = stoi(thirdArg);
			}
			catch (...) {
				LOG_TRACE_GAME("Error converting string to int");
				return;
			}

			bool sameTeam = false;
			if (splitCommands[3] == "true")
				sameTeam = true;

			real_point3d* localPlayerPosition = h2mod->get_player_unit_coords(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)));
			this->spawn(object_datum, count, localPlayerPosition->x + 0.5f, localPlayerPosition->y + 0.5f, localPlayerPosition->z + 0.5f, randomMultiplier, false, sameTeam);
			return;
		}
		else if (firstCommand == "$ishost") {
			s_network_session* session = NetworkSession::GetCurrentNetworkSession();
			std::wstring isHostStr = L"isHost=";
			DWORD isHostByteValue = session->local_session_state;
			std::wostringstream ws;
			ws << isHostByteValue;
			const std::wstring s(ws.str());
			isHostStr += (NetworkSession::LocalPeerIsSessionHost() ? L"yes" : L"no");
			isHostStr += L", value=";
			isHostStr += s;
			output(isHostStr);
			return;
		}
		else if (firstCommand == "$leavegame") {
			NetworkSession::LeaveSession();
			return;
		}
		else if (firstCommand == "$xyz") {
			if (h2mod->GetEngineType() == e_engine_type::_mutliplayer && !NetworkSession::LocalPeerIsSessionHost()) {
				output(L"Only host can see xyz for now...");
				return;
			}
			extern bool displayXyz;
			displayXyz = !displayXyz;
			return;
		}
		
		else if (firstCommand == "$spawn") {
			if (splitCommands.size() != 7) {
				output(L"Invalid command, usage $spawn command_name count x y z same_team");
				return;
			}

			if (h2mod->GetEngineType() == e_engine_type::_main_menu) {
				output(L"Can only be used ingame");
				return;
			}

			if (!NetworkSession::LocalPeerIsSessionHost()) {
				output(L"Can only be used by the session host!");
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

			bool sameTeam = false;
			if (splitCommands[6] == "true")
				sameTeam = true;
			
			this->spawn(object_datum, count, x, y, z, 1.0f, true, sameTeam);
			return;
		}
		else if (firstCommand == "$spawn_rotate")
		{
			if (splitCommands.size() != 8)
			{
				output(L"Invalid command, usage $spawn_rotate object_id x y z i j k");
				return;
			}
			if (h2mod->GetEngineType() == e_engine_type::_main_menu) {
				output(L"Can only be used ingame");
				return;
			}

			if (!NetworkSession::LocalPeerIsSessionHost()) {
				output(L"Can only be used by the session host!");
				return;
			}

			//lookup a commands.txt file that contain string->object_datums
			checkForIds();

			std::string secondArg = splitCommands[1];
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

			float x = stof(splitCommands[2]);
			float y = stof(splitCommands[3]);
			float z = stof(splitCommands[4]);
			float i = stof(splitCommands[5]);
			float j = stof(splitCommands[6]);
			float k = stof(splitCommands[7]);
			bool sameTeam = false;
			if (splitCommands[6] == "true")
				sameTeam = true;

			this->spawn_rotate(object_datum, x, y, z, i, j, k);
			return;
		}
		else if (firstCommand == "$destroy_object")
		{
			if (splitCommands.size() != 2)
			{
				output(L"Invalid usage, usage $destroy_object datum");
				return;
			}
			unsigned int object_datum = strtoul(splitCommands[1].c_str(), NULL, 0);
			this->delete_object(object_datum);
		}
		else if (firstCommand == "$controller_sens") {
			if (splitCommands.size() != 2) {
				output(L"Invalid usage, usage $controller_sens value");
				return;
			}
			std::string sensVal = splitCommands[1];

			if (isNum(sensVal.c_str())) {
				ControllerInput::SetSensitiviy(stof(sensVal));
				H2Config_controller_sens = stof(sensVal);
			}
			else {
				output(L"Wrong input! Use a number.");
			}
			return;
		}
		else if (firstCommand == "$mouse_sens") {
			if (splitCommands.size() != 2) {
				output(L"Invalid usage, usage $mouse_sens value");
				return;
			}
			std::string sensVal = splitCommands[1];

			if (isNum(sensVal.c_str())) {
				MouseInput::SetSensitivity(stof(sensVal));
				H2Config_mouse_sens = stof(sensVal);
			}
			else {
				output(L"Wrong input! Use a number.");
			}
			return;
		}
		else if (firstCommand == "$lognetworksessionoffsets") {
			NetworkSession::LogStructureOffsets();
			return;
		}
		else if (firstCommand == "$requestfilename") {
			NetworkMessage::SendRequestMapFilename(NONE);
		}
		else if (firstCommand == "$warpfix") {
			if (splitCommands.size() != 2 && !splitCommands[1].empty()) {
				output(L"Invalid command, usage warpfix true/false");
				return;
			}

			std::string secondArg = splitCommands[1];

			if (secondArg.compare("true") == 0 || secondArg.compare("1") == 0)
				H2Tweaks::WarpFix(true);
			else
				H2Tweaks::WarpFix(false);
		}
		else if (firstCommand == "$d3dex") {
			if (splitCommands.size() != 2 && !splitCommands[1].empty()) {
				output(L"Invalid command, usage d3dex true/false");
				return;
			}
			std::string secondArg = splitCommands[1];
			if (secondArg.compare("true") == 0 || secondArg.compare("1") == 0) {
				H2Config_d3dex = true;
				output(L"D3D9Ex version of D3D9 has been enabled. Restart your game to take effect. If your game crashes on startup, disable it from the config file.");
			}
			else if (secondArg.compare("false") == 0 || secondArg.compare("0") == 0) {
				H2Config_d3dex = false;
				output(L"D3D9Ex version of D3D9 has been disabled. Restart your game to take effect.");
			}
			else {
				output(L"Invalid command, usage d3dex true/false");
			}
			return;
		}
		else if (firstCommand == "$injecttag")
		{
			if (!NetworkSession::LocalPeerIsSessionHost() && h2mod->GetEngineType() != e_engine_type::_single_player)
			{
				output(L"Can only be used by the session host!");
				return;
			}

			if (splitCommands.size() != 4) {
				output(L"Invalid paramters");
				return;
			}
			std::string tagName = splitCommands[1];

			blam_tag tagType = blam_tag::from_string(splitCommands[2]);

			std::string mapName = splitCommands[3];

			auto tagDatum = tag_loader::Get_tag_datum(tagName, tagType, mapName);
			tag_loader::Load_tag(tagDatum, true, mapName);
			tag_loader::Push_Back();
			std::wstring result = L"Loaded Tag Datum: ";
			result += IntToWString<int>(tag_loader::ResolveNewDatum(tagDatum), std::hex);
			output(result);

			LOG_INFO_GAME("{} {} {}", tagName, tagType.as_string(), mapName);
			return;
		}
		else if (firstCommand == "$logxnetconnections") {
			if (splitCommands.size() != 1) {
				output(L"No parameters are needed.");
			}
			gXnIp.LogConnectionsToConsole();
		}
		
		else if (firstCommand == "$net_metrics")
		{
			imgui_handler::g_network_stats_overlay = !imgui_handler::g_network_stats_overlay;
		}
		else {
			output(L"Unknown command.");
		}
	}
}