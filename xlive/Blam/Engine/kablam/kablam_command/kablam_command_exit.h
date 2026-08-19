#pragma once
#include "kablam_command.h"

/* classes */

class kablam_command_exit : public kablam_command
{
public:
	void execute_rpc_command(void) override;

	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
};