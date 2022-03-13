#pragma once

class c_screen_with_menu;

template<typename T>
static T c_screen_with_menu_base_vtable_get_func_ptr(DWORD idx)
{
	return reinterpret_cast<T>(Memory::GetAddressRelative<void**>(0x7CF3A4)[idx]);
}

// this is so we don't have to use the preserve/restore virtual table hack
class c_screen_with_menu_game_base
{
protected:
	c_screen_with_menu_game_base::c_screen_with_menu_game_base(int menu_wgit_type, int a3, int a4, __int16 a5, void* a6)
	{
		typedef void* (__thiscall* c_screen_with_menu_ctor_game_impl)(c_screen_with_menu_game_base*, int a2, int a3, int a4, __int16 a5, void* a6);
		auto p_c_screen_with_menu_ctor_game_impl = Memory::GetAddressRelative<c_screen_with_menu_ctor_game_impl>(0x611159);
		// call the constructor built-in game, which will set-up the vtable and everything
		p_c_screen_with_menu_ctor_game_impl(this, menu_wgit_type, a3, a4, a5, a6);
	}

	c_screen_with_menu_game_base::~c_screen_with_menu_game_base()
	{
	}

	// interface
	virtual int custom_deconstructor(int a1) = 0;
	virtual int sub_611E23() = 0;
	virtual void sub_611488() = 0;
	virtual int sub_612CD8() = 0;
	virtual void nullsub_213(int a2) = 0;
	virtual DWORD sub_6114E0(DWORD* a2) = 0;
	virtual int sub_611692(int a2) = 0;
	virtual int sub_6116BD() = 0;
	virtual DWORD* sub_611703(DWORD* a2) = 0;
	virtual void sub_612A7C() = 0;
	virtual int sub_612ABC() = 0;
	virtual int sub_612BCA() = 0;
	virtual int sub_60EEBE(DWORD* a2) = 0;
	virtual int sub_6120F8() = 0;
	virtual int sub_612120() = 0;
	virtual void nullsub_227(int a1) = 0;
	virtual void nullsub_228(int a1) = 0;
	virtual int sub_60EF08(int a2) = 0;
	virtual bool sub_40AD4D() = 0;
	virtual int sub_40AD50() = 0;
	virtual bool sub_60E885() = 0;
	virtual void nullsub_138() = 0;
	virtual int sub_6111AB(int a2) = 0;
	virtual int sub_60F790() = 0;
	virtual BYTE* sub_610B4E() = 0;
	virtual int sub_6102C5() = 0;
	virtual int sub_6103D6() = 0;
	virtual int sub_60F1F4(int a2) = 0;
	virtual bool sub_60EFC1(int* a2) = 0;
	virtual bool sub_60F081(int a2) = 0;
	virtual int sub_60F151(int a2) = 0;
	virtual bool sub_40AD53(int a2) = 0;
	virtual int sub_60EB8A() = 0;
	virtual int sub_60EB8E() = 0;
	virtual unsigned int sub_60EB92(int a2) = 0;
	virtual void nullsub_139(int a2) = 0;
	virtual int sub_60EC5C(DWORD* a2) = 0;
	virtual int sub_60ECC9(int a2) = 0;
	virtual void sub_688255() = 0;
	virtual bool sub_40AD58() = 0;
	virtual void sub_60F2A4(bool a2) = 0;

	// data
	// vtable doesn't need to be declared
	// void** vtbl;
	// char data[4092];
public:
};


// base class of many menus
class c_screen_with_menu : public c_screen_with_menu_game_base
{
public:

	c_screen_with_menu::c_screen_with_menu(int menu_wgit_type, int a3, int a4, __int16 a5, void* a6) :
		c_screen_with_menu_game_base(menu_wgit_type, a3, a4, a5, a6)
	{
	};

	c_screen_with_menu::~c_screen_with_menu()
	{
	}

	// interface
	virtual int custom_deconstructor(int a1)
	{
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<int(c_screen_with_menu::**)(int)>(0);
		return (this->**pFn)(a1);
	}

	virtual int sub_611E23()
	{
		// typedef int(c_screen_with_menu::*fnT)(); works
		// WHILE FUCKING decltype(sub_611E23) doesn't
		// even Intellisense says the same fucking thing
		// aids compiler
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(1);

		return (this->**pFn)();
	}

	virtual void sub_611488()
	{
		typedef void(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(2);
		return (this->**pFn)();
	}

	virtual int sub_612CD8()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(3);
		return (this->**pFn)();
	}

	virtual void nullsub_213(int a2)
	{
		typedef void(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(4);
		(this->**pFn)(a2);
	}

	virtual DWORD sub_6114E0(DWORD* a2)
	{
		typedef DWORD(c_screen_with_menu::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(5);
		return (this->**pFn)(a2);
	}

	virtual int sub_611692(int a2)
	{
		typedef int(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(6);
		return (this->**pFn)(a2);
	}

	virtual int sub_6116BD()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(7);
		return (this->**pFn)();
	}

	virtual DWORD* sub_611703(DWORD* a2)
	{
		typedef DWORD*(c_screen_with_menu::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(8);
		return (this->**pFn)(a2);
	}

	virtual void sub_612A7C()
	{
		typedef void(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(9);
		return (this->**pFn)();
	}

	virtual int sub_612ABC()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(10);
		return (this->**pFn)();
	}

	virtual int sub_612BCA()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(11);
		return (this->**pFn)();
	}

	virtual int sub_60EEBE(DWORD* a2)
	{
		typedef int(c_screen_with_menu::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(12);
		return (this->**pFn)(a2);
	}

	virtual int sub_6120F8()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(13);
		return (this->**pFn)();
	}

	virtual int sub_612120()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(14);
		return (this->**pFn)();
	}

	virtual void nullsub_227(int a1) 
	{
		typedef void(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(15);
		return (this->**pFn)(a1);
	}

	virtual void nullsub_228(int a1) 
	{
		typedef void(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(16);
		return (this->**pFn)(a1);
	}

	virtual int sub_60EF08(int a2)
	{
		typedef int(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(17);
		return (this->**pFn)(a2);
	}

	virtual bool sub_40AD4D()
	{
		typedef bool(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(18);
		return (this->**pFn)();
	}

	virtual int sub_40AD50()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(19);
		return (this->**pFn)();
	}

	virtual bool sub_60E885()
	{
		typedef bool(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(20);
		return (this->**pFn)();
	}

	virtual void nullsub_138() 
	{
		typedef void(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(21);
		return (this->**pFn)();
	}

	virtual int sub_6111AB(int a2)
	{
		typedef int(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(22);
		return (this->**pFn)(a2);
	}

	virtual int sub_60F790()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(23);
		return (this->**pFn)();
	}

	virtual BYTE* sub_610B4E()
	{
		typedef BYTE*(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(24);
		return (this->**pFn)();
	}

	virtual int sub_6102C5()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(25);
		return (this->**pFn)();
	}

	virtual int sub_6103D6()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(26);
		return (this->**pFn)();
	}

	virtual int sub_60F1F4(int a2)
	{
		typedef int(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(27);
		return (this->**pFn)(a2);
	}

	virtual bool sub_60EFC1(int* a2)
	{
		typedef bool(c_screen_with_menu::** fnT)(int*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(28);
		return (this->**pFn)(a2);
	}

	virtual bool sub_60F081(int a2)
	{
		typedef bool(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(29);
		return (this->**pFn)(a2);
	}

	virtual int sub_60F151(int a2)
	{
		typedef int(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(30);
		return (this->**pFn)(a2);
	}

	virtual bool sub_40AD53(int a2)
	{
		typedef bool(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(31);
		return (this->**pFn)(a2);
	}

	virtual int sub_60EB8A()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(32);
		return (this->**pFn)();
	}

	virtual int sub_60EB8E()
	{
		typedef int(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(33);
		return (this->**pFn)();
	}

	virtual unsigned int sub_60EB92(int a2)
	{
		typedef unsigned int(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(34);
		return (this->**pFn)(a2);
	}

	virtual void nullsub_139(int a2) 
	{
		typedef void(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(35);
		return (this->**pFn)(a2);
	}

	virtual int sub_60EC5C(DWORD* a2)
	{
		typedef int(c_screen_with_menu::** fnT)(DWORD*);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(36);
		return (this->**pFn)(a2);
	}

	virtual int sub_60ECC9(int a2)
	{
		typedef int(c_screen_with_menu::** fnT)(int);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(37);
		return (this->**pFn)(a2);
	}

	virtual void sub_688255()
	{
		typedef void(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(38);
		(this->**pFn)();
	}

	virtual bool sub_40AD58()
	{
		typedef bool(c_screen_with_menu::** fnT)();
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(39);
		return (this->**pFn)();
	}

	virtual void sub_60F2A4(bool a2)
	{
		typedef void(c_screen_with_menu::** fnT)(bool);
		auto pFn = c_screen_with_menu_base_vtable_get_func_ptr<fnT>(40);
		(this->**pFn)(a2);
	}

	// member functions
	void __thiscall build_player_list(void* a1, int player_count);

	// static member functions
	static void applyPatches();

	// data
	// vtable doesn't need to be declared
	// void** vtbl;
	char data[4092];
};

static_assert(offsetof(c_screen_with_menu, data) == 0x4);