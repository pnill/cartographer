#pragma once

/* constants */

enum
{
	k_number_of_transition_functions = 8
};

/* enums */

enum e_transport_type : int32
{
	_transport_type_udp = 2,
	_transport_type_vdp,
	_transport_type_tcp,
	k_protocol_count
};

enum e_transport_platform : int32
{
	_transport_platform_xbox,
	_transport_platform_windows,
	k_transport_platform_count
};

/* structures */

struct s_transport_globals
{
	bool initialized;
	bool field_1;
	bool field_2;
	int8 pad;
	int32 transition_function_count;
	void* startup_functions[k_number_of_transition_functions];
	void* shutdown_functions[k_number_of_transition_functions];
	void* reset_functions[k_number_of_transition_functions];
	void* reset_parameters[k_number_of_transition_functions];
};
ASSERT_STRUCT_SIZE(s_transport_globals, 136);

/* prototypes */

void network_transport_apply_patches(void);

s_transport_globals* transport_globals_get(void);

bool transport_available(void);

int32 __cdecl transport_get_packet_overhead_hook(int32 transport_type);

void __cdecl transport_register_transition_functions(
	void* startup_func,
	void* shutdown_func,
	void* reset_func,
	void* reset_parameter);
