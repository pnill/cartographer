#pragma once
#include "kablam_command.h"

class kablam_command_statsfolder : public kablam_command
{
public:
	wchar_t folder_path[MAX_PATH];
	e_kablam_command_stats_folder_result_code result_code;

	void execute_rpc_command() override;

	static void print_help_text();
	static void parse_response(kablam_command* in_command);
	static kablam_command* create_instance(wchar_t** arguments, uint32 argument_count, class kablam_string* out_message);
};