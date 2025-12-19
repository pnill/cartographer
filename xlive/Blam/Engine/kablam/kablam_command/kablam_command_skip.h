#pragma once
#include "kablam_rpc/kablam_commands_h.h"
#include "kablam_command.h"

class kablam_command_skip : public kablam_command
{
public:
	kablam_command_skip_result result;

	void execute_rpc_command() override;
	static class kablam_command* create_instance(const wchar_t* const* arguments, uint32 argument_count, class kablam_string* out_message);
	static void print_help_text();
	static void parse_response(class kablam_command* command);
};

