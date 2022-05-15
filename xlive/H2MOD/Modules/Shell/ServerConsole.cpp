#include "stdafx.h"

#include "ServerConsole.h"

#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Utils\Utils.h"
#include "Util\Hooks\Hook.h"

typedef void* (__cdecl* dedi_command_t)(wchar_t** a1, int a2, char a3);
dedi_command_t p_dedi_command;

typedef int(__cdecl* kablam_vip_add_t)(LPCWSTR gamer_tag);
kablam_vip_add_t p_kablam_vip_add;

typedef signed int(__cdecl* kablam_vip_clear_t)();
kablam_vip_clear_t p_kablam_vip_clear;

ServerConsole::DediConsoleOutput dediOutput;

void* __cdecl DediCommandHook(wchar_t** command_line_split_wide, int split_count, char a3) {

	wchar_t* command = command_line_split_wide[0];
	if (command[0] == L'$') {
		ServerConsole::LogToDedicatedServerConsole(L"# running custom command: ");
		
		std::string command_line;
		command_line.reserve(MAX_CONSOLE_INPUT_BUFFER);
		for (int i = 0; i < split_count; i++)
		{
			std::wstring tokenWide;

			if (i > 0)
			{
				tokenWide = command_line_split_wide[i];
			}
			else
			{
				// skip $ character
				tokenWide = &command_line_split_wide[i][1];
			}

			command_line += std::string(tokenWide.begin(), tokenWide.end());
			command_line += " ";
		}

		ConsoleCommand::HandleCommandLine(command_line.c_str(), command_line.length(), &dediOutput);
		return 0;
	}

	// Transform the command into lower invariant so that we can convert the string to it's enum value
	auto lowerCommand = std::wstring(command);
	std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), [](unsigned char c) { return std::tolower(c); });

	// Execute any events that may be linked to that command.
	// EventHandler::executeServerCommandCallback(ServerConsole::s_commandsMap[LowerCommand]);

	// Commented out because it's unused but the functionality of it remains, incase it is ever needed in the future for modifying the behaviour of the commands.
	/*switch(ServerConsole::s_commandsMap[LowerCommand])
	{
		case ServerConsole::ServerConsoleCommands::skip: {
			break;
		}
		case ServerConsole::ban: break;
		case ServerConsole::description: break;
		case ServerConsole::exit: break;
		case ServerConsole::kick: break;
		case ServerConsole::live: break;
		case ServerConsole::name: break;
		case ServerConsole::play: break;
		case ServerConsole::players: break;
		case ServerConsole::playing: break;
		case ServerConsole::privacy: break;
		case ServerConsole::sendmsg: break;
		case ServerConsole::statsfolder: break;
		case ServerConsole::status: break;
		case ServerConsole::unban: break;
		case ServerConsole::vip: break;
		default: ;
	}*/

	// Temporary if statement to prevent double calling events,
	// all server command functions will be hooked in the future and these event executes will be removed.

	bool playCommand = false;
	auto playCommandFind = ServerConsole::s_commandsMap.find(lowerCommand);
	if (playCommandFind != ServerConsole::s_commandsMap.end()
		&& playCommandFind->second == ServerConsole::play)
		playCommand = true;

	if (!playCommand)
		EventHandler::ServerCommandEventExecute(EventExecutionType::execute_before, ServerConsole::s_commandsMap[lowerCommand]);

	void* result = p_dedi_command(command_line_split_wide, split_count, a3);

	// Temporary if statement to prevent double calling events,
	// all server command functions will be hooked in the future and these executes will be removed.
	
	if (!playCommand)
		EventHandler::ServerCommandEventExecute(EventExecutionType::execute_after, ServerConsole::s_commandsMap[lowerCommand]);

	return result;
}

typedef bool(__cdecl kablam_command_play_t)(wchar_t* playlist_file_path, int a2);
kablam_command_play_t* p_kablam_command_play;

bool __cdecl kablam_command_play(wchar_t* playlist_file_path, int a2)
{
	LOG_INFO_GAME("[{}]: {}", __FUNCTION__, "");
	EventHandler::ServerCommandEventExecute(EventExecutionType::execute_before, ServerConsole::play);
	auto res = p_kablam_command_play(playlist_file_path, a2);
	EventHandler::ServerCommandEventExecute(EventExecutionType::execute_after, ServerConsole::play);
	return res;
}

void ServerConsole::ApplyHooks()
{
	if (!Memory::IsDedicatedServer())
		return;
	s_commandsMap[L"ban"] = e_server_console_commands::ban;
	s_commandsMap[L"description"] = e_server_console_commands::description;
	s_commandsMap[L"exit"] = e_server_console_commands::exit;
	s_commandsMap[L"kick"] = e_server_console_commands::kick;
	s_commandsMap[L"live"] = e_server_console_commands::live;
	s_commandsMap[L"name"] = e_server_console_commands::name;
	s_commandsMap[L"play"] = e_server_console_commands::play;
	s_commandsMap[L"players"] = e_server_console_commands::players;
	s_commandsMap[L"playing"] = e_server_console_commands::playing;
	s_commandsMap[L"privacy"] = e_server_console_commands::privacy;
	s_commandsMap[L"sendmsg"] = e_server_console_commands::sendmsg;
	s_commandsMap[L"skip"] = e_server_console_commands::skip;
	s_commandsMap[L"statsfolder"] = e_server_console_commands::statsfolder;
	s_commandsMap[L"status"] = e_server_console_commands::status;
	s_commandsMap[L"unban"] = e_server_console_commands::unban;
	s_commandsMap[L"vip"] = e_server_console_commands::vip;
	s_commandsMap[L"any"] = e_server_console_commands::any;
	p_dedi_command = (dedi_command_t)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x1CCFC), (BYTE*)DediCommandHook, 7);
	p_kablam_vip_add = Memory::GetAddress<kablam_vip_add_t>(0, 0x1D932);
	p_kablam_vip_clear = Memory::GetAddress<kablam_vip_clear_t>(0, 0x1DB16);

	p_kablam_command_play = Memory::GetAddress<kablam_command_play_t*>(0, 0xE7FA);
	PatchCall(Memory::GetAddress(0, 0x724B), kablam_command_play);
}

void ServerConsole::LogToDedicatedServerConsole(const wchar_t* fmt, ...) {

	if (!Memory::IsDedicatedServer())
		return;

	typedef int(__cdecl* dedi_print_t)(const wchar_t* fmt, ...);
	auto p_dedi_print = Memory::GetAddress<dedi_print_t>(0, 0x2354C8);

	p_dedi_print(fmt);
}

void ServerConsole::SendCommand(wchar_t** command, int split_commands_size, char unk)
{
	BYTE* unk1 = reinterpret_cast<BYTE*>(p_dedi_command(command, split_commands_size, unk));
	BYTE* threadparams = Memory::GetAddress<BYTE*>(0, 0x450680);

	typedef void(__cdecl* unk_func1_t)(void* a1);
	auto p_fn_unk1 = Memory::GetAddress<unk_func1_t>(0, 0x1D6EA);

	typedef void(__cdecl* free_memory_game_t)(LPVOID lpMem);
	auto p_free_memory_game = Memory::GetAddress<free_memory_game_t>(0x0, 0x2344B8);

	typedef void(__thiscall* async_set_atomic_long_value_t)(void* thisx, LONG Value);
	auto p_async_set_atomic_long_value = Memory::GetAddress<async_set_atomic_long_value_t>(0, 0x6E00);

	typedef void(__cdecl* unk_func3_t)(wchar_t* a1);
	auto p_fn_unk3 = Memory::GetAddress<unk_func3_t>(0, 0x19C93);

	if (unk1)
	{
		if (*(BYTE*)(unk1 + 8))
		{
			p_async_set_atomic_long_value(*(BYTE**)(threadparams + 8), (LONG)unk1);
			p_fn_unk1(unk1);
		}
		else
		{
			p_fn_unk1(unk1);
			p_fn_unk3(*command);
		}

		// free allocated command resources
		p_free_memory_game(unk1);
	}

	BYTE v8 = (*(BYTE**)(threadparams + 8))[20];
	*(BYTE*)(threadparams + 4) = v8;
	if (!v8)
		LogToDedicatedServerConsole(L"\r\n");
}


void ServerConsole::SendCommand2(int argCount, wchar_t* command, wchar_t* argument, ...)
{
	typedef int(__cdecl* ProcessCommand_t)(wchar_t** commandArray, int argumentCount, char a3);
	auto p_process_command = Memory::GetAddress<ProcessCommand_t>(0, 0x1CCFC);

	std::vector<wchar_t*> commandVector;
	int size = 0;
	commandVector.push_back(command);
	va_list arguments;
	va_start(arguments, argument);
	while (argument)
	{
		commandVector.push_back(argument);
		size++;
		if (size == argCount) break;
		argument = va_arg(arguments, wchar_t*);
	}

	size++;
	SendCommand(commandVector.data(), size, 1);
}

void ServerConsole::AddVip(std::wstring gamerTag)
{
	p_kablam_vip_add(gamerTag.c_str());
}

void ServerConsole::ClearVip()
{
	p_kablam_vip_clear();
}

int messageTimeout = 0;

void ServerConsole::SendMsg(wchar_t* message, bool timeout)
{
	bool execute = !timeout;
	if (TimeElapsedMS(messageTimeout) > 10000)
	{
		messageTimeout = GetCurrentTimeMS();
		execute = true;
	}

	if (execute) {

		// first we construct kablam_command_send_msg, by manually passing the vtable pointer, and the message to be copied
		kablam_command_send_msg sendMsgCommand(Memory::GetAddress(0, 0x352dfc), message);

		// send the message
		sendMsgCommand.sendGameMessage();
	}
}

int ServerConsole::DediConsoleOutput::Output(StringHeaderFlags flags, const char* fmt)
{
	std::string fmtStr(fmt);
	std::wstring fmtStrWide(fmtStr.begin(), fmtStr.end());
	ServerConsole::LogToDedicatedServerConsole(fmtStrWide.c_str());
	ServerConsole::LogToDedicatedServerConsole(L"\n");
	return 0;
}

int ServerConsole::DediConsoleOutput::OutputFmt(StringHeaderFlags flags, const char* fmt, ...)
{
	va_list valist;
	va_start(valist, fmt);
	int buffer_size_needed = _vsnprintf(NULL, 0, fmt, valist) + 1;
	char* buffer = (char*)_malloca(buffer_size_needed);
	int copied_characters = _vsnprintf(buffer, buffer_size_needed, fmt, valist);
	DediConsoleOutput::Output(flags, buffer);
	_freea(buffer);
	va_end(valist);
	return 0;
}
