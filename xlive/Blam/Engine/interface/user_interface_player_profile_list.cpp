#include "stdafx.h"
#include  "user_interface_player_profile_list.h"

#include "user_interface_widget_list_item.h"
#include "saved_games/saved_game_files.h"

int32 c_player_profile_list::setup_children()
{
	return INVOKE_TYPE(0x237583, 0x0, int32(__thiscall*)(c_player_profile_list*), this);
}

void c_player_profile_list::on_screen_leave()
{
	INVOKE_TYPE(0x23685F, 0x0, void(__thiscall*)(c_player_profile_list*), this);
}

void c_player_profile_list::update()
{
	INVOKE_TYPE(0x2375A4, 0x0, void(__thiscall*)(c_player_profile_list*), this);
}

void c_player_profile_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	INVOKE_TYPE(0x23686E, 0x0, void(__thiscall*)(c_player_profile_list*, c_list_item_widget*, int32), this, item, skin_index);
}

void c_player_profile_list::unknown_function_2(int32 a1)
{
	INVOKE_TYPE(0x236B1E, 0x0, void(__thiscall*)(c_player_profile_list*, int32), this, a1);
}

__declspec(naked) void jmp_c_player_profile_list__update_displayed_profiles() { __asm { jmp c_player_profile_list::update_displayed_profiles } };

void c_player_profile_list::update_displayed_profiles()
{
	uint32 profile_storage_size;
	s_user_interface_player_profile_storage* profile_storage = this->get_displayed_player_profile_storage(&profile_storage_size);

	uint32 profile_indices[16];

	int32 enumerated_file_index = NONE;

	c_list_item_widget* current_child = (c_list_item_widget*)this->m_child_widget;

	memset(profile_indices, NONE, sizeof(uint32) * 16);
	for (int32 current_child_widget_index = 0; current_child_widget_index < 16; ++current_child_widget_index)
	{
		if (current_child->get_last_data_index() != NONE)
		{
			void* item = datum_get(this->m_list_data, current_child->get_last_data_index());
			if (item)
				profile_indices[current_child_widget_index] = *((int32*)item + 1);
		}
		current_child = (c_list_item_widget*)current_child->next_widget;

		if (!current_child)
			break;
	}

	if (profile_storage_size > 0)
	{
		for (int32 current_profile_storage_index = 0; current_profile_storage_index < profile_storage_size; ++current_profile_storage_index)
		{
			enumerated_file_index = profile_storage[current_profile_storage_index].enumerated_file_index;

			int32 current_player_profile_indices_index = 0;
			for (; current_player_profile_indices_index < 16; ++current_player_profile_indices_index)
			{
				if (enumerated_file_index == profile_indices[current_player_profile_indices_index])
				{
					if (enumerated_file_index != user_interface_globals_get_edit_player_profile_index())
					{
						profile_indices[current_player_profile_indices_index] = NONE;
						break;
					}
				}
			}

			if (current_player_profile_indices_index != 16)
				profile_indices[current_player_profile_indices_index] = NONE;

			if (current_player_profile_indices_index == 16 && enumerated_file_index != NONE)
				profile_storage[current_profile_storage_index].enumerated_file_index = NONE;
		}
	}

	for(int32 index = 0; index < 16; ++index)
	{
		const uint32 current_profile_index = profile_indices[index];
		if(current_profile_index != NONE)
		{
			if(profile_storage_size > 0)
			{
				uint32 current_profile_storage_index = 0;
				for (; current_profile_storage_index < profile_storage_size; ++current_profile_storage_index)
				{
					if (profile_storage[current_profile_storage_index].enumerated_file_index == NONE)
						break;
				}

				if(current_profile_storage_index >= 0 && current_profile_storage_index < profile_storage_size)
				{
					s_user_interface_player_profile_storage* stored_profile = &profile_storage[current_profile_storage_index];
					stored_profile->enumerated_file_index = current_profile_index;

					if(current_profile_index == user_interface_globals_get_edit_player_profile_index())
					{
						csmemcpy(&stored_profile->profile, user_interface_globals_get_edit_player_profile(), sizeof(s_saved_game_player_profile));
					}
					else
					{
						bool result = saved_game_player_profile_load(current_profile_index, &stored_profile->profile);
						if (!result)
						{
							csmemset(&stored_profile->profile, 0, sizeof(s_saved_game_player_profile));
							saved_games_get_display_name(enumerated_file_index, stored_profile->profile.name);
						}
					}
				}
			}
		}
	}

	//INVOKE_TYPE(0x2372D6, 0x0, void(__thiscall*)(c_player_profile_list*), this);
}

wchar_t* c_player_profile_list::unknown_function_3(int32 a1)
{
	return INVOKE_TYPE(0x2372D6, 0x0, wchar_t*(__thiscall*)(c_player_profile_list*, int32 a1), this, a1);
}

void user_interface_player_profile_list_apply_patches()
{
	// c_player_profile_list vtable
	WritePointer(Memory::GetAddress(0x3D284C), jmp_c_player_profile_list__update_displayed_profiles);
	// c_player_profile_list_fancy vtable
	WritePointer(Memory::GetAddress(0x3D01DC), jmp_c_player_profile_list__update_displayed_profiles);
	// c_player_profile_list_basic vtanle
	WritePointer(Memory::GetAddress(0x3CB834), jmp_c_player_profile_list__update_displayed_profiles);
}
