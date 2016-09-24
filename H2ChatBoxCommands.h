#ifndef CHATBOX_COMMANDS_H
#define CHATBOX_COMMANDS_H

class ChatBoxCommands {
public:
	ChatBoxCommands();

	void reloadMap();
	void printDistance(int player1, int player2);
	void listPlayers();
	void listBannedPlayers();
	void mute(const char*, bool);
	void unmute(const char*);
	void ban(char*);
	void kick(char*);
	void kick(int);
	void setChatMode(CLIENT_CHAT_MODE);
	void handle_command(std::string);
	void setVoiceActivationLevel(float);
private:
	bool isNum(const char *s);

	float MIN_VOICE_ACTIVATION_LEVEL = -50.0f;
	float MAX_VOICE_ACTIVATION_LEVEL = 50.0f;

	char* SET_CHAT_MODE_VALIDATION_MSG = "Invalid setChatMode command, can only be used on server, usage - $setChatMode 0-3 (0=everyone,1=proximity,2=teamonly,3=off)";
	//server only below
	char* SET_CHANNEL_CODEC_VALIDATION_MSG = "Invalid setChannelCodec command, can only be used on server, usage - $setChannelCodec 0-5 (5=best, 0=worst)";
	char* SET_CHANNEL_QUALITY_VALIDATION_MSG = "Invalid setChannelQuality command, can only be used on server, usage - $setChannelQuality 0-10 (10=best, 0=worst)";
};

#endif