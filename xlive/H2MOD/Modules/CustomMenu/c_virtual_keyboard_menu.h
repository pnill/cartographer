#pragma once

#include "CustomMenuGlobals.h"
#include "c_screen_with_menu.h"
#include "c_user_interface_widget.h"
#include "c_screen_widget.h"

#define VIRTUAL_KEYBOARD_MENU_ID 167

#define VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX 17

// we just re-use the first seventeen keyboard id's in default code for now
#define VIRTUAL_KEYBOARD_MENU_TYPE_MAX_NEW (VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX * 2)

#define VIRTUAL_KEYBOARD_BUTTON_COUNT 47

class c_virtual_keyboard_menu : protected c_screen_widget
{
public:
	static void* __cdecl open(s_new_ui_screen_parameters* parameters);

	// TODO list initializer once the ctors are re-implemented
	c_virtual_keyboard_menu::c_virtual_keyboard_menu(int _ui_channel, int a4, __int16 _flags) :
		c_screen_widget::c_screen_widget(VIRTUAL_KEYBOARD_MENU_ID, _ui_channel, a4, _flags, false)
	{
		typedef c_virtual_keyboard_menu* (__thiscall* c_virtual_keyboard_menu_ctor_t)(c_virtual_keyboard_menu*, int, int, __int16);
		auto p_c_virtual_keyboard_menu_base_ctor = Memory::GetAddressRelative<c_virtual_keyboard_menu_ctor_t>(0x63BDF6);

		void* old_vtbl = *(void**)this;
		p_c_virtual_keyboard_menu_base_ctor(this, _ui_channel, a4, _flags);
		*(void**)this = old_vtbl;
	}

	c_virtual_keyboard_menu::~c_virtual_keyboard_menu()
	{
	}

	virtual int custom_deconstructor(BYTE flags) override
	{
		auto pFn = c_virtual_keyboard_menu_vtable_get_func_ptr<int(class_type::**)(int)>(0);
		return (this->** pFn)(flags);
	}

	virtual void IUnkFunc5_used_by_virtual_kb(int a2)
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_virtual_keyboard_menu_vtable_get_func_ptr<fnT>(4);
		(this->* * pFn)(a2);
	}

	virtual int IUnkFunc13(int* a2) override;

	virtual int IUnkFunc23(int a2);

	virtual void* get_open_menu_cb() override
	{
		return c_virtual_keyboard_menu::open;
	}

	void __thiscall set_input_string_buffer(wchar_t* buffer, size_t buffer_size);

	char virtual_keyboard_data[15232 - sizeof(c_screen_widget)];

private:
	typedef c_screen_widget class_type;

	template<typename T>
	static T c_virtual_keyboard_menu_vtable_get_func_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x7D302C)[idx]);
	}
};
static_assert(sizeof(c_virtual_keyboard_menu) == 15232);

void CustomMenuCall_VKeyboard_Inner(wchar_t* textBuffer, __int16 textBufferLen, int menuType, int menuIdTitle, int title, int menuIdDesc, int description);