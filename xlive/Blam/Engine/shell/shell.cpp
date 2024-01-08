#include "stdafx.h"
#include "shell.h"

e_shell_tool_type shell_tool_type(void)
{
	return _shell_tool_type_game;
}

static int32* shell_startup_flags_get()
{
	return Memory::GetAddress<int32*>(0x46D820);
}

bool shell_startup_flag_is_set(e_startup_flags flag)
{
	return shell_startup_flags_get()[flag] != 0;
}

int32 __cdecl shell_startup_flag_get(e_startup_flags flag)
{
	static int32 flag_log_count[k_startup_flags_count];
	if (flag_log_count[(int32)flag] < 10)
	{
		LOG_TRACE_GAME("{} : flag {}", __FUNCTION__, (int32)flag);
		flag_log_count[(int32)flag]++;
		if (flag_log_count[(int32)flag] == 10)
			LOG_TRACE_GAME("{} : flag {} logged to many times ignoring", __FUNCTION__, (int32)flag);
	}

	return shell_startup_flags_get()[flag];
}

void shell_startup_flag_set(e_startup_flags flag, int32 state)
{
	shell_startup_flags_get()[flag] = state;
}