#pragma once

/* constants */

#define k_number_of_transition_functions 8

/* structures */

struct network_address
{
	union
	{
		int ipv4;
		char ipv6[16];
	} address;
	short port;
	short address_type;
};
ASSERT_STRUCT_SIZE(network_address, 20);

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
