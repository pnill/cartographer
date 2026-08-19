#include "stdafx.h"
#include "kablam_command_privacy.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

/* constants */

static wchar_t const* const k_kablam_command_privacy_mode_strings[k_kablam_command_privacy_type_count]
{
	L"open",
	L"gold",
	L"vip"
};

/* public code */

void kablam_command_privacy::execute_rpc_command(void)
{
	if (type() == _kablam_command_privacy_get)
	{
		kablam_command_get_privacy_rpc(&this->privacy_mode, &this->result_code);
	}
	else if (type() == _kablam_command_privacy_set)
	{
		kablam_command_set_privacy_rpc(this->privacy_mode, &this->result_code);
	}

	return;
}

void kablam_command_privacy::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_privacy_desc, kablam_string_help_privacy_usage);
	return;
}

void kablam_command_privacy::parse_response(
	kablam_command* in_command)
{
	kablam_command_privacy* command = (kablam_command_privacy*)in_command;

	if (command->type() == _kablam_command_privacy_get)
	{

		int32 response_string_id = 0;
		if (command->result_code == privacy_result_code_live_only)
		{
			response_string_id = kablam_string_err_command_live_only;
		}
		else
		{
			switch (command->privacy_mode)
			{
			case kablam_command_privacy_open:
				response_string_id = kablam_string_privacy_open_label;
				break;
			case kablam_command_privacy_gold:
				response_string_id = kablam_string_privacy_gold_label;
				break;
			case kablam_command_privacy_vip:
				response_string_id = kablam_string_privacy_vip_label;
				break;
			default:
				break;
			}
		}

		kablam_string_quick_wprintf(L"%ws", response_string_id);
	}

	if (command->type() == _kablam_command_privacy_set)
	{
		kablam_string response_string;
		response_string.load(command->result_code == privacy_result_code_live_only ? kablam_string_err_command_live_only : kablam_string_info_privacy_changed);

		wprintf(L"%ws", response_string.get());

		response_string.free();
	}

	return;
}

kablam_command* kablam_command_privacy::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command_privacy* result = nullptr;

	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else
	{
		result = new kablam_command_privacy();
		result->set_valid(true);

		if (argument_count == 1)
		{
			result->set_type(_kablam_command_privacy_get);
			result->privacy_mode = _kablam_command_privacy_none;
			result->result_code = privacy_result_code_success;
		}
		else
		{
			result->set_type(_kablam_command_privacy_set);
			result->privacy_mode = _kablam_command_privacy_none;
			result->result_code = privacy_result_code_success;

			for (int32 i = 0; i < k_kablam_command_privacy_type_count; ++i)
			{
				if (_wcsicmp(arguments[1], k_kablam_command_privacy_mode_strings[i]))
				{
					result->privacy_mode = (e_kablam_command_privacy_type)i;
					break;
				}
			}

			if (result->privacy_mode == _kablam_command_privacy_none)
			{
				out_message->load(kablam_string_err_privacy_mode_invalid);

				delete result;
				result = nullptr;
			}
		}
	}

	return result;
}