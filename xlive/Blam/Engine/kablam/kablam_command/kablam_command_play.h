#pragma once
#include "kablam_command.h"
#include "kablam_rpc/kablam_commands_h.h"

/* classes */

class kablam_command_play : public kablam_command
{
public:
	wchar_t file_path[256];
	kablam_command_play_result response;

	void execute_rpc_command(void) override;

	static void print_help_text(void);
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t const* const* arguments, uint32 argument_count, class kablam_string* out_message);
};
