#include "stdafx.h"
#include "kablam_command_util.h"

#include "kablam_rpc/kablam_commands_h.h"

#include "kablam_strings.h"

/* constants */

static const int32 k_play_string_labels[k_play_warning_count]
{
	kablam_string_err_playlist_section_dup,
	kablam_string_err_variant_name_duplicate,
	kablam_string_err_unknown_setting,
	kablam_string_err_unknown_section,
	kablam_string_err_invalid_value_for_setting,
	kablam_string_err_already_set,
	kablam_string_err_match_missing_setting,
	kablam_string_err_invalid_setting_for_variant,
	kablam_string_err_variant_missing_type_or_base,
	kablam_string_err_variant_missing_name,
	kablam_string_err_variant_invalid_utf16,
	kablam_string_err_variant_pipe_forbidden,
	kablam_string_err_invalid_base_variant,
	kablam_string_err_variant_both_type_and_base,
	kablam_string_err_variant_missing_or_invalid,
	kablam_string_err_map_missing_or_invalid,
	kablam_string_warn_variants_overflow,
	kablam_string_warn_matches_overflow
};

/* prototypes */

static int32 hex_digit_value_from_wchar(wchar_t ch);

static bool kablam_set_ipv4_octet(uint32* ip_value, uint32* octet_index, uint32 value);

/* public code */

void kablam_command_print_help_text(
	int32 description_string_id,
	int32 usage_string_id)
{
	kablam_string label_description;
	kablam_string command_description;
	kablam_string label_usage;
	kablam_string command_usage;

	label_description.load(kablam_string_description_label);
	command_description.load(description_string_id);

	label_usage.load(kablam_string_usage_label);
	command_usage.load(usage_string_id);

	wprintf(L"%s\r\n", label_description.get());
	wprintf(L"%s\r\n\r\n", command_description.get());

	wprintf(L"%s\r\n", label_usage.get());
	wprintf(L"%s", command_usage.get());

	label_description.free();
	command_description.free();
	command_usage.free();
	label_usage.free();
	return;
}

void kablam_command_print_playlist_warning(
	kablam_command_playlist_warning* warning)
{
	const bool valid_warning = VALID_INDEX(warning->warning_type, k_play_warning_count);
	kablam_string label;

	if (valid_warning)
	{
		label.load(k_play_string_labels[warning->warning_type]);

		switch (warning->warning_type)
		{
		case play_warning_duplicate_variant_name:
		case play_warning_error_unknown_setting:
		case play_warning_error_unknown_section:
		case play_warning_error_match_missing_setting:
			wprintf(L"%ws (\"%ws\")", label.get(), warning->parameter);
			break;
		case play_warning_error_invalid_setting_value:
			wprintf(label.get(), warning->parameter, warning->value);
			break;
		case play_warning_error_setting_value_already_set:
			wprintf(label.get(), warning->parameter);
			break;
		case play_warning_error_invalid_variant_setting:
			_wprintf_p(label.get(), warning->parameter, warning->value);
			break;
		case play_warning_error_variant_invalid_base_variant:
			_wprintf_p(label.get(), warning->parameter);
			break;
		case play_warning_error_variant_missing_type_or_base:
		case play_warning_error_variant_missing_name:
		case play_warning_error_variant_invalid_utf16:
		case play_warning_error_fariant_pipe_forbidden:
		case play_warning_error_variant_both_type_and_base:
		case play_warning_error_variant_missing_or_invalid:
		case play_warning_error_map_missing_or_invalid:
		case play_warning_warn_variants_overflow:
		case play_warning_warn_matches_overflow:
		case play_warning_duplicate_playlist_section:
			wprintf(L"%ws", label.get());
			break;
		default:
			unreachable();
			break;
		}

		wprintf(L"\r\n");
		label.free();
	}

	return;
}

bool kablam_command_parse_duration_string(
	wchar_t const* duration_string,
	int32* out_duration)
{
	enum parse_state
	{
		state_initial_or_ws = 0, // skipping whitespace, waiting for a number or end
		state_reading_number,    // collecting digits
		state_expect_unit,       // have a number, now need D/H/M/S (optionally with ws in between)
		state_error,
		state_done
	};

	int32  current_value = 0;
	int32  days = -1;
	int32  hours = -1;
	int32  minutes = -1;
	int32  seconds = -1;

	parse_state state = state_initial_or_ws;
	wchar_t const* p = duration_string;

	while (state != state_done && state != state_error)
	{
		wchar_t ch = *p;

		switch (state)
		{
			case state_initial_or_ws:
				while (ch == L'\t' || ch == L' ')
				{
					++p;
					ch = *p;
				}

				if (ch == L'\0')
				{
					state = state_done;
				}
				else if (iswdigit(ch))
				{
					state = state_reading_number;
					current_value = 0;
					continue;
				}
				else
				{
					state = state_error;
				}
				break;

			case state_reading_number:
				if (iswdigit(ch))
				{
					int32 digit = ch - L'0';
					current_value = current_value * 10 + digit;
					if (current_value > 999)
					{
						state = state_error;
					}
					else
					{
						++p;
					}
				}
				else
				{
					state = state_expect_unit;
				}
				break;

			case state_expect_unit:
				while (ch == L'\t' || ch == L' ')
				{
					++p;
					ch = *p;
				}

				if (ch == L'\0')
				{
					state = state_error;
				}
				else if (ch == L'D' || ch == L'd')
				{
					if (days != -1)
					{
						state = state_error;
					}
					else
					{
						days = current_value;
						state = state_initial_or_ws;
						++p;
					}
				}
				else if (ch == L'H' || ch == L'h')
				{
					if (hours != -1)
					{
						state = state_error;
					}
					else
					{
						hours = current_value;
						state = state_initial_or_ws;
						++p;
					}
				}
				else if (ch == L'M' || ch == L'm')
				{
					if (minutes != -1)
					{
						state = state_error;
					}
					else
					{
						minutes = current_value;
						state = state_initial_or_ws;
						++p;
					}
				}
				else if (ch == L'S' || ch == L's')
				{
					if (seconds != -1)
					{
						state = state_error;
					}
					else
					{
						seconds = current_value;
						state = state_initial_or_ws;
						++p;
					}
				}
				else
				{
					state = state_error;
				}
				break;

			default:
				state = state_error;
				break;
		}
	}

	// Validation and output
	if ((days == -1 && hours == -1 && minutes == -1 && seconds == -1) ||
		state == state_error)
	{
		*out_duration = 0;
		return false;
	}

	int32 total_seconds = 0;

	if (days != -1) total_seconds += days * 86400;
	if (hours != -1) total_seconds += hours * 3600;
	if (minutes != -1) total_seconds += minutes * 60;
	if (seconds != -1) total_seconds += seconds;

	*out_duration = total_seconds;
	return true;
}

bool kablam_command_parse_ip_cidr(
	wchar_t const* address_string,
	uint32* ip_out,
	int8* cidr_out)
{
	enum parse_state
	{
		state_initial = 0,
		state_octet_digits,
		state_expect_dot_or_cidr,
		state_cidr_digits,
		state_after_cidr,
		state_error,
		state_done
	};

	parse_state  state = state_initial;
	uint32 ip_value = 0;
	uint32 octet_index = 0;
	int32 current_value = 0;
	int32 digits_in_value = 0;
	int8 cidr_value = -1;

	wchar_t const* p = address_string;

	while (state != state_done && state != state_error)
	{
		wchar_t ch = *p;

		switch (state)
		{
		case state_initial:
			if (iswdigit(ch))
			{
				current_value = 0;
				digits_in_value = 0;
				state = state_octet_digits;
				continue;
			}
			if (ch == L'\0')
			{
				state = state_error;
			}
			else
			{
				p++;
			}
			break;

		case state_octet_digits:
			if (iswdigit(ch))
			{
				int32 digit = (int32)(ch - L'0');
				current_value = current_value * 10 + digit;
				digits_in_value++;

				if (current_value > 255)
				{
					state = state_error;
				}
				else
				{
					p++;
				}
			}
			else
			{
				if (digits_in_value == 0)
				{
					state = state_error;
					p++;
					break;
				}

				if (!kablam_set_ipv4_octet(&ip_value, &octet_index, current_value))
				{
					state = state_error;
					break;
				}

				current_value = 0;
				digits_in_value = 0;

				if (octet_index == 4)
				{
					state = state_expect_dot_or_cidr;
				}
				else
				{
					if (ch == L'.')
					{
						p++;
						state = state_octet_digits;
					}
					else
					{
						state = state_error;
						p++;
					}
				}
			}
			break;

		case state_expect_dot_or_cidr:
			if (ch == L'/')
			{
				p++;
				current_value = 0;
				digits_in_value = 0;
				state = state_cidr_digits;
			}
			else
			{
				cidr_value = 32;
				state = state_after_cidr;
			}
			break;

		case state_cidr_digits:
			if (iswdigit(ch))
			{
				int32 digit = (int)(ch - L'0');
				current_value = current_value * 10 + digit;
				digits_in_value++;

				if (current_value > 32)
				{
					state = state_error;
					p++;
				}
				else
				{
					p++;
				}
			}
			else
			{
				if (digits_in_value > 0)
				{
					cidr_value = (int8)current_value;
					state = state_after_cidr;
				}
				else
				{
					state = state_error;
					p++;
				}
			}
			break;

		case state_after_cidr:
			if (ch == L' ' || ch == L'\t')
			{
				p++;
			}
			else if (ch == L'\0')
			{
				state = state_done;
			}
			else
			{
				state = state_error;
				p++;
			}
			break;

		default:
			state = state_error;
			break;
		}
	}

	if (state == state_done && octet_index == 4 && cidr_value >= 0)
	{
		*ip_out = ip_value;
		*cidr_out = cidr_value;
		return true;
	}

	*ip_out = 0;
	*cidr_out = 0;
	return false;
}

bool kablam_command_parse_ip_cidr(
	wchar_t const* address_string,
	s_ipv4_subnet* ipv4_subnet)
{
	return kablam_command_parse_ip_cidr(address_string, (uint32*)&ipv4_subnet->ipv4_address, (int8*)&ipv4_subnet->cidr);
}

bool kablam_command_parse_mac_address(
	wchar_t const* mac_string,
	s_mac_address* out_mac)
{
	enum parse_state
	{
		state_scan_first_digit = 0,
		state_reading_byte,
		state_between_bytes,
		state_trailing,
		state_done,
		state_error
	};

	parse_state  state = state_scan_first_digit;
	uint32 octet_index = 0;
	int32 current_byte = 0;
	int32 digit_count = 0;

	wchar_t const* p = mac_string;

	while (!(state == state_done || state == state_error))
	{
		wchar_t ch = *p;

		switch (state)
		{
			case state_scan_first_digit:
			{
				int32 digit = hex_digit_value_from_wchar(ch);
				if (digit >= 0)
				{
					state = state_reading_byte;
					current_byte = digit;
					digit_count = 1;
					++p;
				}
				else
				{
					if (ch == L'\0')
					{
						state = state_done;
					}
					else
					{
						++p;
					}
				}
				break;
			}

			case state_reading_byte:
			{
				int32 digit = hex_digit_value_from_wchar(ch);
				if (digit >= 0)
				{
					current_byte = (current_byte << 4) | digit;
					++digit_count;
					++p;

					if (digit_count == 2)
					{
						if (octet_index >= 6)
						{
							state = state_error;
							break;
						}

						out_mac->octets[octet_index++] = (int8)current_byte;

						if (octet_index == 6)
							state = state_trailing;
						else
							state = state_between_bytes;

						current_byte = 0;
						digit_count = 0;
					}
				}
				else
				{
					state = state_error;
				}
				break;
			}

			case state_between_bytes:
			{
				int32 digit = hex_digit_value_from_wchar(ch);
				if (digit >= 0)
				{
					state = state_reading_byte;
					current_byte = digit;
					digit_count = 1;
					++p;
				}
				else if (ch == L'-' || ch == L':')
				{
					++p;
					state = state_reading_byte;
					current_byte = 0;
					digit_count = 0;
				}
				else if (ch == L'\0')
				{
					state = state_done;
				}
				else
				{
					state = state_error;
				}
				break;
			}

			case state_trailing:
				if (ch == L' ' || ch == L'\t')
				{
					++p;
				}
				else if (ch == L'\0')
				{
					state = state_done;
				}
				else
				{
					state = state_error;
				}
				break;

			default:
				state = state_error;
				break;
		}
	}

	if (state != state_done || octet_index < 6)
	{
		out_mac->octets[0] = 0;
		out_mac->octets[1] = 0;
		out_mac->octets[2] = 0;
		out_mac->octets[3] = 0;
		out_mac->octets[4] = 0;
		out_mac->octets[5] = 0;
		return false;
	}

	return true;
}

/* private code */

static int32 hex_digit_value_from_wchar(
	wchar_t ch)
{
	if (ch >= L'0' && ch <= L'9')
		return ch - L'0';

	if (ch >= L'A' && ch <= L'F')
		return ch - L'A' + 10;

	if (ch >= L'a' && ch <= L'f')
		return ch - L'a' + 10;

	return -1;
}

static bool kablam_set_ipv4_octet(
	uint32* ip_value,
	uint32* octet_index, 
	uint32 value)
{
	unsigned char* ip_bytes;

	if (*octet_index >= 4)
		return false;

	ip_bytes = (unsigned char*)ip_value;
	ip_bytes[3 - *octet_index] = (unsigned char)value;
	(*octet_index)++;

	return true;
}
