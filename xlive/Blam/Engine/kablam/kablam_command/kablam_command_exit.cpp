#include "stdafx.h"
#include "kablam_command_exit.h"

#include "kablam_command_util.h"

#include "kablam_rpc/kablam_rpc_client.h"

/* public code*/

void kablam_command_exit::execute_rpc_command(void)
{
	kablam_rpc_client* client = kablam_rpc_client_get();

	if (client)
	{
		client->disconnect_local();
		kablam_rpc_client_free();

		exit(0);
	}

	return;
}

kablam_command* kablam_command_exit::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	UNREFERENCED_PARAMETER(arguments);
	UNREFERENCED_PARAMETER(argument_count);
	UNREFERENCED_PARAMETER(out_message);

	kablam_command_exit* instance = new kablam_command_exit();
	instance->set_valid(true);
	instance->set_type(_kablam_command_exit);
	return instance;
}

void kablam_command_exit::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_exit_desc, kablam_string_help_exit_usage);
	return;
}

void kablam_command_exit::parse_response(
	kablam_command* in_command)
{
	UNREFERENCED_PARAMETER(in_command);
	wprintf(L"How in the world did you get this far, you are not supposed to be here!");
	return;
}
