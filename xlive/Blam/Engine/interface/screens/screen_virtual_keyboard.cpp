#include "stdafx.h"
#include "screen_virtual_keyboard.h"

#include "H2MOD/Modules/CustomMenu/CustomMenuGlobals.h"

void sub_23CC18_CM(c_user_interface_widget* thisptr);

void ui_globals_set_keyboard_input_state(bool state)
{
	*Memory::GetAddress<bool*>(0x971290) = state;
}

void* __cdecl c_virtual_keyboard_menu::open(s_screen_parameters* parameters) 
{
	c_virtual_keyboard_menu* virtual_keyboard_menu = nullptr;
	BYTE* ui_buffer = ui_pool_allocate_space(sizeof(c_virtual_keyboard_menu), 0);
	if (ui_buffer) {
		virtual_keyboard_menu = new (ui_buffer) c_virtual_keyboard_menu(parameters->m_channel_type, parameters->m_window_index, parameters->user_flags);
		virtual_keyboard_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(virtual_keyboard_menu, parameters);
	return virtual_keyboard_menu;
}

void __thiscall c_virtual_keyboard_menu::set_input_string_buffer(wchar_t* buffer, uint32 buffer_size)
{
	INVOKE_TYPE(0x23B118, 0x0, void(__thiscall*)(c_virtual_keyboard_menu*, wchar_t*, int), this, buffer, buffer_size);
}

void ui_load_virtual_keyboard(wchar_t* out_keyboard_text, uint32 out_keyboard_text_lenght, int32 keyboard_type) {
	// VirtualKeyboardTypes - original
	// -1 allows all symbols and does not check the output string if valid for any purpose as below options do

	// 0 to 4 - profile name
	// 5,6 - variant name
	// 7 - variant name default
	// 8,9 - playlist name
	// 10, 11 - film name
	// 12, 15 - enter message text
	// 13 - rename squad
	// 16 - search for games by its description

	s_screen_parameters virtual_keyboard_params;
	virtual_keyboard_params.m_context = nullptr;
	virtual_keyboard_params.data_new(0, FLAG(0), _user_interface_channel_type_keyboard, _window_4, c_virtual_keyboard_menu::open);
	c_virtual_keyboard_menu* virtual_keyboard = (c_virtual_keyboard_menu*)virtual_keyboard_params.ui_screen_load_proc_exec();

	virtual_keyboard->m_keyboard_type = keyboard_type;
	virtual_keyboard->set_input_string_buffer(out_keyboard_text, out_keyboard_text_lenght);
}

// TODO **CLEANUP**
#pragma region Virtual Keyboard button handlers

void sub_23CC18_CM(c_user_interface_widget* thisptr)//__thiscall
{
	thisptr->start_widget_animation(3);
}

void __fastcall sub_23CD58_CM(void* thisptr, int _EDX, __int16 a2)//__thiscall
{
	//virtual key press handler
	//return psub_23CD58(thisptr, a2);

	//void(__thiscall* sub_23CD58)(void*, __int16) = (void(__thiscall*)(void*, __int16))((char*)H2BaseAddr + 0x23CD58);
	//return sub_23CD58(thisptr, a2);

	char(__cdecl * sub_4C6E0)(__int16) = (char(__cdecl*)(__int16))((char*)H2BaseAddr + 0x4C6E0);
	char(__thiscall * sub_23C4C9)(int, __int16) = (char(__thiscall*)(int, __int16))((char*)H2BaseAddr + 0x23C4C9);
	int(__thiscall * sub_23B9DE)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x23B9DE);
	int(__cdecl * sub_21DD04)(signed int) = (int(__cdecl*)(signed int))((char*)H2BaseAddr + 0x21DD04);
	signed int(__thiscall * sub_23B8F1)(void*) = (signed int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23B8F1);
	int(__cdecl * sub_20E1D8)(int, int, int, int, int, int) = (int(__cdecl*)(int, int, int, int, int, int))((char*)H2BaseAddr + 0x20E1D8);
	//char(__thiscall*sub_23CC18)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23CC18);
	int(__thiscall * sub_23BADA)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BADA);
	void(__thiscall * sub_23BB13)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BB13);
	void(__thiscall * sub_23BB4C)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BB4C);
	void(__thiscall * sub_23BB85)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BB85);

	int v4; // eax
	__int16* v5; // eax
	__int16* v6; // ebx
	int v7; // edi
	signed __int16 v8; // bp
	unsigned int v9; // eax
	bool v10; // cf
	bool v11; // zf
	int v12; // eax
	WORD* v13; // eax
	int error_message_id; // edi
	signed int v15; // eax

	int VKbMenuTypeDefault = *((DWORD*)thisptr + 663);

	if (VKbMenuTypeDefault != 12 || *((DWORD*)thisptr + 3806) == -1)
	{
		switch (a2)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35://all normal/alphabet vkeyboard keys
			v4 = (*(int(__thiscall**)(void*))(*((DWORD*)thisptr + 64 * a2 + 798) + 76))((void*)((a2 << 8) + (DWORD)thisptr + 0xc78));
			v5 = (__int16*)(*(int(__thiscall**)(int))(*(DWORD*)v4 + 12))(v4);
			v6 = v5;
			v7 = 0;
			if (*v5)
			{
				do
				{
					v8 = *v5;
					if (sub_4C6E0(*v5))
						v8 = 9633;
					if (!sub_23C4C9((int)thisptr, v8))
						break;
					v5 = &v6[++v7];
				} while (v6[v7]);
			}
			break;
		case 36://backspace
			sub_23B9DE((int)thisptr);
			break;
		case 37://space
			sub_23C4C9((int)thisptr, 32);
			break;
		case 38://move cursor left
			v9 = *((DWORD*)thisptr + 794);
			v10 = v9 < *((DWORD*)thisptr + 793);
			v11 = v9 == *((DWORD*)thisptr + 793);
			*((BYTE*)thisptr + 3188) = 0;
			if (v10 || v11)
				goto LABEL_17;
			v12 = v9 - 2;
			goto LABEL_16;
		case 39://move cursor right
			v13 = (WORD*)*((DWORD*)thisptr + 794);
			*((BYTE*)thisptr + 3188) = 0;
			if (*v13)
			{
				v12 = (int)(v13 + 1);
			LABEL_16:
				*((DWORD*)thisptr + 794) = v12;
			}
		LABEL_17:
			sub_21DD04(6);
			break;
		case 40: // Done button
			if (VKbMenuTypeDefault != -1 && (error_message_id = 0, (v15 = sub_23B8F1(thisptr)) != 0))
			{
				switch (VKbMenuTypeDefault)//error with message type
				{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
					if (v15 == 1)
					{
						error_message_id = 88;
					}
					else if (v15 == 2)
					{
						// error_message_id = 70;

						// replace already exist error message with success
						goto success;
					}
					break;
				case 5:
				case 6:
				case 7:
					if (v15 == 1)
					{
						error_message_id = 89;
					}
					else if (v15 == 2)
					{
						error_message_id = 71;
					}
					break;
				default:
					break;
				}

				sub_20E1D8(1, error_message_id, 4, *((unsigned __int16*)thisptr + 4), 0, 0);//can get rid of this and do custom message
			}
			else
			{
				// success. however it will open other menus like customise new variant
				// we replace the original function that would call the default menus
			success:
				sub_23CC18_CM((c_user_interface_widget*)thisptr);
			}
			break;
		case 41://shift button
			sub_23BADA(thisptr);
			break;
		case 42://caps lock
			sub_23BB13(thisptr);
			break;
		case 43://"ACCENTS" button
			if (VKbMenuTypeDefault == 12 || VKbMenuTypeDefault == 15 || VKbMenuTypeDefault == 16)
				sub_23BB4C(thisptr);
			break;
		case 44://"SYMBOLS" button
			if (VKbMenuTypeDefault == 12 || VKbMenuTypeDefault == 15 || VKbMenuTypeDefault == 16)
				sub_23BB85(thisptr);
			break;
		default:
			return;
		}
	}
}

bool c_virtual_keyboard_button::handle_virtual_keyboard_event(s_event_record* event)
{
	//return psub_23CF88(thisptr, a2);

	//char(__thiscall* sub_23CF88)(int, int*) = (char(__thiscall*)(int, int*))((char*)H2BaseAddr + 0x23CF88);
	//return sub_23CF88(thisptr, a2);

	int(__thiscall * sub_211BA1)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x211BA1);
	int(__cdecl *sub_209A8C)() = (int(*)())((char*)H2BaseAddr + 0x209A8C);
	void(__cdecl * sub_43233)(signed int) = (void(__cdecl*)(signed int))((char*)H2BaseAddr + 0x43233);
	char(__thiscall * sub_212152)(int, int) = (char(__thiscall*)(int, int))((char*)H2BaseAddr + 0x212152);
	int(__thiscall * sub_20E807)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x20E807);
	char(__thiscall * sub_212660)(int, int, char) = (char(__thiscall*)(int, int, char))((char*)H2BaseAddr + 0x212660);
	//void(__thiscall* sub_23CD58)(void*, __int16) = (void(__thiscall*)(void*, __int16))((char*)H2BaseAddr + 0x23CD58);
	char(__thiscall * sub_2118F0)(int, int) = (char(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2118F0);

	int thisptr = (int)this;
	int* a2 = (int*)event;

	int v2; // esi
	char v3; // bl
	DWORD* v4; // ebp
	int v5; // ecx
	int VKbMenuTypeDefault; // eax
	//int v7; // eax
	DWORD* v8; // eax
	char v10; // [esp+13h] [ebp-1h]

	bool v20 = false;

	v2 = thisptr;
	v10 = 0;
	v3 = 0;
	v4 = (DWORD*)sub_211BA1(thisptr);
	if (*a2 == 5)
	{
		v5 = a2[2];
		if (v5)
		{
			if (v5 == 1 || v5 == 13)
				v3 = 1;//close menu button pressed (cancel)
		}
		else
		{
			v10 = 1;
		}
	}
	if (*a2 == 7 && a2[2] == 27 || v3)//close menu esc key
	{
		VKbMenuTypeDefault = v4[663];
		/*if (VKbMenuType == 7 || VKbMenuType == 5)
		{
			v7 = sub_209A8C();
			if (v7 != -1)
				sub_43233(v7);
		}*/
		v20 = true;
	}
	if (*a2 == 6)//virtual key pressed
	{
		if (!sub_212152(v2, 0))
		{
			if (sub_20E807((int)v4))
			{
				v8 = (DWORD*)sub_20E807((int)v4);
				sub_212660((int)v8, 0, 0);
			}
			sub_212660(v2, 0, 1);
		}
		//goto LABEL_20;
	}
	char result;
	if (v10 || *a2 == 6)
	{
		//LABEL_20:
		sub_23CD58_CM((void*)v4, 0, *(WORD*)(v2 + 0xF8));//handles type input.
		result = 1;
	}
	else {
		result = sub_2118F0(thisptr, (int)a2);//only handles all kinds of menu exiting.
	}
	return result;
}

bool c_virtual_keyboard_menu::handle_event(s_event_record* event)
{
	//return psub_23D060(thisptr, event);

	void* thisptr = (void*)this;

	char(__thiscall * sub_23AF4E)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x23AF4E);
	void(__thiscall * sub_23BBBE)(void*, void*, int) = (void(__thiscall*)(void*, void*, int))((char*)H2BaseAddr + 0x23BBBE);
	int(__thiscall * sub_23B080)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23B080);
	int(__cdecl * sub_21DD04)(signed int) = (int(__cdecl*)(signed int))((char*)H2BaseAddr + 0x21DD04);
	int(__thiscall * sub_23B9DE)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x23B9DE);
	int(__thiscall * sub_23BADA)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BADA);
	char(__thiscall * sub_23C4C9)(int, __int16) = (char(__thiscall*)(int, __int16))((char*)H2BaseAddr + 0x23C4C9);
	//void(__thiscall* sub_23CD58)(void*, __int16) = (void(__thiscall*)(void*, __int16))((char*)H2BaseAddr + 0x23CD58);
	char(__thiscall * sub_20EB2B)(void*, s_event_record*) = (char(__thiscall*)(void*, s_event_record*))((char*)H2BaseAddr + 0x20EB2B);
	void(__thiscall * sub_23B5C1)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23B5C1);
	int(__cdecl * sub_4BD54)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x4BD54);
	char(__cdecl * sub_4C6E0)(__int16) = (char(__cdecl*)(__int16))((char*)H2BaseAddr + 0x4C6E0);

	void* dword_3D2D78 = (void*)(H2BaseAddr + 0x3D2D78);
	void* dword_3D2A78 = (void*)(H2BaseAddr + 0x3D2A78);
	void* dword_3D2B38 = (void*)(H2BaseAddr + 0x3D2B38);
	void* dword_3D2CB8 = (void*)(H2BaseAddr + 0x3D2CB8);
	void* dword_3D2E38 = (void*)(H2BaseAddr + 0x3D2E38);
	BYTE* byte_3D2F30 = (BYTE*)((char*)H2BaseAddr + 0x3D2F30);

	unsigned int v6; // eax
	bool v7; // cf
	int v8; // eax
	bool result;
	WORD* v10; // eax
	WORD* v11; // eax
	char v12; // bl
	void* v13 = nullptr; // eax

	signed __int16 v20;

	switch (event->type)
	{
	case _user_interface_event_type_tab_up:
	case _user_interface_event_type_tab_left:
	case _user_interface_event_type_tab_down:
	case _user_interface_event_type_tab_right:
	{
		v12 = sub_23AF4E((signed __int16*)thisptr, event->type);
		switch (v12)
		{
		case 41:
			if (*((BYTE*)thisptr + 3186) & 1)
				goto LABEL_47;
			v13 = dword_3D2D78;
			goto LABEL_48;
		case 42:
			if (*((BYTE*)thisptr + 3186) & 2)
				goto LABEL_47;
			v13 = dword_3D2A78;
			goto LABEL_48;
		case 43:
			if (*((BYTE*)thisptr + 3186) & 4)
				goto LABEL_47;
			v13 = dword_3D2B38;
			goto LABEL_48;
		case 44:
			v13 = dword_3D2CB8;
			if (*((BYTE*)thisptr + 3186) & 8)
				LABEL_47:
			v13 = dword_3D2E38;
		LABEL_48:
			sub_23BBBE(thisptr, v13, 47);
			break;
		default:
			break;
		}
		int VKbMenuType = *((DWORD*)thisptr + 663);
		if (byte_3D2F30[8 * VKbMenuType] & 0xA && (v12 == 43 || v12 == 44))
			v12 = sub_23AF4E((signed __int16*)thisptr, event->type);
		(*(void(__thiscall**)(void*, BYTE*))(*(DWORD*)thisptr + 36))(thisptr, (BYTE*)thisptr + 256 * v12 + 3192);
		result = 1;
	}
	break;

	case _user_interface_event_type_gamepad_button_pressed:
	{
		switch (event->component)
		{
		case 1:
		case 13:
			sub_23B080(thisptr);
			break;
		case 6:
		LABEL_9:
			v6 = *((DWORD*)thisptr + 794);
			*((BYTE*)thisptr + 3188) = 0;
			if (v6 < *((DWORD*)thisptr + 793) || v6 == *((DWORD*)thisptr + 793))
				goto LABEL_12;
			v8 = v6 - 2;
			goto LABEL_11;
		case 7:
		LABEL_15:
			v10 = (WORD*)*((DWORD*)thisptr + 794);
			*((BYTE*)thisptr + 3188) = 0;
			if (!*v10)
			{
			LABEL_12:
				sub_21DD04(6);
				return 1;
			}
			v8 = (int)(v10 + 1);
		LABEL_11:
			*((DWORD*)thisptr + 794) = v8;
			goto LABEL_12;
		case 2:
		LABEL_33:
			sub_23B9DE((int)thisptr);
			return 1;
		case 14:
		case 15:
			sub_23BADA(thisptr);
			return 1;
		case 5:
			sub_23C4C9((int)thisptr, 32);
			return 1;
		case 12:
		LABEL_23:
			sub_23CD58_CM(thisptr, 0, 40);
			return 1;
		}
		return sub_20EB2B(thisptr, event);
	}
	break;

	default:
	{
		if (event->type != _user_interface_event_type_keyboard_button_pressed)
		{
			return sub_20EB2B(thisptr, event);
		}
		else
		{
			switch (event->component)
			{
			case 8:
				goto LABEL_33;
			case 13:
				goto LABEL_23;
			case 27:
				sub_23B080(thisptr);
				event->type = _user_interface_event_type_gamepad_button_pressed;
				event->component = _user_interface_controller_component_button_b;
				return sub_20EB2B(thisptr, event);
			case 37:
				goto LABEL_9;
			case 38:
			case 40://Up & Down Arrows
				return 1;
			case 39:
				goto LABEL_15;
			case 46:
				sub_23B5C1(thisptr);
				v11 = (WORD*)*((DWORD*)thisptr + 794);
				if (!*v11)
					return sub_20EB2B(thisptr, event);
				*((BYTE*)thisptr + 3188) = 0;
				if (*v11)
					*((DWORD*)thisptr + 794) = (DWORD)(v11 + 1);
				sub_21DD04(6);
				goto LABEL_33;
			default:
				v20 = event->value;
				if (v20 == -1 || sub_4BD54(v20) || sub_4C6E0(v20))
					return sub_20EB2B(thisptr, event);
				sub_23C4C9((int)thisptr, v20);
				result = 1;
				break;
			}
		}
	}
	break;
	}

	return result;
}


void c_virtual_keyboard_menu::initialize(s_screen_parameters* parameters)
{
	INVOKE_VFPTR_FN(_get_vfptr_table, 22, void(class_type::**)(s_screen_parameters *), parameters);

	// set the default header and description text

	m_header_text.set_text(L"");
	try_find_text_widget(3)->set_text(L"");
}

#pragma endregion
