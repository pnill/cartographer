#pragma once



namespace ServerConsole
{
	static enum ServerConsoleCommands {
		ban,
		description,
		exit,
		kick,
		live,
		name,
		play,
		players,
		playing,
		privacy,
		sendmsg,
		skip,
		statsfolder,
		status,
		unban,
		vip,
		any
	};
	static std::map<std::wstring, ServerConsoleCommands> s_commandsMap;
	void ApplyHooks();
	void logToDedicatedServerConsole(const wchar_t* string, ...);
	void SendCommand(wchar_t** command, int split_commands_size, char unk);
	void AddVip(std::wstring gamertag);
	void ClearVip();
	/**
	 * \brief Proxies sending a command to the server via H2Admin or H2Server
	 * \param argCount The amount of arguments being sent to command (Will most likely be removed later)
	 * \param command The base command being sent
	 * \param argument An arguments to go with the command being sent
	 * \param ... An additional argument
	 */
	void SendCommand2(int argCount, wchar_t* command, wchar_t* argument, ...);
	void SendMsg(wchar_t* message, bool timeout = false);
}