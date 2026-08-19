#include "stdafx.h"
#include "kablam_command_live.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"
#include "kablam_shell.h"

#include "kablam_rpc/kablam_commands_h.h"

void kablam_command_live::execute_rpc_command(void)
{
	switch (type())
	{
	case _kablam_command_live_key:
		((kablam_command_live_key*)this)->execute_rpc_command();
		break;
	case _kablam_command_live_signin:
		((kablam_command_live_signin*)this)->execute_rpc_command();
		break;
	case _kablam_command_live_auto_signin:
		((kablam_command_live_auto_signin*)this)->execute_rpc_command();
		break;
	case _kablam_command_live_signout:
		((kablam_command_live_signout*)this)->execute_rpc_command();
		break;
	default:
		assert(false && "unknown command type reached");
		break;
	}

	return;
}

void kablam_command_live::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_live_desc, kablam_string_help_live_usage);
	return;
}

void kablam_command_live::parse_response(
	kablam_command* in_command)
{
	switch (in_command->type())
	{
	case _kablam_command_live_key:
		((kablam_command_live_key*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_live_signin:
		((kablam_command_live_signin*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_live_auto_signin:
		((kablam_command_live_auto_signin*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_live_signout:
		((kablam_command_live_signout*)in_command)->parse_response(in_command);
		break;
	default:
		assert(false && "unknown command type reached");
		break;
	}

	return;
}

kablam_command* kablam_command_live::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command* result = nullptr;

	out_message->free();

	if (g_instance_is_lan)
	{
		out_message->load(kablam_string_err_command_live_only);
	}
	else if (argument_count < 2)
	{
		out_message->load(kablam_string_err_missing_subcommand);
	}
	else
	{
		if (!_wcsicmp(L"key", arguments[1]))
		{
			result = kablam_command_live_key::create_instance(arguments, argument_count, out_message);
		}
		else if (!_wcsicmp(L"signin", arguments[1]))
		{
			result = kablam_command_live_signin::create_instance(arguments, argument_count, out_message);
		}

		else if (!_wcsicmp(L"autosignin", arguments[1]))
		{
			result = kablam_command_live_auto_signin::create_instance(arguments, argument_count, out_message);
		}
		else if (!_wcsicmp(L"signout", arguments[1]))
		{
			result = kablam_command_live_signout::create_instance(arguments, argument_count, out_message);
		}
		else
		{
			out_message->load(kablam_string_err_unknown_command);
		}
	}

	return result;
}

void kablam_command_live_key::execute_rpc_command(void)
{
	kablam_command_live_activate_rpc(this->live_key, &this->result_code);
}

void kablam_command_live_key::parse_response(
	kablam_command* in_command)
{
	kablam_command_live_key* command = (kablam_command_live_key*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case live_key_response_code_product_key_set:
		response_string_id = kablam_string_info_product_key_set;
		break;
	case live_key_response_code_key_invalid:
		response_string_id = kablam_string_err_product_key_invalid;
		break;
	case live_key_response_code_key_set_failed:
		response_string_id = kablam_string_err_product_key_set_failed;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
	return;
}

kablam_command* kablam_command_live_key::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command_live_key* result = nullptr;

	out_message->free();

	if (argument_count == 2)
	{
		out_message->load(kablam_string_err_missing_product_key);
	}
	else if (argument_count > 3)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else
	{
		result = new kablam_command_live_key();
		result->set_type(_kablam_command_live_key);
		result->set_valid(true);

		if (wcsncpy_s(result->live_key, NUMBEROF(result->live_key), arguments[2], _TRUNCATE))
		{
			result->result_code = live_key_response_code_product_key_set;
		}
		else
		{
			wchar_t const* live_key = result->live_key;
			const size_t length = wcslen(live_key);
			
			bool format_ok = (length == 29);

			if (length != 0)
			{
				for (unsigned int index = 0; index < length && format_ok; ++index)
				{
					unsigned int one_based = index + 1;
					wchar_t ch = live_key[index];

					if (one_based % 6 != 0)
						format_ok = (iswalnum(ch) != 0);
					else
						format_ok = (ch == L'-');
				}
			}

			result->result_code = format_ok ? live_key_response_code_product_key_set : live_key_response_code_key_invalid;
		}
	}
	return result;
}

void kablam_command_live_signin::execute_rpc_command(void)
{
	kablam_command_live_signin_rpc(this->username, this->password, &this->result_code);
}

void kablam_command_live_signin::parse_response(
	kablam_command* in_command)
{
	kablam_command_live_signin* command = (kablam_command_live_signin*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case live_signin_result_code_success:
		response_string_id = kablam_string_info_signing_in_manual;
		break;
	case live_signin_result_code_live_only:
		response_string_id = kablam_string_err_command_live_only;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
	return;
}

kablam_command* kablam_command_live_signin::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command_live_signin* result = nullptr;

	out_message->free();
	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_argument);
	}
	else if (argument_count > 3)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else
	{
		kablam_string_quick_wprintf(L"%ws\r\n\r\n", kablam_string_hint_live_autosignin);

		const HANDLE std_handle = GetStdHandle(STD_INPUT_HANDLE);
		
		DWORD std_console_mode = 0;
		GetConsoleMode(std_handle, &std_console_mode);
		SetConsoleMode(std_handle, (std_console_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) | ENABLE_LINE_INPUT);

		kablam_string_quick_wprintf(L"%ws", kablam_string_prompt_password);

		fflush(stdout);

		wchar_t password[64];
		wchar_t confirm_password[64];

		_getws_s(password, NUMBEROF(password));

		wprintf(L"\r\n");

		kablam_string_quick_wprintf(L"%ws", kablam_string_prompt_confirm_password);

		_getws_s(confirm_password, NUMBEROF(confirm_password));

		wprintf(L"\r\n\r\n");

		SetConsoleMode(std_handle, std_console_mode);

		if (wcscmp(password, confirm_password) != 0)
		{
			out_message->load(kablam_string_err_passwords_do_not_match);
		}
		else
		{
			result = new kablam_command_live_signin();

			result->set_type(_kablam_command_live_signin);
			result->set_valid(true);
			result->result_code = live_signin_result_code_success;

			wcsncpy_s(result->username, NUMBEROF(result->username), arguments[2], _TRUNCATE);
			wcsncpy_s(result->password, NUMBEROF(result->password), password, _TRUNCATE);
		}
	}

	return result;
}

void kablam_command_live_auto_signin::execute_rpc_command(void)
{
	kablam_command_live_auto_signin_rpc(this->username, this->password, &this->result_code, (uint32*)&this->xlive_login_result);
	return;
}

void kablam_command_live_auto_signin::parse_response(
	kablam_command* in_command)
{
	kablam_command_live_auto_signin* command = (kablam_command_live_auto_signin*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case live_auto_signin_response_code_auto_signin_enabled:
		response_string_id = kablam_string_info_autosignin_enabled;
		break;
	case live_auto_signin_response_code_auto_signin_failed:
		response_string_id = kablam_string_err_autosignin_failed;
		break;
	case live_auto_signin_response_code_live_only:
		response_string_id = kablam_string_err_command_live_only;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);

	if (command->xlive_login_result < XLIVE_S_OK)
	{
		wprintf(L" (0x%08x)", command->xlive_login_result);
	}

	return;
}

kablam_command* kablam_command_live_auto_signin::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command_live_auto_signin* result = nullptr;

	out_message->free();

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_argument);
	}
	else if (argument_count > 3)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else
	{
		kablam_string_quick_wprintf(L"%ws\r\n\r\n", kablam_string_hint_live_autosignin);

		const HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);

		assert(stdin_handle != INVALID_HANDLE_VALUE);

		DWORD std_console_mode = 0;
		GetConsoleMode(stdin_handle, &std_console_mode);
		SetConsoleMode(stdin_handle, (std_console_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) | ENABLE_LINE_INPUT);

		kablam_string_quick_wprintf(L"%ws", kablam_string_prompt_password);

		fflush(stdout);

		wchar_t password[64];
		wchar_t confirm_password[64];

		_getws_s(password, NUMBEROF(password));

		wprintf(L"\r\n");

		kablam_string_quick_wprintf(L"%ws", kablam_string_prompt_confirm_password);

		_getws_s(confirm_password, NUMBEROF(confirm_password));

		wprintf(L"\r\n\r\n");

		SetConsoleMode(stdin_handle, std_console_mode);

		if (wcscmp(password, confirm_password) != 0)
		{
			out_message->load(kablam_string_err_passwords_do_not_match);
		}
		else
		{
			result = new kablam_command_live_auto_signin();

			result->set_type(_kablam_command_live_signin);
			result->set_valid(true);
			result->result_code = live_auto_signin_response_code_auto_signin_enabled;
			result->xlive_login_result = XLIVE_S_OK;

			wcsncpy_s(result->username, NUMBEROF(result->username), arguments[2], _TRUNCATE);
			wcsncpy_s(result->password, NUMBEROF(result->password), password, _TRUNCATE);
		}
	}
	
	return result;
}

void kablam_command_live_signout::execute_rpc_command(void)
{
	kablam_command_live_signout_rpc(&this->result_code);
	return;
}

void kablam_command_live_signout::parse_response(
	kablam_command* in_command)
{
	kablam_command_live_signout* command = (kablam_command_live_signout*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case live_signout_result_code_signing_out_live:
		response_string_id = kablam_string_info_signing_out_live;
		break;
	case live_signout_result_code_not_signed_in:
		response_string_id = kablam_string_live_not_signed_in;
		break;
	case live_signout_result_code_live_only:
		response_string_id = kablam_string_err_command_live_only;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
	return;
}

kablam_command* kablam_command_live_signout::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	UNREFERENCED_PARAMETER(arguments);
	UNREFERENCED_PARAMETER(argument_count);
	UNREFERENCED_PARAMETER(out_message);
	return nullptr;
}
