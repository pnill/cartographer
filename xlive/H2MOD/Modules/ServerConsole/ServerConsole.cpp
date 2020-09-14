
#include "ServerConsole.h"

#include "H2MOD.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Console/ConsoleCommands.h"

typedef void*(__cdecl *dedi_command_hook)(wchar_t** a1, int a2, char a3);
dedi_command_hook p_dedi_command_hook;

void* __cdecl dediCommandHook(wchar_t** command_line_args, int split_strings, char a3) {

	wchar_t* command = command_line_args[0];
	if (command[0] == L'$') {
		ServerConsole::logToDedicatedServerConsole(L"Running custom command\n");
		//run the chatbox commands
		std::wstring wsCommand(command);
		commands->handle_command(std::string(wsCommand.begin(), wsCommand.end()));
		return 0;
	}
	
	return p_dedi_command_hook(command_line_args, split_strings, a3);
}

void ServerConsole::ApplyHooks()
{
	if (!h2mod->Server)
		return;

	p_dedi_command_hook = (dedi_command_hook)DetourFunc(h2mod->GetAddress<BYTE*>(0, 0x1CCFC), (BYTE*)dediCommandHook, 7);
}

void ServerConsole::logToDedicatedServerConsole(const wchar_t* string, ...) {

	if (!h2mod->Server)
		return;

	typedef signed int(dedi_print)(const wchar_t* str, ...);
	auto dedi_print_method = h2mod->GetAddress<dedi_print*>(0, 0x2354C8);

	dedi_print_method(string);
}

void ServerConsole::SendCommand(wchar_t** command, int split_commands_size, char unk)
{
	BYTE* unk1 = reinterpret_cast<BYTE*>(p_dedi_command_hook(command, split_commands_size, unk));
	BYTE* threadparams = h2mod->GetAddress<BYTE*>(0, 0x450680);

	typedef void(__cdecl* unk_func1)(void* a1);
	auto func_unk1 = h2mod->GetAddress<unk_func1>(0, 0x1D6EA);

	typedef void(__cdecl* free_memory_game_impl)(LPVOID lpMem);
	auto p_free_memory_game_impl = h2mod->GetAddress<free_memory_game_impl>(0x0, 0x2344B8);

	typedef void(__thiscall* async_set_atomic_long_value)(void *thisx, LONG Value);
	auto p_async_set_atomic_long_value = h2mod->GetAddress<async_set_atomic_long_value>(0, 0x6E00);

	typedef void(__cdecl* unk_func3)(wchar_t *a1);
	auto func_unk3 = h2mod->GetAddress<unk_func3>(0, 0x19C93);

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
	auto p_process_command = h2mod->GetAddress<ProcessCommand>(0, 0x1CCFC);

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
