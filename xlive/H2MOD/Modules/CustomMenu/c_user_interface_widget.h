#pragma once

enum e_user_interface_widget_type
{
	_widget_type_screen_widget = 0,
	_widget_type_list_widget = 1,
	_widget_type_list_item_widget = 2,
	_widget_type_button_widget = 3,
	_widget_type_tab_view_widget = 5,
	_widget_type_text_widget = 6,
	_widget_type_model_widget = 7,
	_widget_type_bitmap_widget = 8,
	_widget_type_hud_widget = 9,
	_widget_type_player_widget = 10
};

// base class for many (if not all) UI widgets
class c_user_interface_widget
{
protected:

	// in child classes the constructors of base classes are called manually, only if it is actually needed
	// (when reimplementing the entire child class ctor for example)
	// because when calling the constructor of that child class directly from the game's code
	// it implicitly calls the base class ctors as well
	c_user_interface_widget::c_user_interface_widget(e_user_interface_widget_type _widget_type, __int16 _flags, bool _call_ctor)
	{
		typedef c_user_interface_widget* (__thiscall* class_constructor_t)(e_user_interface_widget_type, __int16);
		auto p_class_constructor = Memory::GetAddressRelative<class_constructor_t>(0x611D81);

		void* old_vtbl = *(void**)this;
		if (_call_ctor)
		{
			p_class_constructor(_widget_type, _flags);
		}
		*(void**)this = old_vtbl;
	}

	// void* vtbl;
	int m_widget_type;
	__int16 m_flags;
	__int16 field_A;
	int field_C;
	int parent_widget;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
	DWORD field_34;
	DWORD field_38;
	WORD field_3C;
	WORD field_3E;
	BYTE gap_40[2];
	WORD field_42;
	DWORD field_44;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	__int16 field_68;
	__int16 field_6A;
	bool field_6C;
	bool field_6D;
	bool field_6E;
	bool field_6F;

	virtual int custom_deconstructor(BYTE flags)
	{
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<int(class_type::**)(int)>(0);
		return (this->* * pFn)(flags);
	}

	virtual int IUnkFunc2()
	{
		// typedef int(class_type::*fnT)(); works
		// WHILE FUCKING decltype(IUnkFunc2) doesn't
		// even Intellisense says the same fucking thing
		// aids compiler
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(1);

		return (this->* * pFn)();
	}

	virtual void IUnkFunc3()
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(2);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc4()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(3);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc5_used_by_virtual_kb(int a2)
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(4);
		(this->* * pFn)(a2);
	}

	virtual DWORD IUnkFunc6(DWORD* a2)
	{
		typedef DWORD(class_type::** fnT)(DWORD*);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(5);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc7(int a2)
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(6);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc8()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(7);
		return (this->* * pFn)();
	}

	virtual DWORD* IUnkFunc9(DWORD* a2)
	{
		typedef DWORD* (class_type::** fnT)(DWORD*);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(8);
		return (this->* * pFn)(a2);
	}

	virtual void IUnkFunc10(int a1)
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(9);
		return (this->* * pFn)(a1);
	}

	virtual int IUnkFunc11()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(10);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc12()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(11);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc13(int* a2)
	{
		typedef int(class_type::** fnT)(int*);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(12);
		return (this->* * pFn)(a2);
	}

	virtual int get_top_most_parent_widget_ui_channel()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(13);
		return (this->* * pFn)();
	}

	virtual int get_top_most_parent_widget_index()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(14);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc16_maybe_debug(int a2)
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(15);
		return (this->* * pFn)(a2);
	}

	virtual void IUnkFunc17_maybe_debug(int a2)
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(16);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc18(int a2)
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(17);
		return (this->* * pFn)(a2);
	}

	virtual bool IUnkFunc19()
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(18);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc20()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(19);
		return (this->* * pFn)();
	}

	virtual bool IUnkFunc21()
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_user_interface_widget_base_vtable_get_func_ptr<fnT>(20);
		return (this->* * pFn)();
	}

private:
	template<typename T>
	static T c_user_interface_widget_base_vtable_get_func_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x7CF44C)[idx]);
	}

	typedef c_user_interface_widget class_type;
};

static_assert(sizeof(c_user_interface_widget) == 112);
