#include "Globals.h"

TSServer::TSServer(bool log) {
	char *version;
	unsigned int error;

	/* Create struct for callback function pointers */
	struct ServerLibFunctions funcs;

	/* Initialize all callbacks with NULL */
	memset(&funcs, 0, sizeof(struct ServerLibFunctions));

	/* Now assign the used callback function pointers */
	funcs.onClientConnected = onClientConnected;
	funcs.onClientDisconnected = onClientDisconnected;
	funcs.onClientMoved = onClientMoved;
	funcs.onUserLoggingMessageEvent = onUserLoggingMessageEvent;
	funcs.onClientStartTalkingEvent = onClientStartTalkingEvent;
	funcs.onClientStopTalkingEvent = onClientStopTalkingEvent;
	funcs.onAccountingErrorEvent = onAccountingErrorEvent;

	//TODO: turn off user logging
	if (error = ts3server_setLogVerbosity(LogLevel_DEBUG)) {
		char* errormsg;
		if (ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			TRACE_GAME_N("Error setting log verbosity: %s (%d)", errormsg, error);
			ts3server_freeMemory(errormsg);
		}

	}
	/* Initialize server lib with callbacks */
	int logTypes;

	if (log) {
		logTypes = LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING;
	}
	else {
		logTypes = LogType_NONE;
	}
	if ((error = ts3server_initServerLib(&funcs, logTypes, NULL)) != ERROR_ok) {
		char* errormsg;
		if (ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			TRACE_GAME_N("Error initialzing serverlib: %s", errormsg);
			ts3server_freeMemory(errormsg);
		}
	}

	/* Query and print server lib version */

	if ((error = ts3server_getServerLibVersion(&version)) != ERROR_ok) {
		TRACE_GAME_N("Error querying server lib version: %d", error);
	}
	TRACE_GAME_N("Server lib version: %s", version);
	ts3server_freeMemory(version);
}

TSServer::~TSServer() {

}

void TSServer::createVirtualServer() {
	char* keyPair;
	char buffer[BUFSIZ] = { 0 };
	char port_str[20];

	/* Attempt to load keypair from file */
	/* Assemble filename: keypair_<port>.txt */
	strcpy(keyPairFilename, "keypair_");
	//port has to match the port you use to start the virtual server, otherwise you will get errors starting up
	//invalid parameter 1538
	sprintf(port_str, "%d", port);
	strcat(keyPairFilename, port_str);
	strcat(keyPairFilename, ".txt");

	/* Try reading keyPair from file */
	if (readKeyPairFromFile(keyPairFilename, buffer) == 0) {
		keyPair = buffer;  /* Id read from file */
	}
	else {
		keyPair = "";  /* No Id saved, start virtual server with empty keyPair string */
	}

	unsigned int error;
	//TRACE_GAME_N("Create virtual server using keypair '%s'", &keyPair);
	//16 client max, since that is the max clients currently in halo2
	if ((error = ts3server_createVirtualServer(port, "0.0.0.0", "tss", keyPair, 16, &serverID)) != ERROR_ok) {
		char* errormsg;
		if (ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			TRACE_GAME_N("Error creating virtual server: %s (%d)", errormsg, error);
			ts3server_freeMemory(errormsg);
			return;
		}
	}

	firstTimeValidateLicense(keyPair);

	/* Set welcome message */
	if ((error = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_WELCOMEMESSAGE, "Hello TeamSpeak 3")) != ERROR_ok) {
		TRACE_GAME_N("Error setting server welcomemessage: %d", error);
	}

	/* Set server password */
	if ((error = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_PASSWORD, "secret")) != ERROR_ok) {
		TRACE_GAME_N("Error setting server password: %d", error);
	}
	/* Set the chanell codec */
	int channelid = 1;
	//channel codecs
	//0 - speex narrowband
	//1 - speex wideband
	//2 - speex ultra wideband
	//3 - celt
	//4 - opus voice
	//5 - opus music
	//TODO: make configurable
	if ((error = ts3server_setChannelVariableAsInt(serverID, channelid, CHANNEL_CODEC, 5)) != ERROR_ok) {
		TRACE_GAME_N("Error setting codec: %d", error);
	}

	/* Lower codec quality */
	//0-10 (default is 7) 10 is best quality
	//TODO: make configurable
	if ((error = ts3server_setChannelVariableAsInt(serverID, channelid, CHANNEL_CODEC_QUALITY, 10)) != ERROR_ok) {
		TRACE_GAME_N("Error changing codec quality: %d", error);
	}

	/* Flush above two changes to server */
	if ((error = ts3server_flushVirtualServerVariable(serverID)) != ERROR_ok) {
		TRACE("Error flushing server variables");
	}
}

void TSServer::destroyVirtualServer() {
	unsigned int error;

	/* Stop virtual server */
	if ((error = ts3server_stopVirtualServer(serverID)) != ERROR_ok) {
		TRACE_GAME_N("Error stopping virtual server: %d", error);
	}
	else {
		TRACE_GAME_N("Destroyed virtual server");
	}
}

/*
* Will grab the keypair from the virtual server if this is the first time starting the server
*/
void TSServer::firstTimeValidateLicense(const char *keyPair) {
	unsigned int error;
	/* If we didn't load the keyPair before, query it from virtual server and save to file */
	if (keyPair == "") {
		char* newKeyPair = "";
		if ((error = ts3server_getVirtualServerKeyPair(serverID, &newKeyPair)) != ERROR_ok) {
			char* errormsg;
			if (ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
				TRACE_GAME_N("Error querying keyPair: %s\n\n", errormsg);
				ts3server_freeMemory(errormsg);
			}
		}

		/* Save keyPair to file "keypair_<port>.txt"*/
		if (writeKeyPairToFile(keyPairFilename, newKeyPair) != 0) {
			ts3server_freeMemory(newKeyPair);
		}
		ts3server_freeMemory(newKeyPair);
	}
}

/*
* Write server key to file
*/
int TSServer::writeKeyPairToFile(const char *fileName, const char* keyPair) {
	FILE *file;

	file = fopen(fileName, "w");
	if (file == NULL) {
		TRACE_GAME_N("Could not open file '%s' for writing keypair\n", fileName);
		return -1;
	}

	fputs(keyPair, file);
	if (ferror(file) != 0) {
		fclose(file);
		TRACE_GAME_N("Error writing keypair to file '%s'.\n", fileName);
		return -1;
	}
	fclose(file);

	TRACE_GAME_N("Wrote keypair '%s' to file '%s'.", keyPair, fileName);
	return 0;
}


/*
* Read server key from file
*/
int TSServer::readKeyPairFromFile(const char *fileName, char *keyPair) {
	FILE *file;

	file = fopen(fileName, "r");
	if (file == NULL) {
		TRACE_GAME_N("Could not open file '%s' for reading keypair\n", fileName);
		return -1;
	}

	fgets(keyPair, BUFSIZ, file);
	if (ferror(file) != 0) {
		fclose(file);
		TRACE_GAME_N("Error reading keypair from file '%s'.\n", fileName);
		return -1;
	}
	fclose(file);

	TRACE_GAME_N("Read keypair '%s' from file '%s'.", keyPair, fileName);
	return 0;
}

void TSServer::onAccountingErrorEvent(uint64 serverID, unsigned int errorCode) {
	char* errorMessage;
	if (ts3server_getGlobalErrorMessage(errorCode, &errorMessage) == ERROR_ok) {
		TRACE_GAME_N("onAccountingErrorEvent serverID=%llu, errorCode=%u: %s", (unsigned long long)serverID, errorCode, errorMessage);
		ts3server_freeMemory(errorMessage);
	}
}

void TSServer::onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
	//TODO: what to do during critical errors? nothing?
	TRACE_GAME_N("[%d]Server log message: %s", logLevel, logMessage);
}
void TSServer::onClientStopTalkingEvent(uint64 serverID, anyID clientID) {
	TRACE_GAME_N("onClientStopTalkingEvent serverID=%llu, clientID=%u", (unsigned long long)serverID, clientID);
}
void TSServer::onClientStartTalkingEvent(uint64 serverID, anyID clientID) {
	TRACE_GAME_N("onClientStartTalkingEvent serverID=%llu, clientID=%d", (unsigned long long)serverID, clientID);
}
void TSServer::onClientMoved(uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID) {
	TRACE_GAME_N("Client %u moved from channel %llu to channel %llu on virtual server %llu",
		clientID, (unsigned long long)oldChannelID, (unsigned long long)newChannelID, (unsigned long long)serverID);
}
void TSServer::onClientDisconnected(uint64 serverID, anyID clientID, uint64 channelID) {
	TRACE_GAME_N("Client %u left channel %llu on virtual server %llu", clientID, (unsigned long long)channelID, (unsigned long long)serverID);
}
void TSServer::onClientConnected(uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError) {
	//TODO: if the client has no business in this channel (for team chat), *removeClientError = ERROR_client_insufficient_permissions;
	//this will make team only chat actually safe
	/*if (chatMode == TEAM_ONLY) {
	//TODO: get clients xuid from client id (somehow)
	char* name;
	if (ts3server_getClientVariableAsString(serverID, clientID, CLIENT_NICKNAME, &name) != ERROR_ok)
	return;

	//TODO: ts server can move clients from channel to channel
	}*/
	TRACE_GAME_N("Client %u joined channel %llu on virtual server %llu", clientID, (unsigned long long)channelID, (unsigned long long)serverID);
}

void TSServer::setPort(unsigned int port) {
	this->port = port;
}

void TSServer::setConnectClient(bool connectClient) {
	this->connectClient = connectClient;
}

void TSServer::startListening() {
	createVirtualServer();

	if (this->connectClient) {
		startClient(false);
		client->connect();
	}
}