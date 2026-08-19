#pragma once
#include "kablam_command.h"

/* classes */

class kablam_command_ban : public kablam_command
{
public:
	void execute_rpc_command(void) override;

	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_ban_ip : public kablam_command_ban
{
public:
	s_ipv4_subnet ipv4_subnet;
	int32 duration;
	e_kablam_command_ban_network_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_ban_nic : public kablam_command_ban
{
public:
	s_mac_address mac_address;
	int32 duration;
	e_kablam_command_ban_network_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_ban_gamer : public kablam_command_ban
{
public:
	wchar_t gamertag[16];
	int32 duration;
	e_kablam_command_gamer_table_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_ban_get_ip : public kablam_command_dynamic_array
{
public:
	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_ban_get_nic : public kablam_command_dynamic_array
{
public:
	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_ban_get_gamer : public kablam_command_dynamic_array
{
public:
	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};
