#pragma once
#include "kablam_command.h"
#include "kablam_rpc/kablam_commands_h.h"

/* classes */

class kablam_command_name : public kablam_command
{
public:
	wchar_t name[16];
	e_kablam_command_set_unicode_setting_result_code result_code;

	void execute_rpc_command(void) override;
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
};

