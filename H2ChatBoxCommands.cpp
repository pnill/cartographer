#include "Globals.h"
#include <Shlwapi.h>

ChatBoxCommands::ChatBoxCommands() {
}

void ChatBoxCommands::mute(const char* name, bool ban) {
	//TODO: if server try to mute on the server (if we even can)
	//if we can't we can just move them to another channel or kick them from the channel
	//but kicking would require us to every minute or so check if we reconnect
	if (client != NULL) {
		client->mute(name, ban);
	}
}

void ChatBoxCommands::setVoiceActivationLevel(float activationLevel) {
	if (client != NULL) {
		client->setVoiceActivationLevel(activationLevel);
	}
}

void ChatBoxCommands::unmute(const char* name) {
	//TODO: unmute for server (if possible)
	if (client != NULL) {
		client->unmute(name);
		//TODO: remove ban if exists
	}
}

void ChatBoxCommands::setChatMode(CLIENT_CHAT_MODE mode) {
	clientChatMode = mode;
}

void ChatBoxCommands::kick(char* playerName) {
	H2Player player = players->getPlayer(playerName);
	int playerIndex = player.index;

	if (playerIndex == -1) {
		h2mod->write_inner_chat_dynamic(L"Could not find player index");
		return;
	}

	TRACE_GAME_N("About to kick player index %d for player name %s", playerIndex, playerName);
	h2mod->kick_player(playerIndex);
}

void ChatBoxCommands::kick(int playerIndex) {
	h2mod->kick_player(playerIndex);
}

bool ChatBoxCommands::isNum(const char *s) {
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

void ChatBoxCommands::listBannedPlayers() {
	std::vector<std::string> stringList;

	BanUtility::getInstance().fillBannedPlayerDisplayStrList(stringList);

	std::vector<std::string>::const_iterator iterator;
	for (iterator = stringList.begin(); iterator != stringList.end(); ++iterator) {
		std::string displayItem = *iterator;
		std::wstring ws;
		ws.assign(displayItem.begin(), displayItem.end());
		h2mod->write_inner_chat_dynamic(ws.c_str());
	}
}

void ChatBoxCommands::printDistance(int player1, int player2) {
	float distance = h2mod->get_distance(player1, player2);
	std::wstringstream oss;
	oss << "Distance from player index " << player1 << " to player index " << player2 << " is " << distance << " units";
	h2mod->write_inner_chat_dynamic(oss.str().c_str());
}

void ChatBoxCommands::listPlayers() {
	//we have to iterate all 16 player spots, since when people leave a game, other people in game don't occupy their spot
	TRACE("Team play on: %d", h2mod->is_team_play());
	for (int i = 0; i < 15; i++) {
		H2Player player = players->getPlayer(i);
		if (player.index == -1) {
			wchar_t* unicodeGamertag = new wchar_t[64];
			mbstowcs(unicodeGamertag, player.name.c_str(), 64);

			const char* hostnameOrIP = inet_ntoa(h2mod->get_player_ip(i));
			/*
			TODO:
			bool resetDynamicBase = false;
			if (i != nameToPlayerIndexMap[gamertag]) {
			resetDynamicBase = true;
			}

			float xPos = h2mod->get_player_x(i, resetDynamicBase);
			float yPos = h2mod->get_player_y(i, resetDynamicBase);
			float zPos = h2mod->get_player_z(i, resetDynamicBase);*/

			std::wstringstream oss;
			oss << "Player " << i << ":" << unicodeGamertag << "/" << hostnameOrIP;// << "/x=" << xPos << ",y=" << yPos << ",z=" << zPos;
			std::wstring playerLogLine = oss.str();
			h2mod->write_inner_chat_dynamic(playerLogLine.c_str());
		}
	}
}

void ChatBoxCommands::ban(char* gamertag) {
	//first get all the unique properties about the player
	H2Player player = players->getPlayer(gamertag);
	int playerIndex = player.index;
	XUID playerXuid = player.index;
	IN_ADDR playerIp = h2mod->get_player_ip(playerIndex);

	//second kick the player from the game
	kick(gamertag);

	//finally ban the player based on all the unique properties of the player
	BanUtility::getInstance().banPlayer(gamertag, playerIp, playerXuid);
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
		if (firstCommand == "$kick") {
			if (splitCommands.size() != 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid kick command, usage - $kick (GAMERTAG or PLAYER_INDEX)");
				return;
			}
			std::string firstArg = splitCommands[1];
			char *cstr = new char[firstArg.length() + 1];
			strcpy(cstr, firstArg.c_str());

			try {
				if (!isServer) {
					h2mod->write_inner_chat_dynamic(L"Only the server can kick players");
				}
				else {
					if (isNum(cstr)) {
						kick(atoi(cstr));
					}
					else {
						kick(cstr);
					}
				}
			}
			catch (...) {
				TRACE("Error trying to kick");
			}

			delete[] cstr;
		}
		else if (firstCommand == "$ban") {
			if (splitCommands.size() != 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid ban command, usage - $ban GAMERTAG");
				return;
			}

			//TODO: enable when ban is ready
			h2mod->write_inner_chat_dynamic(L"not ready yet");
			return;

			std::string firstArg = splitCommands[1];
			char *cstr = new char[firstArg.length() + 1];
			strcpy(cstr, firstArg.c_str());

			if (!isServer) {
				h2mod->write_inner_chat_dynamic(L"Only the server can ban players");
			}
			else {
				ban(cstr);
			}

			delete[] cstr;
		}
		else if (firstCommand == "$mute") {
			if (splitCommands.size() < 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid mute command, usage - $mute GAMERTAG");
				return;
			}
			std::string firstArg = splitCommands[1];
			char *cstr = new char[firstArg.length() + 1];
			strcpy(cstr, firstArg.c_str());

			/*
			std::string secondArg = splitCommands[2];
			bool ban = secondArg == "true" ? true : false;*/

			try {
				mute(cstr, false);
			}
			catch (...) {
				TRACE("Error trying to mute");
			}

			delete[] cstr;
		}
		else if (firstCommand == "$unmute") {
			if (splitCommands.size() < 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid mute command, usage - $unmute GAMERTAG");
				return;
			}
			std::string firstArg = splitCommands[1];
			char *cstr = new char[firstArg.length() + 1];
			strcpy(cstr, firstArg.c_str());

			try {
				unmute(cstr);
			}
			catch (...) {
				TRACE("Error trying to unmute");
			}
			delete[] cstr;
		}
		else if (firstCommand == "$setchatmode") {
			//TODO: use command socket to set chat modes for client
		}
		else if (firstCommand == "$listplayers") {
			if (splitCommands.size() != 1) {
				h2mod->write_inner_chat_dynamic(L"Invalid listPlayers command, usage - $listPlayers");
				return;
			}

			if (!isServer) {
				h2mod->write_inner_chat_dynamic(L"listPlayers can only be used on the server");
				return;
			}
			listPlayers();
		}
		else if (firstCommand == "$listbannedplayers") {
			if (splitCommands.size() != 1) {
				h2mod->write_inner_chat_dynamic(L"Invalid listBannedPlayers command, usage - $listBannedPlayers");
				return;
			}

			//TODO: enable when ban is ready
			h2mod->write_inner_chat_dynamic(L"not ready yet");
			return;

			if (!isServer) {
				h2mod->write_inner_chat_dynamic(L"listBannedPlayers can only be used on the server");
				return;
			}
			listBannedPlayers();
		}
		else if (firstCommand == "$printdistance") {
			if (splitCommands.size() != 3) {
				h2mod->write_inner_chat_dynamic(L"Invalid $printDistance command, usage - $printDistance playerIndex1 playerIndex2");
				return;
			}

			//TODO: enable when ready
			h2mod->write_inner_chat_dynamic(L"not ready yet");
			return;

			if (!isServer) {
				h2mod->write_inner_chat_dynamic(L"printDistance can only be used on the server");
				return;
			}

			std::string firstArg = splitCommands[1];
			if (!isNum(firstArg.c_str())) {
				h2mod->write_inner_chat_dynamic(L"playerIndex1 is not a number");
				return;
			}
			int player1 = atoi(firstArg.c_str());
			std::string secondArg = splitCommands[2];
			if (!isNum(secondArg.c_str())) {
				h2mod->write_inner_chat_dynamic(L"playerIndex2 is not a number");
				return;
			}
			int player2 = atoi(secondArg.c_str());
			printDistance(player1, player2);
		}
		else if (firstCommand == "$setvoiceactivationlevel") {
			if (splitCommands.size() != 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid $setVoiceActivationLevel command, usage - $setVoiceActivationLevel activationLevel(-50 to 50)");
				return;
			}

			std::string firstArg = splitCommands[1];
			if (!isNum(firstArg.c_str())) {
				h2mod->write_inner_chat_dynamic(L"activation level is not a number");
				return;
			}
			float activationLevel = ::atof(firstArg.c_str());
			if (activationLevel < MIN_VOICE_ACTIVATION_LEVEL || activationLevel > MAX_VOICE_ACTIVATION_LEVEL) {
				h2mod->write_inner_chat_dynamic(L"activation level is not between -50 and 50, default is -25.0f");
			}

			setVoiceActivationLevel(activationLevel);
		}
		else if (firstCommand == "$unban") {
			//TODO: unbanning would involve being able to unban by name, or address, or xuid
			//TODO: check for unban all, and unban everyone

			//TODO: enable when ban is ready
			h2mod->write_inner_chat_dynamic(L"not ready yet");
			return;
		}
		else if (firstCommand == "$getplayerindex") {
			if (splitCommands.size() != 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid $getplayerindex command, usage - $getplayerindex player_name");
				return;
			}
			std::string firstArg = splitCommands[1];
			char *cstr = new char[firstArg.length() + 1];
			strcpy(cstr, firstArg.c_str());

			H2Player player = players->getPlayer(cstr);

			std::wstringstream oss;
			oss << "Player " << firstArg.c_str() << " index is = " << player.index;
			h2mod->write_inner_chat_dynamic(oss.str().c_str());

			delete[] cstr;
		}
		else if (firstCommand == "$start") {
			if (splitCommands.size() != 2) {
				h2mod->write_inner_chat_dynamic(L"Invalid $start command - server only, usage - $start script_name");
				return;
			}
			std::string firstArg = splitCommands[1];
			char *scriptName = new char[firstArg.length() + 1];
			strcpy(scriptName, firstArg.c_str());

			//TODO: 

			delete[] scriptName;
		}
		else if (firstCommand == "$muteall") {
			if (splitCommands.size() != 1) {
				h2mod->write_inner_chat_dynamic(L"Invalid command, usage - $muteAll");
				return;
			}

			for (int i = 0; i < 15; i++) {
				H2Player player = players->getPlayer(i);

				if (player.index != -1) {
					mute(player.name.c_str(), true);
				}
			}
		}
		else if (firstCommand == "$unmuteall") {
			if (splitCommands.size() != 1) {
				h2mod->write_inner_chat_dynamic(L"Invalid command, usage - $unmuteAll");
				return;
			}

			for (int i = 0; i < 15; i++) {
				H2Player player = players->getPlayer(i);

				if (player.index != -1) {
					unmute(player.name.c_str());
				}
			}
		}
		else if (firstCommand == "$reloadmaps") {
			if (splitCommands.size() != 1) {
				h2mod->write_inner_chat_dynamic(L"Invalid command, usage - $reloadMaps");
				return;
			}

			typedef char(__thiscall *possible_map_reload)(int thisx);
			possible_map_reload possible_map_reload_method = (possible_map_reload)(h2mod->GetBase() + 0x4D021);
			DWORD* unk = (DWORD*)(h2mod->GetBase() + 0x482D70);

			typedef struct RTL_CRITICAL_SECTION**(__thiscall *init_critical_section)(int thisx);
			init_critical_section init_critical_section_method = (init_critical_section)(h2mod->GetBase() + 0xC18BD);

			//*(WORD *)((int)unk + 148016) = 0;
			init_critical_section_method((int)unk);
			possible_map_reload_method((int)unk);
		}
	}
}