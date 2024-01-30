#pragma once

class c_simulation_watcher
{
public:
	static c_simulation_watcher* get()
	{
		// ### TODO dedi offset
		return *Memory::GetAddress<c_simulation_watcher**>(0x5178E0, 0x0);
	}

	bool __thiscall need_to_generate_updates()
	{
		// ### TODO dedi offset
		return INVOKE_TYPE(0x1D4B42, 0x0, bool(__thiscall*)(c_simulation_watcher*), this);
	}
};
