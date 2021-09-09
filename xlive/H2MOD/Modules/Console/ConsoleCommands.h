#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"

class ConsoleCommands {
public:
	ConsoleCommands();
	void handle_command(std::string);
	void spawn(datum object_datum, int count, float x, float y, float z, float randomMultiplier, bool specificPosition, bool sameTeam);
	void output(std::wstring result);
	void display(std::string output);
	std::vector<std::string> prevCommands;
	std::vector<std::string> prevOutput;
	BOOL handleInput(WPARAM wp);
	bool consoleOpen() { return this->console; };
	bool shouldCaretBlink();
	void resetCaretBlink();

	std::string command;
	int caretPos;

	bool handled;
	bool console = false;
	int previous_command_index = 0;
private:
	void writePreviousCommand(std::string& msg);
	void writePreviousOutput(std::string& msg);
	bool isNum(const char *s);
	void checkForIds();
	bool checked_for_ids = false;
	std::unordered_map<std::string, unsigned int> object_ids;
	DWORD sleepTime;
	const int caretBlinkTimeMs = 500;
	unsigned lastTimeCaretBlink = timeGetTime();
	bool caretBlinked = false;
};

extern ConsoleCommands* commands;
