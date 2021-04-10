
#include "ServerConsole.h"

#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Console/ConsoleCommands.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "H2MOD/Modules/Utils/Utils.h"

typedef void*(__cdecl *dedi_command_hook)(wchar_t** a1, int a2, char a3);
dedi_command_hook p_dedi_command_hook;

typedef int(__cdecl p_kablam_vip_add)(LPCWSTR gamertag);
p_kablam_vip_add* kablam_vip_add;

typedef signed int(p_kablam_vip_clear)();
p_kablam_vip_clear* kablam_vip_clear;

void* __cdecl dediCommandHook(wchar_t** command_line_args, int split_strings, char a3) {

	wchar_t* command = command_line_args[0];
	if (command[0] == L'$') {
		ServerConsole::logToDedicatedServerConsole(L"Running custom command\n");
		//run the chatbox commands
		std::wstring wsCommand(command);
		commands->handle_command(std::string(wsCommand.begin(), wsCommand.end()));
		return 0;
	}
	
	//Transform the command into lower invariant so that we can convert the string to it's enum value
	auto LowerCommand = std::wstring(command);
	std::transform(LowerCommand.begin(), LowerCommand.end(), LowerCommand.begin(),[](unsigned char c) { return std::tolower(c); });

	//Execute any events that may be linked to that command.
	EventHandler::executeServerCommandCallback(ServerConsole::s_commandsMap[LowerCommand]);

	//Commented out because it's unused but the functionality of it remains, incase it is ever needed in the future for modifying the behaviour of the commands.
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
	
	return p_dedi_command_hook(command_line_args, split_strings, a3);
}

void ServerConsole::ApplyHooks()
{
	if (!Memory::isDedicatedServer())
		return;
	s_commandsMap[L"ban"] = ServerConsoleCommands::ban;
	s_commandsMap[L"description"] = ServerConsoleCommands::description;
	s_commandsMap[L"exit"] = ServerConsoleCommands::exit;
	s_commandsMap[L"kick"] = ServerConsoleCommands::kick;
	s_commandsMap[L"live"] = ServerConsoleCommands::live;
	s_commandsMap[L"name"] = ServerConsoleCommands::name;
	s_commandsMap[L"play"] = ServerConsoleCommands::play;
	s_commandsMap[L"players"] = ServerConsoleCommands::players;
	s_commandsMap[L"playing"] = ServerConsoleCommands::playing;
	s_commandsMap[L"privacy"] = ServerConsoleCommands::privacy;
	s_commandsMap[L"sendmsg"] = ServerConsoleCommands::sendmsg;
	s_commandsMap[L"skip"] = ServerConsoleCommands::skip;
	s_commandsMap[L"statsfolder"] = ServerConsoleCommands::statsfolder;
	s_commandsMap[L"status"] = ServerConsoleCommands::status;
	s_commandsMap[L"unban"] = ServerConsoleCommands::unban;
	s_commandsMap[L"vip"] = ServerConsoleCommands::vip;
	s_commandsMap[L"any"] = ServerConsoleCommands::any;
	p_dedi_command_hook = (dedi_command_hook)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x1CCFC), (BYTE*)dediCommandHook, 7);
	kablam_vip_add = Memory::GetAddress<p_kablam_vip_add*>(0, 0x1D932);
	kablam_vip_clear = Memory::GetAddress<p_kablam_vip_clear*>(0, 0x1DB16);
}

void ServerConsole::logToDedicatedServerConsole(const wchar_t* string, ...) {

	if (!Memory::isDedicatedServer())
		return;

	typedef signed int(dedi_print)(const wchar_t* str, ...);
	auto dedi_print_method = Memory::GetAddress<dedi_print*>(0, 0x2354C8);

	dedi_print_method(string);
}

void ServerConsole::SendCommand(wchar_t** command, int split_commands_size, char unk)
{
	BYTE* unk1 = reinterpret_cast<BYTE*>(p_dedi_command_hook(command, split_commands_size, unk));
	BYTE* threadparams = Memory::GetAddress<BYTE*>(0, 0x450680);

	typedef void(__cdecl* unk_func1)(void* a1);
	auto func_unk1 = Memory::GetAddress<unk_func1>(0, 0x1D6EA);

	typedef void(__cdecl* free_memory_game_impl)(LPVOID lpMem);
	auto p_free_memory_game_impl = Memory::GetAddress<free_memory_game_impl>(0x0, 0x2344B8);

	typedef void(__thiscall* async_set_atomic_long_value)(void *thisx, LONG Value);
	auto p_async_set_atomic_long_value = Memory::GetAddress<async_set_atomic_long_value>(0, 0x6E00);

	typedef void(__cdecl* unk_func3)(wchar_t *a1);
	auto func_unk3 = Memory::GetAddress<unk_func3>(0, 0x19C93);

	if (unk1)
	{
		if (*(BYTE*)(unk1 + 8))
		{
			p_async_set_atomic_long_value(*(BYTE**)(threadparams + 8), (LONG)unk1);
			func_unk1(unk1);
		}
		else
		{
			func_unk1(unk1);
			func_unk3(*command);
		}

		// free allocated command resources
		p_free_memory_game_impl(unk1);
	}

	BYTE v8 = (*(BYTE**)(threadparams + 8))[20];
	*(BYTE*)(threadparams + 4) = v8;
	if (!v8)
		logToDedicatedServerConsole(L"\r\n");
}


void ServerConsole::SendCommand2(int argCount, wchar_t* command, wchar_t* argument, ...)
{
	typedef int(__cdecl* ProcessCommand)(wchar_t** commandArray, int argumentCount, char a3);
	auto p_process_command = Memory::GetAddress<ProcessCommand>(0, 0x1CCFC);

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

void ServerConsole::AddVip(std::wstring Gamertag)
{
	kablam_vip_add(Gamertag.c_str());
}

void ServerConsole::ClearVip()
{
	kablam_vip_clear();
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
