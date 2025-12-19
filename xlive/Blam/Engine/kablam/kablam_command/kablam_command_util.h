#pragma once

void kablam_command_print_help_text(int32 description_string_id, int32 usage_string_id);

void kablam_command_print_playlist_warning(struct kablam_command_playlist_warning* warning);

bool kablam_command_parse_duration_string(const wchar_t* duration_string, int32* out_duration);

bool kablam_command_parse_ip_cidr(const wchar_t* address_string, uint32* ip_out, int8* cidr_out);

bool kablam_command_parse_ip_cidr(const wchar_t* address_string, struct s_ipv4_subnet* ipv4_subnet);

bool kablam_command_parse_mac_address(const wchar_t* mac_string, struct s_mac_address* out_mac);