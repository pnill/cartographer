#ifndef TSSERVER_H
#define TSSERVER_H
#include "public_definitions.h"
#include "public_errors.h"
#include "serverlib_publicdefinitions.h"
#include "serverlib.h"

class TSServer {
public:
	TSServer(bool log);
	virtual ~TSServer();
	void startListening();
	void setPort(unsigned int port);
	void setConnectClient(bool connectClient);
	//only dedicated team speak servers don't need to connect as client
	//unless some server admin wants to listen to the team speak
	bool connectClient = true;
	void destroyVirtualServer();

private:
	//needed for licensing, one ts server per machine
	char keyPairFilename[BUFSIZ];
	uint64 serverID;
	unsigned int port;

	void createVirtualServer();
	void firstTimeValidateLicense(const char *keyPair);
	static int readKeyPairFromFile(const char *fileName, char *keyPair);
	static int writeKeyPairToFile(const char *fileName, const char* keyPair);
	static void onAccountingErrorEvent(uint64 serverID, unsigned int errorCode);
	static void onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);
	static void onClientMoved(uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID);
	static void onClientStopTalkingEvent(uint64 serverID, anyID clientID);
	static void onClientStartTalkingEvent(uint64 serverID, anyID clientID);
	static void onClientDisconnected(uint64 serverID, anyID clientID, uint64 channelID);
	static void onClientConnected(uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError);
};

#endif