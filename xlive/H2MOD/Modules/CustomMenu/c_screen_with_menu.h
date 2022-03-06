#pragma once

#include "c_user_interface_widget.h"

class c_screen_with_menu;

// this is so we don't have to use the preserve/restore virtual table hack
// c_user_interface_widget is not the main parent class
// but for now it's ok
class c_screen_with_menu_game_base : protected c_user_interface_widget
{
protected:
	c_screen_with_menu_game_base::c_screen_with_menu_game_base(int menu_wgit_type, int a3, int a4, __int16 a5, void* a6)
	{
		typedef void* (__thiscall* c_screen_with_menu_ctor_game_impl)(c_screen_with_menu_game_base*, int, int, int, __int16, void*);
		auto p_c_screen_with_menu_ctor_game_impl = Memory::GetAddressRelative<c_screen_with_menu_ctor_game_impl>(0x611159);
		// call the constructor built-in game, which will set-up the vtable and everything
		p_c_screen_with_menu_ctor_game_impl(this, menu_wgit_type, a3, a4, a5, a6);
	}

	c_screen_with_menu_game_base::~c_screen_with_menu_game_base()
	{
	}

public:
	// c_screen_with_menu interface
	virtual void IUnkFunc22_maybe_debug() = 0;
	virtual int IUnkFunc23(int a2) = 0;
	virtual int IUnkFunc24() = 0;
	virtual BYTE* IUnkFunc25() = 0;
	virtual int IUnkFunc26() = 0;
	virtual int IUnkFunc27() = 0;
	virtual int IUnkFunc28(int a2) = 0;
	virtual bool IUnkFunc29(int* a2) = 0;
	virtual bool IUnkFunc30(int a2) = 0;
	virtual int IUnkFunc31(int a2) = 0;
	virtual bool IUnkFunc32(int a2) = 0;
	virtual int IUnkFunc33() = 0;
	virtual int IUnkFunc34() = 0;
	virtual unsigned int IUnkFunc35(int a2) = 0;
	virtual void IUnkFunc36_maybe_debug(int a2) = 0;
	virtual int IUnkFunc37(DWORD* a2) = 0;
	virtual int IUnkFunc38(int a2) = 0;
	virtual void* get_open_menu_cb() = 0;
	virtual bool IUnkFunc40() = 0;
	virtual void IUnkFunc41(bool a2) = 0;
};


// base class of many menus
class c_screen_with_menu : public c_screen_with_menu_game_base
{
public:

	c_screen_with_menu::c_screen_with_menu(int menu_wgit_type, int ui_channel, int a4, __int16 a5, void* a6) :
		c_screen_with_menu_game_base(menu_wgit_type, ui_channel, a4, a5, a6)
	{
	};

	c_screen_with_menu::~c_screen_with_menu()
	{
	}

	// base interface overrides
	virtual int custom_deconstructor(BYTE flags) override
	{
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<int(class_type::**)(int)>(0);
		return (this->**pFn)(flags);
	}

	virtual int IUnkFunc2() override
	{
		// typedef int(c_screen_with_menu::*fnT)(); works
		// WHILE FUCKING decltype(IUnkFunc2) doesn't
		// even Intellisense says the same fucking thing
		// aids compiler
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(1);

		return (this->* * pFn)();
	}

	virtual void IUnkFunc3() override
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(2);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc4() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(3);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc5_maybe_debug(int a2) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(4);
		(this->* * pFn)(a2);
	}

	virtual DWORD IUnkFunc6(DWORD* a2) override
	{
		typedef DWORD(class_type::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(5);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc7(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(6);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc8() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(7);
		return (this->* * pFn)();
	}

	virtual DWORD* IUnkFunc9(DWORD* a2) override
	{
		typedef DWORD* (class_type::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(8);
		return (this->* * pFn)(a2);
	}

	virtual void IUnkFunc10(int a1) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(9);
		return (this->* * pFn)(a1);
	}

	virtual int IUnkFunc11() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(10);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc12() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(11);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc13(DWORD* a2) override
	{
		typedef int(class_type::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(12);
		return (this->* * pFn)(a2);
	}

	virtual int get_top_most_parent_window_ui_channel() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(13);
		return (this->* * pFn)();
	}

	virtual int get_top_most_parent_window_index() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(14);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc16_maybe_debug(int a1)  override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(15);
		return (this->* * pFn)(a1);
	}

	virtual void IUnkFunc17_maybe_debug(int a1) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(16);
		return (this->* * pFn)(a1);
	}

	virtual int IUnkFunc18(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(17);
		return (this->* * pFn)(a2);
	}

	virtual bool IUnkFunc19() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(18);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc20() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(19);
		return (this->* * pFn)();
	}

	virtual bool IUnkFunc21() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(20);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc22_maybe_debug() override
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(21);
		return (this->* * pFn)();
	}

	// c_screen_with_menu specific interface
	virtual int IUnkFunc23(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(22);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc24() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(23);
		return (this->* * pFn)();
	}

	virtual BYTE* IUnkFunc25() override
	{
		typedef BYTE* (class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(24);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc26() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(25);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc27() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(26);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc28(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(27);
		return (this->* * pFn)(a2);
	}

	virtual bool IUnkFunc29(int* a2) override
	{
		typedef bool(class_type::** fnT)(int*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(28);
		return (this->* * pFn)(a2);
	}

	virtual bool IUnkFunc30(int a2) override
	{
		typedef bool(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(29);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc31(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(30);
		return (this->* * pFn)(a2);
	}

	virtual bool IUnkFunc32(int a2) override
	{
		typedef bool(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(31);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc33() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(32);
		return (this->* * pFn)();
	}

	virtual int IUnkFunc34() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(33);
		return (this->* * pFn)();
	}

	virtual unsigned int IUnkFunc35(int a2) override
	{
		typedef unsigned int(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(34);
		return (this->* * pFn)(a2);
	}

	virtual void IUnkFunc36_maybe_debug(int a2)  override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(35);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc37(DWORD* a2) override
	{
		typedef int(class_type::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(36);
		return (this->* * pFn)(a2);
	}

	virtual int IUnkFunc38(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(37);
		return (this->* * pFn)(a2);
	}

	virtual void* get_open_menu_cb() override
	{
		typedef void*(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(38);
		return (this->* * pFn)();
	}

	virtual bool IUnkFunc40() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(39);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc41(bool a2) override
	{
		typedef void(class_type::** fnT)(bool);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(40);
		(this->* * pFn)(a2);
	}

	// member functions
	void __thiscall build_player_list(void* a1, int player_count);

	// static member functions
	static void applyPatches();

	// data
	char c_screen_with_menu_data[2544];

private:
	typedef c_screen_with_menu class_type;

	template<typename T>
	static T c_screen_with_menu_base_vtable_get_func_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x7CF3A4)[idx]);
	}
};
static_assert(sizeof(c_screen_with_menu) == 2656);
