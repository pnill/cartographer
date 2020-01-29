
#include "ServerConsole.h"

#include "H2MOD.h"
#include "Util/Hooks/Hook.h"

typedef void*(__cdecl *dedi_command_hook)(wchar_t** a1, int a2, char a3);
dedi_command_hook p_dedi_command_hook;

void* __cdecl dediCommandHook(wchar_t** command_line_args, int split_strings, char a3) {

	wchar_t* command = command_line_args[0];
	if (command[0] == L'$') {
		ServerConsole::logToDedicatedServerConsole(L"Running custom command\n");
		//run the chatbox commands
		std::wstring wsCommand(command);
		h2mod->handle_command(wsCommand);
		return 0;
	}

	return p_dedi_command_hook(command_line_args, split_strings, a3);
}

void ServerConsole::ApplyHooks()
{
	if (!h2mod->Server)
		return;

	p_dedi_command_hook = (dedi_command_hook)DetourFunc(h2mod->GetAddress<BYTE*>(0x0, 0x1CCFC), (BYTE*)dediCommandHook, 7);
}

void ServerConsole::logToDedicatedServerConsole(const wchar_t* string, ...) {

	if (!h2mod->Server)
		return;

	typedef signed int(dedi_print)(const wchar_t* str, ...);
	auto dedi_print_method = h2mod->GetAddress<dedi_print*>(0x0, 0x2354C8);

	dedi_print_method(string);
}

void ServerConsole::SendCommand(wchar_t** command, int split_commands_size, char unk)
{
	BYTE* unk1 = reinterpret_cast<BYTE*>(p_dedi_command_hook(command, split_commands_size, unk));
	BYTE* threadparams = h2mod->GetAddress<BYTE*>(0x0, 0x450680);

	typedef void(__cdecl* unk_func1)(void* a1);
	auto func_unk1 = h2mod->GetAddress<unk_func1>(0x0, 0x1D6EA);

	if (unk1)
	{
		if (*(BYTE*)(unk1 + 8))
		{
			typedef void(__thiscall* func_unk2_def)(void *thisx, LONG Value);
			auto func_unk2 = h2mod->GetAddress<func_unk2_def>(0x0, 0x6E00);
			func_unk2(*(BYTE**)(threadparams + 8), (LONG)unk1);

			func_unk1(unk1);
		}
		else
		{
			typedef void(__cdecl* unk_func3)(wchar_t *a1);
			auto func_unk3 = h2mod->GetAddress<unk_func3>(0x0, 0x19C93);

			func_unk1(unk1);
			func_unk3(*command);
		}
	}

	BYTE v8 = (*(BYTE**)(threadparams + 8))[20];
	*(BYTE*)(threadparams + 4) = v8;
	if (!v8)
		logToDedicatedServerConsole(L"\r\n");
}

