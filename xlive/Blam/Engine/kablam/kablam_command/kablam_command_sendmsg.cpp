#include "stdafx.h"
#include "kablam_command_sendmsg.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

void kablam_command_sendmsg::execute_rpc_command()
{
	kablam_command_send_message_rpc(this->message, &this->result_code);
}

void kablam_command_sendmsg::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_sendmsg_desc, kablam_string_help_sendmsg_usage);
}

void kablam_command_sendmsg::parse_response(kablam_command* in_command)
{
	kablam_command_sendmsg* command = (kablam_command_sendmsg*)in_command;

	int32 response_string_id = 0;
	switch (command->result_code)
	{
	case _send_message_result_code_sent:
		response_string_id = kablam_string_info_message_sent;
		break;
	case _send_message_result_code_server_not_active:
		response_string_id = kablam_string_status_server_not_active;
		break;
	case _send_message_result_code_no_players_in_session:
		response_string_id = kablam_string_info_no_players_in_session;
		break;
	case _send_message_result_code_message_failed:
		response_string_id = kablam_string_err_send_message_failed;
		break;
	}

	kablam_string_quick_wprintf(L"%s", response_string_id);
}

kablam_command* kablam_command_sendmsg::create_instance(const wchar_t* const* arguments, uint32 argument_count, kablam_string* out_message)
{
	kablam_command_sendmsg* result = nullptr;

	out_message->free();

	if (argument_count < 2)
	{
		out_message->load(kablam_string_err_missing_message);
	}
	else if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args_quotes_hint);
	}
	else if (wcsnlen(arguments[1], 121) == 121)
	{
		out_message->load(kablam_string_err_text_too_long);
	}
	else
	{
		result = new kablam_command_sendmsg();

		result->type = _kablam_command_send_message;
		result->valid = true;
		result->result_code = _send_message_result_code_sent;
		wcsncpy_s(result->message, NUMBEROF(result->message) - 1, arguments[1], _TRUNCATE);
	}
	
	return result;
}
