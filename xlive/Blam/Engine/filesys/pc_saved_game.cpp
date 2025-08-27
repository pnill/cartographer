#include "stdafx.h"
#include "pc_saved_game.h"

int32 pc_saved_game_get_next_available_save_location(char* flat_path, wchar_t* full_display_name, int32 unk_1, int32 unk_2,
	wchar_t* wide_path, int32 wide_path_size)
{
	return INVOKE(0x9C8D7, 0, pc_saved_game_get_next_available_save_location, flat_path, full_display_name, unk_1, unk_2, wide_path, wide_path_size);
}