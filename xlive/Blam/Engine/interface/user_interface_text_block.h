#pragma once
#include "user_interface_text.h"
#include "cseries/cseries_strings.h"


/* classes */

class c_small_user_interface_text : public c_user_interface_text
{
protected:
	c_static_wchar_string32 string;

public:
	c_small_user_interface_text();

	// c_small_user_interface_text virtual functions

	virtual ~c_small_user_interface_text()
	{
	}
	virtual void  set_text(const wchar_t* text) override;
	virtual void  append_text(const wchar_t* text) override;
	virtual const wchar_t* get_raw_string() override;
};
ASSERT_STRUCT_SIZE(c_small_user_interface_text, 0x84);

class c_normal_user_interface_text : public c_user_interface_text
{
protected:
	c_static_wchar_string<512> string;

public:
	c_normal_user_interface_text();

	// c_normal_user_interface_text virtual functions

	virtual ~c_normal_user_interface_text()
	{
	}
	virtual void  set_text(const wchar_t* raw_text) override;
	virtual void  append_text(const wchar_t* Source) override;
	virtual const wchar_t* get_raw_string() override;
};
ASSERT_STRUCT_SIZE(c_normal_user_interface_text, 0x444);

class c_long_user_interface_text : public c_user_interface_text
{
protected:
	c_static_wchar_string<1024> string;

public:
	c_long_user_interface_text();

	// c_normal_user_interface_text virtual functions

	virtual ~c_long_user_interface_text() 
	{
	}
	virtual void  set_text(const wchar_t* raw_text) override;
	virtual void  append_text(const wchar_t* Source) override;
	virtual const wchar_t* get_raw_string() override;
};
ASSERT_STRUCT_SIZE(c_long_user_interface_text, 0x844);