#pragma once

#pragma once

#include "CustomMenuGlobals.h"
#include "c_brightness_menu.h"

#define BRIGHTNESS_MENU_ID 272

template<typename T>
static T c_brightness_menu_base_vtable_get_func_ptr(DWORD idx)
{
	return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x7D7ABC)[idx]);
}

class c_brightness_menu : public c_screen_with_menu
{
public:
	c_brightness_menu::c_brightness_menu(int a3, int a4, int a5);

	c_brightness_menu::~c_brightness_menu()
	{
	}

	// interface
	virtual int custom_deconstructor(int a1) override
	{
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<int(c_brightness_menu::**)(int)>(0);
		return (this->**pFn)(a1);
	};

	virtual int sub_611E23() override
	{
		// typedef int(c_brightness_menu::**fnT)(); works
		// WHILE FUCKING decltype(sub_611E23) doesn't
		// even Intellisense says the same fucking thing
		// aids compiler
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(1);
		return (this->**pFn)();
	}

	virtual void sub_611488() override
	{
		typedef void(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(2);
		return (this->**pFn)();
	}

	virtual int sub_612CD8() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(3);
		return (this->**pFn)();
	}

	virtual void nullsub_213(int a2) override
	{
		typedef void(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(4);
		(this->**pFn)(a2);
	}

	virtual DWORD sub_6114E0(DWORD* a2) override
	{
		typedef DWORD(c_brightness_menu::** fnT)(DWORD*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(5);
		return (this->**pFn)(a2);
	}

	virtual int sub_611692(int a2) override
	{
		typedef int(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(6);
		return (this->**pFn)(a2);
	}

	virtual int sub_6116BD() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(7);
		return (this->**pFn)();
	}

	virtual DWORD* sub_611703(DWORD* a2) override
	{
		typedef DWORD* (c_brightness_menu::** fnT)(DWORD*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(8);
		return (this->**pFn)(a2);
	}

	virtual void sub_612A7C() override
	{
		typedef void(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(9);
		return (this->**pFn)();
	}

	virtual int sub_612ABC() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(10);
		return (this->**pFn)();
	}

	virtual int sub_612BCA() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(11);
		return (this->**pFn)();
	}

	virtual int sub_60EEBE(DWORD* a2) override
	{
		typedef int(c_brightness_menu::** fnT)(DWORD*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(12);
		return (this->**pFn)(a2);
	}

	virtual int sub_6120F8() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(13);
		return (this->**pFn)();
	}

	virtual int sub_612120() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(14);
		return (this->**pFn)();
	}

	virtual void nullsub_227(int a1) override
	{
		typedef void(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(15);
		return (this->**pFn)(a1);
	}

	virtual void nullsub_228(int a1) override
	{
		typedef void(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(16);
		return (this->**pFn)(a1);
	}

	virtual int sub_60EF08(int a2) override
	{
		typedef int(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(17);
		return (this->**pFn)(a2);
	}

	virtual bool sub_40AD4D() override
	{
		typedef bool(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(18);
		return (this->**pFn)();
	}

	virtual int sub_40AD50() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(19);
		return (this->**pFn)();
	}

	virtual bool sub_60E885() override
	{
		typedef bool(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(20);
		return (this->**pFn)();
	}

	virtual void nullsub_138() override
	{
		typedef void(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(21);
		return (this->**pFn)();
	}

	virtual int sub_6111AB(int a2) override
	{
		typedef int(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(22);
		return (this->**pFn)(a2);
	}

	virtual int sub_60F790() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(23);
		return (this->**pFn)();
	}

	virtual BYTE* sub_610B4E() override
	{
		typedef BYTE* (c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(24);
		return (this->**pFn)();
	}

	virtual int sub_6102C5() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(25);
		return (this->**pFn)();
	}

	virtual int sub_6103D6() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(26);
		return (this->**pFn)();
	}

	virtual int sub_60F1F4(int a2) override
	{
		typedef int(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(27);
		return (this->**pFn)(a2);
	}

	virtual bool sub_60EFC1(int* a2) override
	{
		typedef bool(c_brightness_menu::** fnT)(int*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(28);
		return (this->**pFn)(a2);
	}

	virtual bool sub_60F081(int a2) override
	{
		typedef bool(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(29);
		return (this->**pFn)(a2);
	}

	virtual int sub_60F151(int a2) override
	{
		typedef int(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(30);
		return (this->**pFn)(a2);
	}

	virtual bool sub_40AD53(int a2) override
	{
		typedef bool(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(31);
		return (this->**pFn)(a2);
	}

	virtual int sub_60EB8A() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(32);
		return (this->**pFn)();
	}

	virtual int sub_60EB8E() override
	{
		typedef int(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(33);
		return (this->**pFn)();
	}

	virtual unsigned int sub_60EB92(int a2) override
	{
		typedef unsigned int(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(34);
		return (this->**pFn)(a2);
	}

	virtual void nullsub_139(int a2) override
	{
		typedef void(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(35);
		return (this->**pFn)(a2);
	}

	virtual int sub_60EC5C(DWORD* a2) override
	{
		typedef int(c_brightness_menu::** fnT)(DWORD*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(36);
		return (this->**pFn)(a2);
	}

	virtual int sub_60ECC9(int a2) override
	{
		typedef int(c_brightness_menu::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(37);
		return (this->**pFn)(a2);
	}

	virtual void sub_688255() override
	{
		typedef void(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(38);
		(this->**pFn)();
	}

	virtual bool sub_40AD58() override
	{
		typedef bool(c_brightness_menu::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(39);
		return (this->**pFn)();
	}

	virtual void sub_60F2A4(bool a2) override
	{
		typedef void(c_brightness_menu::** fnT)(bool);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(40);
		(this->**pFn)(a2);
	}

	static void* open_brightness_menu(s_menu_input_unk_data* a1);
};

void replace_brightness_menu();
