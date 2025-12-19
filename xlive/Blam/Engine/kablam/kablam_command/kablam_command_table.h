#pragma once
#include "kablam_command.h"

enum 
{
	k_kablam_command_table_count = 16
};

class kablam_command_definition
{
public:
	const wchar_t* command;
	class kablam_command* (*command_constructor)(wchar_t** arguments, uint32 arugument_count, class kablam_string* out_message);
	void (*help_text_function)();
	void (*parse_response_function)(class kablam_command* in_command);
};


class kablam_command* kablam_command_table_create_command(wchar_t** arguments, uint32 argument_count, bool print_messages, uint32* out_command_table_index);
void kablam_command_table_print_help_function(e_kablam_command_type command_type);

void kablam_command_table_parse_input(wchar_t* command_buffer, wchar_t** command_arguments, uint32 max_argument_count, uint32* command_argument_count);
void kablam_command_table_parse_response(class kablam_command* command, uint32 command_table_index);