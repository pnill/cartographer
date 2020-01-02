#pragma once

namespace ServerConsole
{
	void ApplyHooks();
	void logToDedicatedServerConsole(const wchar_t* string, ...);
	void SendCommand(wchar_t** command, int split_commands_size, char unk);
}