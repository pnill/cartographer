#pragma once
#pragma once
struct s_machine_identifier
{
	__int16 machine_index;
	__int16 machine_index1;
	__int16 unk;
};

struct s_machine_array
{
	s_machine_identifier machine_identifier[16];
	DWORD unk_60;
};
