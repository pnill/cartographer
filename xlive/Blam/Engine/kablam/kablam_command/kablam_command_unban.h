#pragma once
#include "kablam_command.h"

/* classes */

class kablam_command_unban : public kablam_command
{
public:
	void execute_rpc_command(void) override;

	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_unban_ip : public kablam_command_unban
{
public:
	s_ipv4_subnet ipv4_subnet;
	e_kablam_command_ban_network_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_unban_nic : public kablam_command_unban
{
public:
	s_mac_address mac_address;
	e_kablam_command_ban_network_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_unban_gamer : public kablam_command_unban
{
public:
	wchar_t gamertag[16];
	e_kablam_command_gamer_table_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};

class kablam_command_unban_all : public kablam_command_unban
{
public:
	e_kablam_unban_all_type clear_type;
	e_kablam_command_unban_all_result_code result_code;

	void execute_rpc_command(void) override;

	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};
