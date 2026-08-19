#pragma once
#include "kablam_command.h"
#include "xlive_result_codes.h"

/* classes */

class kablam_command_live : public kablam_command
{
public:
	void execute_rpc_command(void) override;

	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_live_key : public kablam_command_live
{
public:
	wchar_t live_key[30];
	e_kablam_command_live_key_response_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_live_signin : public kablam_command_live
{
public:
	wchar_t username[256];
	wchar_t password[64];
	e_kablam_command_live_signin_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_live_auto_signin : public kablam_command_live
{
public:
	wchar_t username[256];
	wchar_t password[64];
	e_kablam_command_live_auto_signin_response_code result_code;
	XLIVE_HResult xlive_login_result;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_live_signout : public kablam_command_live
{
public:
	e_kablam_command_live_signout_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};
