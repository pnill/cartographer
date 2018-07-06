/*
 * This is the main header for the TeamSpeak 3 Server SDK. All the functions that are used
 * to communicate with the TeamSpeak 3 Server are here. Please view the documentation for
 * details. Note that some of these functions REQUIRE you to call ts3server_freeMemory()
 * after finishing with the result. This holds true for all the functions returning
 * strings (so the parameter is "char** result"). Again, the details are all in the
 * documentation.
 */

#ifndef SERVERLIB_H
#define SERVERLIB_H

#include <teamspeak/public_definitions.h>
#include <teamspeak/server_commands.h>

#ifdef __cplusplus
extern "C" {
#endif

struct TS3ChannelCreationParams;        //opaque definition
struct TS3VirtualServerCreationParams;  //opaque definition
struct TS3Variables;                    //opaque definition

enum VirtualServerCreateFlags{ 
	VIRTUALSERVER_CREATE_FLAG_NONE                = 0x0000,
	VIRTUALSERVER_CREATE_FLAG_PASSWORDS_ENCRYPTED = 0x0001,
};

enum ChannelCreateFlags{ 
	CHANNEL_CREATE_FLAG_NONE                = 0x000,
	CHANNEL_CREATE_FLAG_PASSWORDS_ENCRYPTED = 0x001,
};

struct ServerLibFunctions {
	void (*onVoiceDataEvent)           (uint64 serverID, anyID clientID, unsigned char* voiceData, unsigned int voiceDataSize, unsigned int frequency);
	void (*onClientStartTalkingEvent)  (uint64 serverID, anyID clientID);
	void (*onClientStopTalkingEvent)   (uint64 serverID, anyID clientID);
	void (*onClientConnected)          (uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError);
	void (*onClientDisconnected)       (uint64 serverID, anyID clientID, uint64 channelID);
	void (*onClientMoved)              (uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID);
	void (*onChannelCreated)           (uint64 serverID, anyID invokerClientID, uint64 channelID);
	void (*onChannelEdited)            (uint64 serverID, anyID invokerClientID, uint64 channelID);
	void (*onChannelDeleted)           (uint64 serverID, anyID invokerClientID, uint64 channelID);
	void (*onServerTextMessageEvent)   (uint64 serverID, anyID invokerClientID, const char* textMessage);
	void (*onChannelTextMessageEvent)  (uint64 serverID, anyID invokerClientID, uint64 targetChannelID, const char* textMessage);
	void (*onUserLoggingMessageEvent)  (const char* logmessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);
	void (*onAccountingErrorEvent)     (uint64 serverID, unsigned int errorCode);
	void (*onCustomPacketEncryptEvent) (char** dataToSend, unsigned int* sizeOfData);
	void (*onCustomPacketDecryptEvent) (char** dataReceived, unsigned int* dataReceivedSize);
	void (*onFileTransferEvent)        (const struct FileTransferCallbackExport* data);

	unsigned int (*permClientCanConnect)               (uint64 serverID, const struct ClientMiniExport* client);
	unsigned int (*permClientCanGetChannelDescription) (uint64 serverID, const struct ClientMiniExport* client);
	unsigned int (*permClientUpdate)                   (uint64 serverID, anyID clientID, struct VariablesExport* variables);
	unsigned int (*permClientKickFromChannel)          (uint64 serverID, const struct ClientMiniExport* client, int toKickCount, struct ClientMiniExport* toKickClients, const char* reasonText);
	unsigned int (*permClientKickFromServer)           (uint64 serverID, const struct ClientMiniExport* client, int toKickCount, struct ClientMiniExport* toKickClients, const char* reasonText);
	unsigned int (*permClientMove)                     (uint64 serverID, const struct ClientMiniExport* client, int toMoveCount, struct ClientMiniExport* toMoveClients, uint64 newChannel, const char* reasonText);
	unsigned int (*permChannelMove)                    (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID, uint64 newParentChannelID);
	unsigned int (*permSendTextMessage)                (uint64 serverID, const struct ClientMiniExport* client, anyID targetMode, uint64 targetClientOrChannel, const char* textMessage);
	unsigned int (*permServerRequestConnectionInfo)    (uint64 serverID, const struct ClientMiniExport* client);
	unsigned int (*permSendConnectionInfo)             (uint64 serverID, const struct ClientMiniExport* client, int* mayViewIpPort, const struct ClientMiniExport* targetClient);
	unsigned int (*permChannelCreate)                  (uint64 serverID, const struct ClientMiniExport* client, uint64 parentChannelID, struct VariablesExport* variables);
	unsigned int (*permChannelEdit)                    (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID, struct VariablesExport* variables);
	unsigned int (*permChannelDelete)                  (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID);
	unsigned int (*permChannelSubscribe)               (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID);
	unsigned int (*permFileTransferInitUpload)         (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftinitupload* params);
	unsigned int (*permFileTransferInitDownload)       (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftinitdownload* params);
	unsigned int (*permFileTransferGetFileInfo)        (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftgetfileinfo* params);
	unsigned int (*permFileTransferGetFileList)        (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftgetfilelist* params);
	unsigned int (*permFileTransferDeleteFile)         (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftdeletefile* params);
	unsigned int (*permFileTransferCreateDirectory)    (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftcreatedir* params);
	unsigned int (*permFileTransferRenameFile)         (uint64 serverID, const struct ClientMiniExport* client, const struct ts3sc_ftrenamefile* params);

	void (*onClientPasswordEncrypt)                    (uint64 serverID, const char* plaintext, char* encryptedText, int encryptedTextByteSize);
	unsigned int (*onTransformFilePath)                (uint64 serverID, anyID invokerClientID, const struct TransformFilePathExport* original, struct TransformFilePathExportReturns* result);
	unsigned int (*onCustomServerPasswordCheck)        (uint64 serverID, const struct ClientMiniExport* client, const char* password);
	unsigned int (*onCustomChannelPasswordCheck)       (uint64 serverID, const struct ClientMiniExport* client, uint64 channelID, const char* password);

}; //END OF ServerLibFunctions

/*Memory management*/
EXPORTDLL unsigned int ts3server_freeMemory(void* pointer);

/*Construction and Destruction*/
EXPORTDLL unsigned int ts3server_initServerLib(const struct ServerLibFunctions* functionPointers, int usedLogTypes, const char* logFileFolder);
EXPORTDLL unsigned int ts3server_enableFileManager(const char* filebase, const char** ips, int port, uint64 downloadBandwitdh, uint64 uploadBandwidth);
EXPORTDLL unsigned int ts3server_destroyServerLib();
EXPORTDLL unsigned int ts3server_disableClientCommand(int clientCommand);
EXPORTDLL unsigned int ts3server_getServerLibVersion(char** result);
EXPORTDLL unsigned int ts3server_getServerLibVersionNumber(uint64* result);

/*logging*/
EXPORTDLL unsigned int ts3server_setLogVerbosity(enum LogLevel logVerbosity);

/*error handling*/
EXPORTDLL unsigned int ts3server_getGlobalErrorMessage(unsigned int globalErrorCode, char** result);

/*client info*/
EXPORTDLL unsigned int ts3server_getClientVariableAsInt(uint64 serverID, anyID clientID, enum ClientProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getClientVariableAsUInt64(uint64 serverID, anyID clientID, enum ClientProperties flag, uint64* result);
EXPORTDLL unsigned int ts3server_getClientVariableAsString(uint64 serverID, anyID clientID, enum ClientProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setClientVariableAsInt(uint64 serverID, anyID clientID, enum ClientProperties flag, int value);
EXPORTDLL unsigned int ts3server_setClientVariableAsUInt64(uint64 serverID, anyID clientID, enum ClientProperties flag, uint64 value);
EXPORTDLL unsigned int ts3server_setClientVariableAsString(uint64 serverID, anyID clientID, enum ClientProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushClientVariable(uint64 serverID, anyID clientID);

EXPORTDLL unsigned int ts3server_setClientWhisperList(uint64 serverID, anyID clID, const uint64* channelID, const anyID* clientID);

EXPORTDLL unsigned int ts3server_getClientList(uint64 serverID, anyID** result);
EXPORTDLL unsigned int ts3server_getChannelOfClient(uint64 serverID, anyID clientID, uint64* result);
EXPORTDLL unsigned int ts3server_clientMove(uint64 serverID, uint64 newChannelID, const anyID* clientIDArray);

EXPORTDLL unsigned int ts3server_clientsKickFromServer(uint64 serverID, const anyID* clientIDArray, const char* kickReason, int failOnClientError);
EXPORTDLL unsigned int ts3server_getClientIDSfromUIDS(uint64 serverID, const char** clientUIDs, anyID** result);

/*channel info*/
EXPORTDLL unsigned int ts3server_getChannelVariableAsInt(uint64 serverID, uint64 channelID, enum ChannelProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getChannelVariableAsUInt64(uint64 serverID, uint64 channelID, enum ChannelProperties flag, uint64* result);
EXPORTDLL unsigned int ts3server_getChannelVariableAsString(uint64 serverID, uint64 channelID, enum ChannelProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setChannelVariableAsInt(uint64 serverID, uint64 channelID, enum ChannelProperties flag, int value);
EXPORTDLL unsigned int ts3server_setChannelVariableAsUInt64(uint64 serverID, uint64 channelID, enum ChannelProperties flag, uint64 value);
EXPORTDLL unsigned int ts3server_setChannelVariableAsString(uint64 serverID, uint64 channelID, enum ChannelProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushChannelVariable(uint64 serverID, uint64 channelID);
EXPORTDLL unsigned int ts3server_flushChannelCreation(uint64 serverID, uint64 channelParentID, uint64* result);

EXPORTDLL unsigned int ts3server_makeChannelCreationParams(struct TS3ChannelCreationParams** result);
EXPORTDLL unsigned int ts3server_setChannelCreationParams(struct TS3ChannelCreationParams* channelCreationParams, uint64 channelParentID, uint64 channelID); /*0 id's means undefined*/
EXPORTDLL unsigned int ts3server_getChannelCreationParamsVariables(struct TS3ChannelCreationParams* channelCreationParams, struct TS3Variables** result);
EXPORTDLL unsigned int ts3server_createChannel(uint64 serverID, struct TS3ChannelCreationParams* channelCreationParams, enum ChannelCreateFlags flags, uint64* result);

EXPORTDLL unsigned int ts3server_getChannelList(uint64 serverID, uint64** result);
EXPORTDLL unsigned int ts3server_getChannelClientList(uint64 serverID, uint64 channelID, anyID** result);
EXPORTDLL unsigned int ts3server_getParentChannelOfChannel(uint64 serverID, uint64 channelID, uint64* result);

EXPORTDLL unsigned int ts3server_channelDelete(uint64 serverID, uint64 channelID, int force);
EXPORTDLL unsigned int ts3server_channelMove(uint64 serverID, uint64 channelID, uint64 newChannelParentID, uint64 newOrder);

/*server info*/
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsInt(uint64 serverID, enum VirtualServerProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsUInt64(uint64 serverID, enum VirtualServerProperties flag, uint64* result);
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsString(uint64 serverID, enum VirtualServerProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsInt(uint64 serverID, enum VirtualServerProperties flag, int value);
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsUInt64(uint64 serverID, enum VirtualServerProperties flag, uint64 value);
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsString(uint64 serverID, enum VirtualServerProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushVirtualServerVariable(uint64 serverID);

EXPORTDLL unsigned int ts3server_makeVirtualServerCreationParams(struct TS3VirtualServerCreationParams** result);
EXPORTDLL unsigned int ts3server_setVirtualServerCreationParams(struct TS3VirtualServerCreationParams* virtualServerCreationParams, unsigned int serverPort, const char* serverIp, const char* serverKeyPair, unsigned int serverMaxClients, unsigned int channelCount, uint64 serverID);
EXPORTDLL unsigned int ts3server_getVirtualServerCreationParamsVariables(struct TS3VirtualServerCreationParams* virtualServerCreationParams, struct TS3Variables** result);
EXPORTDLL unsigned int ts3server_getVirtualServerCreationParamsChannelCreationParams(struct TS3VirtualServerCreationParams* virtualServerCreationParams, unsigned int channelIdx, struct TS3ChannelCreationParams** result);
EXPORTDLL unsigned int ts3server_createVirtualServer2(struct TS3VirtualServerCreationParams* virtualServerCreationParams, enum VirtualServerCreateFlags flags, uint64* result);

EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsUInt64(uint64 serverID, enum ConnectionProperties flag, uint64* result);
EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsDouble(uint64 serverID, enum ConnectionProperties flag, double* result);

EXPORTDLL unsigned int ts3server_getVirtualServerList(uint64** result);
EXPORTDLL unsigned int ts3server_stopVirtualServer(uint64 serverID);
EXPORTDLL unsigned int ts3server_createVirtualServer(unsigned int serverPort, const char* serverIp, const char* serverName, const char* serverKeyPair, unsigned int serverMaxClients, uint64* result);
EXPORTDLL unsigned int ts3server_getVirtualServerKeyPair(uint64 serverID, char** result);

/*security salt/hash*/
EXPORTDLL unsigned int ts3server_createSecuritySalt(int options, void* salt, int saltByteSize, char** securitySalt);
EXPORTDLL unsigned int ts3server_calculateSecurityHash(const char* securitySalt, const char* clientUniqueIdentifier, const char* clientNickName, const char* clientMetaData, char** securityHash);

/*variable editing*/
EXPORTDLL unsigned int ts3server_getVariableAsInt(struct TS3Variables* var, int flag, int* result);
EXPORTDLL unsigned int ts3server_getVariableAsUInt64(struct TS3Variables* var, int flag, uint64* result);
EXPORTDLL unsigned int ts3server_getVariableAsString(struct TS3Variables* var, int flag, char** result);
EXPORTDLL unsigned int ts3server_setVariableAsInt(struct TS3Variables* var, int flag, int value);
EXPORTDLL unsigned int ts3server_setVariableAsUInt64(struct TS3Variables* var, int flag, uint64 value);
EXPORTDLL unsigned int ts3server_setVariableAsString(struct TS3Variables* var, int flag, const char* value);

#ifdef __cplusplus
}
#endif

#endif
