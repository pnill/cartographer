#pragma once

class c_simulation_watcher
{
public:
	static c_simulation_watcher* get()
	{
		return *Memory::GetAddress<c_simulation_watcher**>(0x5178E0, 0x520B70);
	}

	bool __thiscall need_to_generate_updates()
	{
		return INVOKE_TYPE(0x1D4B42, 0x1C188C, bool(__thiscall*)(c_simulation_watcher*), this);
	}
};
