#include "stdafx.h"
#include "network_configuration.h"

#include "cseries/cseries_strings.h"
#include "text/unicode.h"

/* constants */

enum
{
    k_network_adapter_max_count = 16,

    // Minimum MTU size that is deemeed "Sufficient" for the Ethernet II framing
    k_network_adapter_minimum_mtu = 1500,   
	
    k_adapter_working_buffer_size = 15000,
	k_get_adapter_max_tries = 3
};

/* prototypes */

uint32* network_adapter_get_count(void);
s_network_adapter* network_adapters_get(void);

// Populates adapter_address parameter passed with all the network adapters connected to the device
// Make sure that adapter_address is free'd using CSERIES_FREE after it's no longer used
uint32 network_adapter_addresses_populate(IP_ADAPTER_ADDRESSES** adapter_address);

// Checks if the adapter_address parameter passed is already populated in our global adapter list
// Return true if it already exists and false if we need to add it
bool network_adapter_is_duplicate(const IP_ADAPTER_ADDRESSES* adapter_address);

// Loop through list of network adapters from the adapter_address parameter
// Populate our global list until we run out of adapters or we fill the global list
void network_adapter_populate_globals(IP_ADAPTER_ADDRESSES* adapter_address);

void __cdecl load_network_adapter(void);

/* public code */

void network_configuration_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x1AC071, 0x1AC576), get_network_adapters);
    WriteJmpTo(Memory::GetAddress(0x215C68, 0x1FD500), get_network_adapters);
    return;
}

int32 __cdecl network_adapter_index_get(void)
{
    return INVOKE(0x1AAEFF, 0x0, network_adapter_index_get);
}

const char* __cdecl network_adapter_name_get(int32 network_adapter_index)
{
    return INVOKE(0x1AB006, 0x0, network_adapter_name_get, network_adapter_index);
}

void __cdecl network_configuration_initialize(void)
{
	INVOKE(0x1A9DE6, 0x0, network_configuration_initialize);
	return;
}

void __cdecl get_network_adapters(void)
{
    uint32* network_adapter_count = network_adapter_get_count();
    *network_adapter_count = 0;

	IP_ADAPTER_ADDRESSES* adapter_address = NULL;

    uint32 error = network_adapter_addresses_populate(&adapter_address);

    if (!error)
    {
        network_adapter_populate_globals(adapter_address);
    }

    if (adapter_address)
    {
        CSERIES_FREE(adapter_address);
    }

    load_network_adapter();
    return;
}

/* private code */

uint32* network_adapter_get_count(void)
{
    return Memory::GetAddress<uint32*>(0x4F98C4, 0x523E14);
}

s_network_adapter* network_adapters_get(void)
{
    return Memory::GetAddress<s_network_adapter*>(0x4F98D0, 0x523E20);
}

uint32 network_adapter_addresses_populate(IP_ADAPTER_ADDRESSES** adapter_address)
{
    // Allocate a 15 KB buffer to start with.
    uint32 buffer_size = k_adapter_working_buffer_size;
    uint8 i = 0;

    uint32 error;
    do
    {
        // Allocate memory that contains all the adapters
        *adapter_address = (IP_ADAPTER_ADDRESSES*)CSERIES_MALLOC(buffer_size);
        if (*adapter_address == NULL)
        {
            error = GetLastError();
            break;
        }

        error = GetAdaptersAddresses(AF_INET, GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST, NULL, *adapter_address, &buffer_size);
        if (error != ERROR_BUFFER_OVERFLOW)
        {
            break;
        }

        // Free memory and begin the next iteration
        CSERIES_FREE(*adapter_address);
        *adapter_address = NULL;
        i++;
    }
    while (i < k_get_adapter_max_tries);

    return error;
}

bool network_adapter_is_duplicate(const IP_ADAPTER_ADDRESSES* adapter_address)
{
    bool duplicate_adapter_detected = false;
    const uint32 network_adapter_count = *network_adapter_get_count();

    // Check for duplicate adapters after the first one is added
    if (network_adapter_count > 0)
    {
        for (uint32 i = 0; i < network_adapter_count; i++)
        {
            s_network_adapter* adapter = &network_adapters_get()[i];
            int32 index = csstrncmp(adapter_address->AdapterName, adapter->adapter_name, NUMBEROF(s_network_adapter::adapter_name)) == 0;
            if (index)
            {
                duplicate_adapter_detected = true;
                break;
            }
        }
    }

    return duplicate_adapter_detected;
}

void network_adapter_populate_globals(IP_ADAPTER_ADDRESSES* adapter_address)
{
    uint32* network_adapter_count = network_adapter_get_count();

    do
    {
        bool ethernet_or_wifi = adapter_address->IfType == IF_TYPE_ETHERNET_CSMACD || adapter_address->IfType == IF_TYPE_IEEE80211;
        if (ethernet_or_wifi && adapter_address->Mtu >= k_network_adapter_minimum_mtu)
        {
            if (!network_adapter_is_duplicate(adapter_address))
            {
                s_network_adapter* adapter = &network_adapters_get()[*network_adapter_count];
                csstrnzcpy(adapter->adapter_name, adapter_address->AdapterName, NUMBEROF(s_network_adapter::adapter_name));
                ustrncpy(adapter->friendly_name, adapter_address->FriendlyName, NUMBEROF(s_network_adapter::friendly_name));
                ustrncpy(adapter->description, adapter_address->Description, NUMBEROF(s_network_adapter::description));
                ++*network_adapter_count;
            }
        }
        adapter_address = adapter_address->Next;
    }
    while (adapter_address && *network_adapter_count < k_network_adapter_max_count);
    
    return;
}

void __cdecl load_network_adapter(void)
{
    INVOKE(0x1AB0BC, 0x1AB5C1, load_network_adapter);
    return;
}
