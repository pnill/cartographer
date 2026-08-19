#pragma once
#include "kablam_command.h"

class kablam_command_vip : public kablam_command
{
public:
	void execute_rpc_command(void) override;

	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const *const *arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_vip_add : public kablam_command_vip
{
public:
	wchar_t gamertag[16];
	e_kablam_command_gamer_table_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const *const *arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_vip_remove : public kablam_command_vip
{
public:
	wchar_t gamertag[16];
	e_kablam_command_gamer_table_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const *const *arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_vip_get : public kablam_command_dynamic_array
{
public:
	void execute_rpc_command(void) override;

	static void parse_response(kablam_command * in_command);
	static kablam_command* create_instance(wchar_t const *const *arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_vip_clear : public kablam_command_vip
{
public:
	e_kablam_command_vip_clear_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const *const *arguments, uint32 argument_count, class kablam_string* out_message);
};
