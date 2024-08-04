#include "stdafx.h"

#include "user_interface_text_block.h"

c_small_user_interface_text::c_small_user_interface_text()
{
	string.clear();
}

c_user_interface_text* c_small_user_interface_text::destructor(uint32 flags)
{
	this->~c_small_user_interface_text();
	if (TEST_BIT(flags, 0))
	{
	}

	return this;
}

void  c_small_user_interface_text::set_text(const wchar_t* raw_text)
{
	INVOKE_TYPE(0x20F425, 0x0, void(__thiscall*)(c_small_user_interface_text*, const wchar_t*), this, raw_text);
}
void c_small_user_interface_text::append_text(const wchar_t* Source)
{
	INVOKE_TYPE(0x20F462, 0x0, void(__thiscall*)(c_small_user_interface_text*, const wchar_t*), this, Source);
}
const wchar_t* c_small_user_interface_text::get_raw_string()
{
	return INVOKE_TYPE(0x20F4A7, 0x0, const wchar_t*(__thiscall*)(c_small_user_interface_text*), this);
}




c_normal_user_interface_text::c_normal_user_interface_text()
{
	string.clear();
}

c_user_interface_text* c_normal_user_interface_text::destructor(uint32 flags)
{
	this->~c_normal_user_interface_text();
	if (TEST_BIT(flags, 0))
	{
	}

	return this;
}

void  c_normal_user_interface_text::set_text(const wchar_t* raw_text)
{
	INVOKE_TYPE(0x20F4C9, 0x0, void(__thiscall*)(c_normal_user_interface_text*, const wchar_t*), this, raw_text);
}
void c_normal_user_interface_text::append_text(const wchar_t* Source)
{
	INVOKE_TYPE(0x20F509, 0x0, void(__thiscall*)(c_normal_user_interface_text*, const wchar_t*), this, Source);
}
const wchar_t* c_normal_user_interface_text::get_raw_string()
{
	return INVOKE_TYPE(0x20F554, 0x0, const wchar_t* (__thiscall*)(c_normal_user_interface_text*), this);
}




c_long_user_interface_text::c_long_user_interface_text()
{
	string.clear();
}

c_user_interface_text* c_long_user_interface_text::destructor(uint32 flags)
{
	this->~c_long_user_interface_text();
	if (TEST_BIT(flags, 0))
	{
	}

	return this;
}

void c_long_user_interface_text::set_text(const wchar_t* raw_text)
{
	INVOKE_TYPE(0x243DDB, 0x0, void(__thiscall*)(c_long_user_interface_text*, const wchar_t*), this, raw_text);
}
void c_long_user_interface_text::append_text(const wchar_t* Source)
{
	INVOKE_TYPE(0x243E1B, 0x0, void(__thiscall*)(c_long_user_interface_text*, const wchar_t*), this, Source);
}
const wchar_t* c_long_user_interface_text::get_raw_string()
{
	return INVOKE_TYPE(0x243E66, 0x0, const wchar_t* (__thiscall*)(c_long_user_interface_text*), this);
}

