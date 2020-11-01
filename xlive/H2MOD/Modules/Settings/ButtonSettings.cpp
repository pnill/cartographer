#include "ButtonSettings.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD.h"
#include "Blam/Cache/TagGroups/s_user_interface_screen_widget_definition.hpp"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Modules/Startup/Startup.h"
#include <commctrl.h>

namespace SettingsMenus
{
	namespace Controller
	{
		namespace ButtonLayout
		{
			typedef int(__fastcall* p_set_button_settings)(void* pThis,BYTE* pThis_1, int a2, __int16 *a3);
			p_set_button_settings set_button_settings;


			void __fastcall h_set_button_settings(void* pThis, byte* pThis_1, int a2, __int16 *a3)
			{
				LOG_INFO_GAME("{} {}", a2, IntToString<__int16>(*a3));
				//__int16 new_settings_index; // di
				//BYTE *v4; // ebx
				//BYTE *Player_profile_settings =  h2mod->GetAddress<byte*>(0x972870);
				//char valid_new_setting_index; // al
				//int v8; // ST20_4
				//int v9; // eax
				//char v11; // [esp+10h] [ebp-1680h]
				//new_settings_index = *a3;
				//switch (new_settings_index)
				//{
				//case 1: //Southpaw
				//	valid_new_setting_index = 1;
				//	break;
				//case 2: //Boxer
				//	valid_new_setting_index = 2;
				//	break;
				//case 3: //Green Thumb
				//	valid_new_setting_index = 3;
				//	break;
				//case 4: //Bumper Jumper
				//	valid_new_setting_index = 4;
				//	break;
				//default:
				//	valid_new_setting_index = 0;
				//	break;
				//}
				//Player_profile_settings[752] = valid_new_setting_index;
				//sub_B31AD0((int)&v11, (int)(Player_profile_settings + 748));
				//sub_B2EE72((int)&v11, (unsigned __int8)Player_profile_settings[753], valid_new_setting_index, Player_profile_settings[756]);
				//sub_B31B62((int)&v11, (int)(Player_profile_settings + 748));
				//if (pThis_1[704])
				//	sub_CD9C3E();
				//v8 = (*(int(__thiscall **)(BYTE *))(*(DWORD *)pThis_1 + 56))(pThis_1);
				//v9 = (*(int(__thiscall **)(BYTE *))(*(DWORD *)pThis_1 + 52))(pThis_1);
				//return sub_CD96DA(v9, v8);
				set_button_settings(pThis, pThis_1, a2, a3);
				h2mod->GetAddress<byte*>(0x972870)[752] = *a3;
			}

			void __stdcall get_button_settings_label(int a1, int a2)
			{
				void(__thiscall* sub_CEBF85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);
				int(__thiscall* sub_CE1909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);

				__int16 v2; // si
				int v3; // eax

				v2 = *(__int16 *)(a1 + 112);
				v3 = sub_CE1909(a1, 6, 0, 0);
				if (v3)
				{
					switch (v2)
					{
					case 0:
						sub_CEBF85(v3, DEFAULT);
						break;
					case 1:
						sub_CEBF85(v3, SOUTH_PAW);
						break;
					case 2:
						sub_CEBF85(v3, BOXER);
						break;
					case 3:
						sub_CEBF85(v3, GREEN_THUMB);
						break;
					case 4:
						sub_CEBF85(v3, BUMPER_JUMPER);
					default:
						sub_CEBF85(v3, -1);
						break;
					}
				}
			}

			void Initialize()
			{
				set_button_settings = (p_set_button_settings)DetourFunc(h2mod->GetAddress<byte*>(0x25D0AC), (byte*)h_set_button_settings, 5);
				WriteValue<byte>(h2mod->GetAddress((0x25D3BA)), 5);
				WriteValue<byte>(h2mod->GetAddress((0x25D3B8)), 5);
				WriteValue<int>(h2mod->GetAddress(0x3DB92C), reinterpret_cast<int>(std::addressof(get_button_settings_label)));
				auto wigt = tags::find_tag(blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, "ui\\screens\\game_shell\\settings_screen\\player_profile\\button_settings");
				auto t_wigt = tags::get_tag<blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, s_user_interface_screen_widget_definition>(wigt);
				for(auto i = 0; i < 13; i++)
				{
					t_wigt->panes[4]->text_blocks[i]->text_boundsbottom = t_wigt->panes[0]->text_blocks[i]->text_boundsbottom;
					t_wigt->panes[4]->text_blocks[i]->text_boundsleft = t_wigt->panes[0]->text_blocks[i]->text_boundsleft;
					t_wigt->panes[4]->text_blocks[i]->text_boundsright = t_wigt->panes[0]->text_blocks[i]->text_boundsright;
					t_wigt->panes[4]->text_blocks[i]->text_boundstop = t_wigt->panes[0]->text_blocks[i]->text_boundstop;
					t_wigt->panes[5]->text_blocks[i]->text_boundsbottom = t_wigt->panes[0]->text_blocks[i]->text_boundsbottom;
					t_wigt->panes[5]->text_blocks[i]->text_boundsleft = t_wigt->panes[0]->text_blocks[i]->text_boundsleft;
					t_wigt->panes[5]->text_blocks[i]->text_boundsright = t_wigt->panes[0]->text_blocks[i]->text_boundsright;
					t_wigt->panes[5]->text_blocks[i]->text_boundstop = t_wigt->panes[0]->text_blocks[i]->text_boundstop;
				}
				t_wigt->panes[4]->text_blocks[0]->string_id = 0x12341234;
				t_wigt->panes[4]->text_blocks[1]->string_id = e_button_strings::left_weapon;
				t_wigt->panes[4]->text_blocks[2]->string_id = e_button_strings::crouch;
				t_wigt->panes[4]->text_blocks[3]->string_id = e_button_strings::score;
				t_wigt->panes[4]->text_blocks[4]->string_id = e_button_strings::pause;
				t_wigt->panes[4]->text_blocks[5]->string_id = e_button_strings::right_weapon;
				//t_wigt->panes[4]->text_blocks[6]->string_id = e_button_strings
				t_wigt->panes[4]->text_blocks[7]->string_id = e_button_strings::switch_weapons;
				t_wigt->panes[4]->text_blocks[8]->string_id = e_button_strings::reload;
				t_wigt->panes[4]->text_blocks[9]->string_id = e_button_strings::swap_grenades;
				t_wigt->panes[4]->text_blocks[10]->string_id = e_button_strings::jump;
				t_wigt->panes[4]->text_blocks[10]->text_boundsleft = -375;
				t_wigt->panes[4]->text_blocks[11]->string_id = e_button_strings::meele;
				t_wigt->panes[4]->text_blocks[12]->string_id = e_button_strings::zoom;
				t_wigt->panes[4]->text_blocks[12]->text_boundsright = 420;
				for(auto i = 0; i < 3; i++)
				{
					t_wigt->panes[4]->bitmap_blocks[i]->topleft_x = t_wigt->panes[0]->bitmap_blocks[i]->topleft_x;
					t_wigt->panes[4]->bitmap_blocks[i]->topleft_y = t_wigt->panes[0]->bitmap_blocks[i]->topleft_y;
					t_wigt->panes[4]->bitmap_blocks[i]->initial_sprite_frame = 0;
					t_wigt->panes[5]->bitmap_blocks[i]->topleft_x = t_wigt->panes[0]->bitmap_blocks[i]->topleft_x;
					t_wigt->panes[5]->bitmap_blocks[i]->topleft_y = t_wigt->panes[0]->bitmap_blocks[i]->topleft_y;
					t_wigt->panes[5]->bitmap_blocks[i]->initial_sprite_frame = 0;
				}
				//MetaExtender::copy_tag_block<s_user_interface_screen_widget_definition::s_panes_block>((unsigned long)std::addressof(t_wigt->panes), 0, 4);
			}
		}
	}
}
