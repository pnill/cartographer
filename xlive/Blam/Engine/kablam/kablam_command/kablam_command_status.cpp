#include "stdafx.h"
#include "kablam_command_status.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

#include "xlive_result_codes.h"

/* constants */

static wchar_t const* const k_status_result_code_strings[k_status_result_code_count]
{
	L"offline - initializing",
	L"offline - product key required",
	L"offline - invalid product key",
	L"offline - server banned",
	L"offline - update required",
	L"offline",
	L"signing in",
	L"signing out",
	L"reconnecting",
	L"offline - waiting to reconnect",
	L"offline - sign-in failed",
	L"offline - disconnected",
	L"offline - no playlist",
	L"online - failed to load playlist",
	L"online - empty playlist",
	L"online - pregame lobby",
	L"online - starting game",
	L"online - game in progress",
	L"online - game over",
	L"online - postgame carnage report",
};

/* public code */

void kablam_command_status::execute_rpc_command(void)
{
	kablam_command_status_rpc(&this->response);
	return;
}

void kablam_command_status::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_status_desc, kablam_string_help_status_usage);
	return;
}

void kablam_command_status::parse_response(
	kablam_command* in_command)
{
	kablam_command_status* command = (kablam_command_status*)in_command;

	const bool is_live = TEST_BIT(command->response.flags, status_response_flag_live);

	wprintf(L"Network mode: %ws\r\n", is_live ? L"LIVE" : L"LAN");

	wprintf(L"Current state: %ws\r\n", (IN_RANGE(command->response.result_code, 0, k_status_result_code_count)) ? k_status_result_code_strings[command->response.result_code] : L"");


	if (is_live)
	{
		int32 xlive_result_string_id = 0;

		if (TEST_BIT(command->response.flags, status_response_flag_xlive_signin_result) && 
			IN_RANGE(command->response.result_code, status_result_code_connecting, status_result_code_offline_signin_failed))
		{
			XLIVE_HResult xlive_result = (XLIVE_HResult)command->response.xlive_signin_result_code;

			switch (xlive_result)
			{
			case XLIVE_E_INVALIDARG:
				xlive_result_string_id = kablam_string_err_live_invalid_credentials;
				break;

			case XONLINE_E_LOGON_NO_NETWORK_CONNECTION:
			case XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE:
				xlive_result_string_id = kablam_string_err_live_service_unreachable;
				break;

			case XONLINE_E_LOGON_MACS_TIMEDOUT:
				xlive_result_string_id = kablam_string_err_server_product_key_missing;
				break;
			case XONLINE_E_LOGON_PPLOGIN_FAILED:
				xlive_result_string_id = kablam_string_err_live_invalid_credentials;
				break;

			case XONLINE_E_LOGON_SPONSOR_TOKEN_INVALID:
				xlive_result_string_id = kablam_string_err_server_product_key_invalid;
				break;

			case XONLINE_E_LOGON_SPONSOR_TOKEN_BANNED:
				xlive_result_string_id = kablam_string_err_live_server_banned;
				break;

			case XONLINE_E_LOGON_SPONSOR_TOKEN_USAGE_EXCEEDED:
				xlive_result_string_id = kablam_string_err_live_server_limit_exceeded;
				break;

			case XONLINE_E_LOGON_FLASH_UPDATE_NOT_DOWNLOADED:
			case XONLINE_E_LOGON_UPDATE_NOT_DOWNLOADED:
				xlive_result_string_id = kablam_string_err_live_update_required;
				break;

			case XONLINE_E_LOGON_SERVICE_NOT_REQUESTED:
			case XONLINE_E_LOGON_SERVICE_NOT_AUTHORIZED:
			case XONLINE_E_LOGON_SERVICE_TEMPORARILY_UNAVAILABLE:
			case XONLINE_E_LOGON_SERVERS_TOO_BUSY:
				xlive_result_string_id = kablam_string_err_live_unavailable;
				break;

			case XONLINE_E_LOGON_CONNECTION_LOST:
				xlive_result_string_id = kablam_string_err_live_service_unreachable;
				break;

			case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
				xlive_result_string_id = kablam_string_err_live_signed_in_elsewhere;
				break;

			case XONLINE_E_LOGON_INVALID_USER:
				xlive_result_string_id = kablam_string_err_live_invalid_id;
				break;

			default:
				break;
			}
		}

		if (xlive_result_string_id)
		{
			kablam_string_quick_wprintf(L"Reason: %ws\r\n", xlive_result_string_id);
		}

		if (TEST_BIT(command->response.flags, status_response_flag_xlive_signin_result))
		{
			wprintf(L"Sign-in: %ws (%ws)\r\n",
				command->response.live_id,
				TEST_BIT(command->response.flags, status_response_flag_signin_mode) ? L"Automatic" : L"Manual");

			if (TEST_BIT(command->response.flags, status_response_flag_has_gamertag))
			{
				wprintf(L"Name: %hs\r\n", command->response.gamer_tag);
			}
		}

		if (wcscmp(command->response.server_description, L""))
		{
			wprintf(L"Description: %ws\r\n", command->response.server_description);
		}
	}
	else // LAN
	{
		if (TEST_BIT(command->response.flags, status_response_flag_has_lan_name))
		{
			wprintf(L"Name: %ws\r\n", command->response.lan_server_name);
		}
	}

	if (TEST_BIT(command->response.flags, status_response_flag_has_playlist_path) &&
		wcscmp(command->response.playlist_path, L""))
	{
		wprintf(L"Playlist: %ws\r\n", command->response.playlist_path);
	}

	if (wcscmp(command->response.custom_map_path, L""))
	{
		wprintf(L"Custom map folder: %ws\r\n", command->response.custom_map_path);
	}

	if (TEST_BIT(command->response.flags, status_response_flag_lobby_info))
	{
		wprintf(L"\r\n");

		if (wcscmp(command->response.playing_variant, L"") && wcscmp(command->response.playing_map, L""))
		{
			wprintf(L"Playing: %ws on %ws.\r\n", command->response.playing_variant, command->response.playing_map);
		}

		if (command->response.result_code == status_result_code_online_game_in_progress)
		{
			int32 time_in_game_seconds = command->response.game_time_in_seconds;
			int32 hours = time_in_game_seconds / 3600;
			int32 minutes = time_in_game_seconds % 3600 / 60;
			int32 seconds = time_in_game_seconds - 60 * (minutes + 60 * hours);

			wchar_t const* prefix = (command->response.game_time_remaining <= 0) ? L"Time remaining:" : L"Time elapsed:";

			wprintf(L"%ws %02d:%02d:%02d\r\n", prefix, hours, minutes, seconds);
		}

		if (wcscmp(command->response.next_variant, L"") && wcscmp(command->response.next_map, L""))
		{
			wprintf(L"Next: %ws on %ws.\r\n", command->response.next_variant, command->response.next_map);
		}

		if (command->response.player_count)
		{
			wprintf(L"Players:\r\n");

			for (int16 i = 0; i < command->response.player_count; ++i)
			{
				wprintf(L"\r\n\t%ws", command->response.player_names[i]);
			}
		}
		else
		{
			wprintf(L"No players present.\r\n");
		}
		
	}
	switch (command->response.result_code)
	{
	case status_result_code_offline_product_key_required:
	case status_result_code_offline_invalid_product_key:
		wprintf(L"\r\nUse the \"live key\" command to set your server's product key.");
		break;

	case status_result_code_offline:
		wprintf(L"\r\nUse the \"live autosignin\" command to sign your server in to LIVE.");
		break;

	case status_result_code_offline_signin_failed:
		wprintf(L"\r\nUse \"live autosignin\" or \"live signin\" to attempt to sign in again.");
		break;

	case status_result_code_offline_disconnected:
		wprintf(
			L"\r\n"
			"Check your Internet connection, and make sure this account is not being\r\n"
			"used for automatic signin by another server.");
		break;

	case status_result_code_online_no_playlist:
	case status_result_code_online_failed_to_load_playlist:
	case status_result_code_online_empty_playlist:
		wprintf(
			L"\r\n"
			"Use the \"play\" command to set your server's playlist.\r\n"
			"Use the \"playing\" command to view playlist errors and warnings.");
		break;

	default:
		unreachable();
		break;
	}

	return;
}

kablam_command* kablam_command_status::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	UNREFERENCED_PARAMETER(arguments);
	UNREFERENCED_PARAMETER(argument_count);

	kablam_command_status* result = nullptr;

	out_message->free();

	if (argument_count > 1)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else
	{
		result = new kablam_command_status();
		result->set_type(_kablam_command_status);
		result->set_valid(true);
		memset(&result->response, 0, sizeof(kablam_command_status_result));
	}

	return result;
}
