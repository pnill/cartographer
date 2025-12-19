#pragma once
#include "kablam_rpc/kablam_commands_h.h"
#include "kablam_command.h"

class kablam_command_kick : public kablam_command
{
public:

	wchar_t gamertag[32];
	kablam_command_kick_result_code result_code;

	void execute_rpc_command() override;
	static kablam_command* create_instance(const wchar_t* const* arguments, uint32 argument_count, class kablam_string* out_message);
	static void print_help_text();
	static void parse_response(kablam_command* in_command);
};

