#ifndef CHATBOX_COMMANDS_H
#define CHATBOX_COMMANDS_H

class ChatBoxCommands {
public:
	ChatBoxCommands();
	void handle_command(std::string);
	void spawn(unsigned int object_datum, int count, float x, float y, float z, float randomMultiplier);
private:
	void checkForIds();
	bool checked_for_ids = false;
	std::unordered_map<std::string, unsigned int> object_ids;
	DWORD sleepTime;
};

#endif