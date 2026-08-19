#pragma once
#include "kablam_command.h"
#include "kablam_rpc/kablam_commands_h.h"

/* classes */

class kablam_command_status : public kablam_command
{
	kablam_command_status_result response;
public:
	void execute_rpc_command(void) override;

	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};
