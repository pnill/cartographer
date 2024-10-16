#pragma once

/* structures */

struct s_network_adapter
{
	char adapter_name[64];
	wchar_t friendly_name[64];
	wchar_t description[64];
};

/* prototypes */

void network_configuration_apply_patches(void);

int32 __cdecl network_adapter_index_get(void);

const char* __cdecl network_adapter_name_get(int32 network_adapter_index);

// initializes some interface that's used to download the network config from the bungie's website but just a stub in release
void __cdecl network_configuration_initialize(void);

void __cdecl get_network_adapters(void);
