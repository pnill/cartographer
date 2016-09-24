#include "public_definitions.h"
#include "public_errors.h"
#include "clientlib_publicdefinitions.h"
#include "clientlib.h"

#include "Globals.h"

const float TSClient::MAX_CLIENT_VOLUME_MODIFIER = 0.0f;
const float TSClient::MIN_CLIENT_VOLUME_MODIFIER = -31.0f;

TSClient::TSClient(bool log) {
	TRACE_GAME_N("Client started");
	initializeCallbacks(log);
	TRACE_GAME_N("Client lib initialized");
}

char* TSClient::programPath(char* programInvocation) {
	char* path;
	char* end;
	int length;
	char pathsep;

	if (programInvocation == NULL) return strdup("");

#ifdef _WIN32
	pathsep = '\\';
#else
	pathsep = '/';
#endif

	end = strrchr(programInvocation, pathsep);
	if (!end) return strdup("");

	length = (end - programInvocation) + 2;
	path = (char*)malloc(length);
	strncpy(path, programInvocation, length - 1);
	path[length - 1] = 0;

	return path;
}

void TSClient::initializeCallbacks(bool log) {
	char * path;
	struct ClientUIFunctions funcs;

	memset(&funcs, 0, sizeof(struct ClientUIFunctions));

	funcs.onConnectStatusChangeEvent = onConnectStatusChangeEvent;
	funcs.onClientMoveEvent = onClientMoveEvent;
	funcs.onClientMoveTimeoutEvent = onClientMoveTimeoutEvent;
	funcs.onTalkStatusChangeEvent = onTalkStatusChangeEvent;
	funcs.onServerErrorEvent = onServerErrorEvent;
	funcs.onUserLoggingMessageEvent = onUserLoggingMessageEvent;

	//soundbackends better be in your /halo2 directory
	path = programPath("soundbackends");
	int logTypes;

	if (log) {
		logTypes = LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING;
	}
	else {
		logTypes = LogType_NONE;
	}
	error = ts3client_initClientLib(&funcs, NULL, logTypes, NULL, path);
	free(path);

	if (error != ERROR_ok) {
		char* errormsg;
		if (ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
			TRACE_GAME_N("Error initialzing client: %s\n", errormsg);
			ts3client_freeMemory(errormsg);
		}
	}

	/* Create a new client identity */
	if ((error = ts3client_createIdentity(&identity)) != ERROR_ok) {
		TRACE_GAME_N("Error creating identity: %d\n", error);
		return;
	}
}

void TSClient::onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
	//TODO: what to do during critical errors? nothing?
	TRACE_GAME_N("[%d]User log message: %s", logLevel, logMessage);
}

void TSClient::connect() {
	/* Spawn a new server connection handler using the default port and store the server ID */
	if ((error = ts3client_spawnNewServerConnectionHandler(0, &scHandlerID)) != ERROR_ok) {
		TRACE("Error spawning server connection handler: %d\n", error);
	}

	this->openMicrophone();
	this->openPlayback();
	this->setVoiceActivationLevel(-5.0f);

	char *version;
	const char* hostnameOrIP = inet_ntoa(serverAddress);
	TRACE_GAME_N("TeamSpeak::hostname: %s", hostnameOrIP);
	TRACE_GAME_N("TeamSpeak::port: %d", port);
	TRACE_GAME_N("TeamSpeak::nickname: %s", nickname);
	//TODO: do we need a secret per session?
	if ((error = ts3client_startConnection(scHandlerID, identity, hostnameOrIP, port, nickname, NULL, "", "secret")) != ERROR_ok) {
		TRACE_GAME_N("Error connecting to server: %d\n", error);
		return;
	}

	/* Query and print client lib version */
	if ((error = ts3client_getClientLibVersion(&version)) != ERROR_ok) {
		TRACE_GAME_N("Failed to get clientlib version: %d\n", error);
		return;
	}
	TRACE_GAME_N("Client lib version: %s", version);
	ts3client_freeMemory(version);  /* Release dynamically allocated memory */
	version = NULL;
}

void TSClient::openPlayback() {
	/* Open default playback device */
	/* Passing empty string for mode and NULL or empty string for device will open the default device */
	if ((error = ts3client_openPlaybackDevice(scHandlerID, "", NULL)) != ERROR_ok) {
		TRACE_GAME_N("Error opening playback device: %d\n", error);
	}
	if ((error = ts3client_setPlaybackConfigValue(scHandlerID, "volume_modifier", "20.0")) != ERROR_ok) {
		TRACE_GAME_N("Error setting playback config value: %d\n", error);
	}
}

void TSClient::openMicrophone() {
	/* Open default capture device */
	/* Passing empty string for mode and NULL or empty string for device will open the default device */
	if ((error = ts3client_openCaptureDevice(scHandlerID, "", NULL)) != ERROR_ok) {
		microphoneEnabled = false;
		TRACE_GAME_N("Error opening capture device: %d\n", error);
	}
}

void TSClient::disconnect() {
	/* Disconnect from server */
	if ((error = ts3client_stopConnection(scHandlerID, "leaving")) != ERROR_ok) {
		TRACE_GAME_N("Error stopping connection: %d\n", error);
	}
	else {
		TRACE_GAME_N("Client disconnecting from the server %s", inet_ntoa(serverAddress));
	}

	//TODO: guess we need to wait a lil fro the connection to stop, why?
	Sleep(200);

	/* Destroy server connection handler */
	if ((error = ts3client_destroyServerConnectionHandler(scHandlerID)) != ERROR_ok) {
		TRACE("Error destroying clientlib: %d\n", error);
	}
}

/*
* Callback for connection status change.
* Connection status switches through the states STATUS_DISCONNECTED, STATUS_CONNECTING, STATUS_CONNECTED and STATUS_CONNECTION_ESTABLISHED.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   newStatus                 - New connection status, see the enum ConnectStatus in clientlib_publicdefinitions.h
*   errorNumber               - Error code. Should be zero when connecting or actively disconnection.
*                               Contains error state when losing connection.
*/
void TSClient::onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
	TRACE("Connect status changed: %llu %d %u", (unsigned long long)serverConnectionHandlerID, newStatus, errorNumber);
	/* Failed to connect ? */
	if (newStatus == STATUS_DISCONNECTED && errorNumber == ERROR_failed_connection_initialisation) {
		TRACE_GAME_N("Looks like there is no server running.");
	}
}

/*
* Called when a client joins, leaves or moves to another channel.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   clientID                  - ID of the moved client
*   oldChannelID              - ID of the old channel left by the client
*   newChannelID              - ID of the new channel joined by the client
*   visibility                - Visibility of the moved client. See the enum Visibility in clientlib_publicdefinitions.h
*                               Values: ENTER_VISIBILITY, RETAIN_VISIBILITY, LEAVE_VISIBILITY
*/
void TSClient::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
	TRACE_GAME_N("ClientID %u moves from channel %llu to %llu with message %s", clientID, (unsigned long long)oldChannelID, (unsigned long long)newChannelID, moveMessage);
}

/*
* Called when a client drops his connection.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   clientID                  - ID of the moved client
*   oldChannelID              - ID of the channel the leaving client was previously member of
*   newChannelID              - 0, as client is leaving
*   visibility                - Always LEAVE_VISIBILITY
*   timeoutMessage            - Optional message giving the reason for the timeout
*/
void TSClient::onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage) {
	TRACE_GAME_N("ClientID %u timeouts with message %s", clientID, timeoutMessage);
}

/*
* Prints the current client volume
*/
void TSClient::printCurrentClientVolume(anyID teamspeakClientID) {
	int volume;
	if (ts3client_getClientVariableAsInt(scHandlerID, teamspeakClientID, CLIENT_VOLUME_MODIFICATOR, &volume)) {
		TRACE("error getting client volume");
	}
	else {
		TRACE("current client=%d volume=%d", teamspeakClientID, volume);
	}
}

int TSClient::getClientVolume(anyID teamspeakClientID) {
	int volume;
	if (ts3client_getClientVariableAsInt(scHandlerID, teamspeakClientID, CLIENT_VOLUME_MODIFICATOR, &volume)) {
		TRACE("error getting client volume");
	}
	return volume;
}

/*
* Sets the volume for the given client id
*/
void TSClient::setClientVolume(anyID teamspeakClientID, float volume) {
	int error;
	if ((error = ts3client_setClientVolumeModifier(scHandlerID, teamspeakClientID, volume)) != ERROR_ok) {
		TRACE("error modifying client volume: %d\n", error);
	}
}

/*
* Sets how close you need to be to your mic for teamspeak to recognize you are talking
*/
void TSClient::setVoiceActivationLevel(float activationLevel) {
	int error;

	std::ostringstream ss;
	ss << activationLevel;
	std::string s(ss.str());

	if (error = ts3client_setPreProcessorConfigValue(scHandlerID, "voiceactivation_level", s.c_str()) != ERROR_ok) {
		TRACE("Error setting voice activation level: %d\n", error);
	}
}

anyID TSClient::getClientId(char* name) {
	anyID clientId = NULL;
	anyID* currentClients = new anyID[16];
	if (ts3client_getClientList(scHandlerID, &currentClients) == ERROR_ok) {
		try {
			for (int i = 0; i < 15; i++) {
				anyID clientID = currentClients[i];
				char* clientName;
				/* Query client nickname from ID */
				int result = ts3client_getClientVariableAsString(scHandlerID, clientID, CLIENT_NICKNAME, &clientName);
				if (result != ERROR_ok) {
					TRACE("error trying to get client name for mute/unmute information, code=%d", result);
				}
				else {
					if (strcmp(name, clientName) == 0) {
						clientId = clientID;
					}

					//free the memory if we didn't get an error
					ts3client_freeMemory(clientName);
				}
			}
		}
		catch (...) {
			TRACE("Exception thrown getting client id, default exception");
		}
		ts3client_freeMemory(currentClients);
	}
	return NULL;
}

/*
* Mutes the ts client associated with the given name
*/
void TSClient::mute(const char* name, bool permanent) {
	int error;
	anyID clientsToMute[2];

	//TODO: strip double quotes
	H2Player player = players->getPlayer(std::string(name));
	WCHAR strw[8192];
	char strw3[4096];
	wsprintf(strw, L"%I64x", player.xuid);
	wcstombs(strw3, strw, 8192);

	anyID clientID = this->getClientId(strw3);
	clientsToMute[0] = clientID;
	clientsToMute[1] = 0;    // Terminating zero
	if ((error = ts3client_requestMuteClients(scHandlerID, clientsToMute, NULL)) != ERROR_ok) {
		TRACE("error turning muting clients: %d\n", error);
	}
	ts3client_freeMemory(clientsToMute);

	TRACE_GAME_N("Player %s mute status %d", name, isMuted(clientID));

	if (permanent) {
		//TODO: write out gamertag (maybe even ip and mac addr) to a voice ban file, so these players automatically get banned in any server
		//this client is in with voice enabled
	}
}

void TSClient::mute(anyID clientToMute) {
	int error;
	anyID clientsToMute[2];
	clientsToMute[0] = clientToMute;
	clientsToMute[1] = 0;    // Terminating zero
	if ((error = ts3client_requestMuteClients(scHandlerID, clientsToMute, NULL)) != ERROR_ok) {
		TRACE("error turning muting clients: %d\n", error);
	}
	ts3client_freeMemory(clientsToMute);
}

/*
* Unmutes the ts client associated with the given name
*/
void TSClient::unmute(const char* name) {
	int error;
	anyID clientsToUnmute[2];

	H2Player player = players->getPlayer(std::string(name));
	WCHAR strw[8192];
	char strw3[4096];
	wsprintf(strw, L"%I64x", player.xuid);
	wcstombs(strw3, strw, 8192);

	anyID clientID = this->getClientId(strw3);
	clientsToUnmute[0] = clientID;
	clientsToUnmute[1] = 0;    // Terminating zero
	if ((error = ts3client_requestUnmuteClients(scHandlerID, clientsToUnmute, NULL)) != ERROR_ok) {
		TRACE("error turning unmuting clients: %d\n", error);
	}

	ts3client_freeMemory(clientsToUnmute);
	//TODO: check if they were permanently banned and remove their entry from the file
}

void TSClient::unmute(anyID clientToUnmute) {
	int error;
	anyID clientsToUnmute[2];
	clientsToUnmute[0] = clientToUnmute;
	clientsToUnmute[1] = 0;    // Terminating zero
	if ((error = ts3client_requestUnmuteClients(scHandlerID, clientsToUnmute, NULL)) != ERROR_ok) {
		TRACE("error turning unmuting clients: %d\n", error);
	}
	ts3client_freeMemory(clientsToUnmute);
}

bool TSClient::isMuted(anyID clientId) {
	int clientIsMuted;
	if (ts3client_getClientVariableAsInt(scHandlerID, clientId, CLIENT_IS_MUTED, &clientIsMuted) != ERROR_ok) {
		TRACE("Error querying client muted state");
	}
	//returns 1 if muted, 0 if not
	return clientIsMuted;
}

//TODO: turn on when ready
void TSClient::handleProximityTalk(anyID teamspeakClientID, XUID remoteId, int remotePlayerIndex, int clientPlayerIndex, bool sameTeam) {
	if (!sameTeam) {
		float distance = h2mod->get_distance(remotePlayerIndex, clientPlayerIndex);

		float MAX = 10.0f;
		if (distance >= MAX) {
			//if the distance is greater than 50 units, mute
			client->mute(teamspeakClientID);
		}
		else {
			//if they are within 10 units, apply ghetto distance to volume calculation
			client->unmute(teamspeakClientID);
			//start with faintest client volume modifier
			float volume = MIN_CLIENT_VOLUME_MODIFIER;
			if (distance >= 9.0f && distance < MAX) {
				volume = -25.0f;
			}
			else if (distance >= 7.0f && distance < 8.0f) {
				volume = -20.0f;
			}
			else if (distance >= 5.0f && distance < 6.0f) {
				volume = -15.0f;
			}
			else if (distance >= 3.0f && distance < 4.0f) {
				volume = -10.0f;
			}
			else if (distance >= 2.0f && distance < 3.0f) {
				volume = -5.0f;
			}
			else if (distance < 1.0f) {
				volume = 0.0f;
			}

			client->setClientVolume(teamspeakClientID, volume);
			client->printCurrentClientVolume(teamspeakClientID);
		}
	}
	else {
		//same team hears each other all the time
		client->setClientVolume(teamspeakClientID, MAX_CLIENT_VOLUME_MODIFIER);
	}
}

void TSClient::handleTeamTalk(anyID teamspeakClientID, XUID remoteId, bool sameTeam) {
	if (!sameTeam) {
		//mute ppl on different team
		client->setClientVolume(teamspeakClientID, MIN_CLIENT_VOLUME_MODIFIER);
		client->printCurrentClientVolume(teamspeakClientID);
	}
	else {
		tsUsers->userStartedTalking(remoteId);
		//unmute ppl on same team that aren't explicitly muted
		client->setClientVolume(teamspeakClientID, MAX_CLIENT_VOLUME_MODIFIER);
		client->printCurrentClientVolume(teamspeakClientID);
	}
}

void TSClient::handleNoTeamsTalk(anyID teamspeakClientID, XUID remoteId) {
	tsUsers->userStartedTalking(remoteId);
	client->setClientVolume(teamspeakClientID, MAX_CLIENT_VOLUME_MODIFIER);
	client->printCurrentClientVolume(teamspeakClientID);
}

void TSClient::handleLobbyTalk(anyID teamspeakClientID, XUID remoteId) {
	tsUsers->userStartedTalking(remoteId);
	client->setClientVolume(teamspeakClientID, MAX_CLIENT_VOLUME_MODIFIER);
	client->printCurrentClientVolume(teamspeakClientID);
}

/*
* This event is called when a client starts or stops talking.
*
* Parameters:
*   serverConnectionHandlerID - Server connection handler ID
*   status                    - 1 if client starts talking, 0 if client stops talking
*   isReceivedWhisper         - 1 if this event was caused by whispering, 0 if caused by normal talking
*   clientID                  - ID of the client who announced the talk status change
*/
void TSClient::onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID teamspeakClientID) {
	char* remoteXuidStr;
	/* Query remote nickname from ID */
	if (ts3client_getClientVariableAsString(serverConnectionHandlerID, teamspeakClientID, CLIENT_NICKNAME, &remoteXuidStr) != ERROR_ok)
		return;

	XUID remoteId = _atoi64(remoteXuidStr);
	XUID clientId = _atoi64(client->nickname);

	if (status == STATUS_TALKING) {
		TRACE_GAME_N("Client \"%s\" starts talking.", remoteXuidStr);

		if (remoteId == clientId) {
			//local user talking
			tsUsers->userStartedTalking(remoteId);
			client->setClientVolume(teamspeakClientID, MAX_CLIENT_VOLUME_MODIFIER);
			client->printCurrentClientVolume(teamspeakClientID);
		}
		else if (isLobby) {
			//in the lobby everyone can hear anyone
			handleLobbyTalk(teamspeakClientID, remoteId);
		}
		else if (clientChatMode == OFF) {
			//TODO: this probably should go somewhere else, we should get notified when someone turns off client chat
			//it would be better to just disconnect the client adhoc
		}
		else {
			//in game only chat modes below
			H2Player remotePlayer = players->getPlayer(remoteId);
			int remotePlayerIndex = remotePlayer.index;
			int remotePlayerTeamIndex = remotePlayer.team;

			H2Player clientPlayer = players->getPlayer(clientId);
			int clientPlayerIndex = clientPlayer.index;
			int clientPlayerTeamIndex = clientPlayer.team;

			TRACE_GAME_N("Client-%d remotePlayerTeamIndex:%u, xuid:%s", remotePlayerIndex, remotePlayerTeamIndex, remoteXuidStr);
			TRACE_GAME_N("Client-%d clientPlayerTeamIndex:%u, xuid:%s", clientPlayerIndex, clientPlayerTeamIndex, client->nickname);
			bool sameTeam = remotePlayerTeamIndex == clientPlayerTeamIndex;
			bool teamPlayOn = h2mod->is_team_play();
			TRACE("IsSameTeam=%d,IsTeamPlay=%d", sameTeam, teamPlayOn);

			//TODO: once proximity is done, it would default to proximity chat
			if (!teamPlayOn) {
				//if team play is off, just treat lobby chat
				handleNoTeamsTalk(teamspeakClientID, remoteId);
			}
			else if (clientChatMode == TEAM) {
				//if team play on and team chat mode is set
				handleTeamTalk(teamspeakClientID, remoteId, sameTeam);
			}
		}
	}
	else {
		TRACE_GAME_N("Client \"%s\" stops talking.", remoteXuidStr);
		tsUsers->userStoppedTalking(remoteId);
	}
	ts3client_freeMemory(remoteXuidStr);
}

void TSClient::onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
	TRACE_GAME_N("Error for server %llu: %s %s", (unsigned long long)serverConnectionHandlerID, errorMessage, extraMessage);
}

void TSClient::setServerAddress(IN_ADDR address) {
	this->serverAddress = address;
}

void TSClient::setServerPort(unsigned int port) {
	this->port = port;
}

void TSClient::setNickname(char* nickname) {
	this->nickname = nickname;
}

void TSClient::startChatting() {
	connect();
}

TSClient::~TSClient() {
	ts3client_freeMemory(identity);  /* Release dynamically allocated memory */
	identity = NULL;

	delete nickname;
}