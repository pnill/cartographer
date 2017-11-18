#ifndef CHATBOX_COMMANDS_H
#define CHATBOX_COMMANDS_H

class ConsoleCommands {
public:
	ConsoleCommands();
	void handle_command(std::string);
	void spawn(unsigned int object_datum, int count, float x, float y, float z, float randomMultiplier);
	void output(std::wstring result);
	std::vector<std::string> prevCommands;
	BOOL handleInput(WPARAM wp);

	std::string command;
	int caretPos;
	bool handled;
	bool console = false;
private:
	void writePreviousCommand(std::string msg);
	bool isNum(const char *s);
	void checkForIds();
	bool checked_for_ids = false;
	std::unordered_map<std::string, unsigned int> object_ids;
	DWORD sleepTime;
};

#endif