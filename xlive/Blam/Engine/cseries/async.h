#pragma once

struct s_async_completion
{
	bool completed;
	bool unk_1;
	bool unk_2;
	bool unk_3;
	int32 unk_4;
	char file_path[2048];
	real32 unk_808;
};
ASSERT_STRUCT_SIZE(s_async_completion, 2060);

void __cdecl async_initialize(void);

void __cdecl async_yield_until_done(s_async_completion* completion, bool play_sound);