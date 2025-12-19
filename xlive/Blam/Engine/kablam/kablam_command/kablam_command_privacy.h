#pragma once
#include "kablam_command.h"

class kablam_command_privacy : public kablam_command
{
public:
	e_kablam_command_privacy_type privacy_mode;
	e_kablam_command_privacy_result_code result_code;

	void execute_rpc_command() override;

	static void print_help_text();
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t** arguments, uint32 argument_count, class kablam_string* out_message);
};