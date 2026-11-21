#pragma once

/* constants */

enum
{
    _network_executable_type_tags = 0,
    _network_executable_type_symbols,
    _network_executable_type_debug,
    _network_executable_type_profile,
    _network_executable_type_release,
    k_network_executable_type_count,
    k_network_executable_type_bits = 3,
};

/* enums */


/* structures */

struct s_network_locator_result
{
	uint8 gap_0[116];
	int16 executable_type;
	int32 executable_version;
	int32 compatible_version;
};
ASSERT_STRUCT_OFFSET(s_network_locator_result, executable_type, 116);

/* prototypes */

void network_utilities_apply_patches();
void __cdecl network_utilities_get_game_version(int32* executable_type, int32* executable_version, int32* compatible_version);

void bandwidth_profiler_record_push(int32 usage_type, class c_bitstream* bitstream);
void bandwidth_profiler_record_pop(int32 usage_type, class c_bitstream* bitstream);
