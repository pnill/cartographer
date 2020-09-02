#pragma once
#include "H2MOD/Modules/Console/ConsoleCommands.h"

class StatsHandler
{
public:
	StatsHandler();
	static void sendStats();
	static int verifyPlaylist();
	static int uploadPlaylist();
	static void Test()
	{
		sendStats();
	}
	static void Test2(std::string message)
	{
		commands->display(message);
	}
};
