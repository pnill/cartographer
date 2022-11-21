#pragma once

#include "Blam/Engine/Game/memory/data.h"
#include "c_user_interface_widget.h"

#include "CustomMenuGlobals.h"

class c_list_widget : protected c_user_interface_widget
{
public:
	// c_lost_widget specific data
	s_data_array* list_data_array;
	char gap_70[60];

	// if you are implementing the child's class ctor, set _call_ctor to true
	// if not (you're calling the game's bin ctor), set to false
	c_list_widget(int a2, bool _call_ctor)
		: c_user_interface_widget(_widget_type_list_widget, a2, false)
	{
		typedef c_list_widget* (__thiscall* c_list_widget_ctor_t)(c_list_widget*, int);
		auto p_c_list_widget = Memory::GetAddressRelative<c_list_widget_ctor_t>(0x613B1C);

		void* old_vtbl = *(void**)this;
		if (_call_ctor)
		{
			p_c_list_widget(this, a2);
		}
		*(void**)this = old_vtbl;
	}

	// interface
	virtual int custom_deconstructor(BYTE flags) override
	{
		auto pFn = c_list_widget_base_vtable_get_func_ptr<int(class_type::**)(int)>(0);
		return (this->* * pFn)(flags);
	}

	virtual int IUnkFunc2() override
	{
		// typedef int(c_list_widget::*fnT)(); works
		// WHILE FUCKING decltype(IUnkFunc2) doesn't
		// even Intellisense says the same fucking thing
		// aids compiler
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(1);

		return (this->* * pFn)();
	}

	virtual void IUnkFunc3() override
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(2);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc4() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(3);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc5_used_by_virtual_kb(int a2) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(4);
		(this->* * pFn)(a2);
	}

	virtual DWORD IUnkFunc6(DWORD* a2) override
	{
		typedef DWORD(class_type::** fnT)(DWORD*);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(5);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc7(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(6);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc8() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(7);
		return (this->* * pFn)();
	}

	virtual DWORD* IUnkFunc9(DWORD* a2) override
	{
		typedef DWORD* (class_type::** fnT)(DWORD*);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(8);
		return (this->* * pFn)(a2);
	}

	virtual void IUnkFunc10(int a1) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(9);
		return (this->* * pFn)(a1);
	}

	virtual int IUnkFunc11() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(10);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc12() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(11);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc13(int* a2) override
	{
		typedef int(class_type::** fnT)(int*);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(12);
		return (this->* * pFn)(a2);
	}

	virtual int get_top_most_parent_widget_ui_channel() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(13);
		return (this->* * pFn)();
	}

	virtual int get_top_most_parent_widget_index() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(14);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc16_maybe_debug(int a2) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(15);
		return (this->* * pFn)(a2);
	}

	virtual void IUnkFunc17_maybe_debug(int a2) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(16);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc18(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(17);
		return (this->* * pFn)(a2);
	}

	virtual bool IUnkFunc19() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(18);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc20() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(19);
		return (this->* * pFn)();
	}

	virtual bool IUnkFunc21() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(20);
		return (this->* * pFn)();
	}

	// bellow class_type's specific virtual functions

	virtual void IUnkFunc22()
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(21);
		return (this->* * pFn)();
	}

	// these next three need to be implemented, otherwise pure virtual function call exception will be raised
	virtual char* get_item_list()
	{
		typedef char*(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(22);
		return (this->* * pFn)();
	}

	virtual int get_list_item_count()
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(23);
		return (this->* * pFn)();
	}

	virtual void get_label(int a1, int a2)
	{
		typedef void(class_type::** fnT)(int, int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(24);
		return (this->* * pFn)(a1, a2);
	}

	// this doesn't have to be implemented
	virtual bool IUnkFunc26(int a1)
	{
		typedef bool(class_type::** fnT)(int);
		auto pFn = c_list_widget_base_vtable_get_func_ptr<fnT>(25);
		return (this->* * pFn)(a1);
	}

	// member functions

	void set_selected_list_button_idx(__int16 idx)
	{
		auto p_set_selected_list_button_idx = Memory::GetAddress<void(__thiscall*)(void*, int)>(0x213F50);
		p_set_selected_list_button_idx(this, idx);
	}

	static struct s_data_array* allocate_list_data(const char* data_name, int total_elements, int size)
	{
		auto p_allocate_list_data = Memory::GetAddressRelative<s_data_array*(__cdecl*)(const char*, int, int)>(0x60D1FD);
		return p_allocate_list_data(data_name, total_elements, size);
	}

private:
	template<typename T>
	static T c_list_widget_base_vtable_get_func_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x7CF5FC)[idx]);
	}

	typedef c_list_widget class_type;
};
static_assert(sizeof(c_list_widget) == 176, "invalid c_list_widget size!");
static_assert(offsetof(c_list_widget, list_data_array) == 112);