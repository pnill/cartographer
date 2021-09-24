#pragma once



namespace ServerConsole
{
	enum ServerConsoleCommands {
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

	// TODO: implement this properly
	class IKablamCommand
	{
		virtual void execute() = 0;
	};

	struct kablam_command_send_msg // : IKablamCommand
	{
		DWORD vtbl; // we set the vtable manually for now
		DWORD unk_1;
		DWORD unk_2;
		wchar_t message[121];
		int unk_3;

		kablam_command_send_msg(DWORD _vtbl, wchar_t* _message) : vtbl(_vtbl)
		{
			memset((void*)&unk_1, 0, sizeof(kablam_command_send_msg) - sizeof(vtbl));
			wcscpy_s(message, ARRAYSIZE(message), _message);
		}

		void sendGameMessage()
		{
			auto p_sendGameMessage = Memory::GetAddress<void(__thiscall*)(kablam_command_send_msg* thisx)>(0x0, 0x7175);
			return p_sendGameMessage(this);
		}
	};
	static_assert(sizeof(kablam_command_send_msg) == 260, "error sizeof(kablam_command_send_msg) != 260");
	static_assert(offsetof(kablam_command_send_msg, vtbl) == 0, "error offsetof(kablam_command_send_msg, v_table) != 0");
}