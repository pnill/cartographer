#ifndef CHATBOX_COMMANDS_H
#define CHATBOX_COMMANDS_H

class ChatBoxCommands {
public:
	ChatBoxCommands();
	void handle_command(std::string);
private:
	bool checked_for_ids = false;
	std::unordered_map<std::string, unsigned int> object_ids;
};

#endif